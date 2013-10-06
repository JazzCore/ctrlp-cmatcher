" =============================================================================
" File:          autoload/matcher.vim
" Description:   CtrlP C matching extension
" Author:        Stanislav Golovanov <stgolovanov@gmail.com>
" Version:       0.7.3
" =============================================================================

" Use pyeval() or py3eval() for newer python versions or fall back to
" vim.command() if vim version is old
" This code is borrowed from Powerline
let s:matcher_pycmd = has('python') ? 'py' : 'py3'
let s:matcher_pyeval = s:matcher_pycmd.'eval'

if exists('*'. s:matcher_pyeval)
  let s:pyeval = function(s:matcher_pyeval)
else
  exec s:matcher_pycmd 'import json, vim'
  exec "function! s:pyeval(e)\n".
  \   s:matcher_pycmd." vim.command('return ' + json.dumps(eval(vim.eval('a:e'))))\n".
  \"endfunction"
endif

let s:script_folder_path = escape( expand( '<sfile>:p:h' ), '\' )
python << ImportEOF
import sys, os, vim
sys.path.insert( 0, os.path.abspath( vim.eval('s:script_folder_path' ) ) )
import fuzzycomt
sys.path.pop(0)
ImportEOF

fu! s:matchtabs(item, pat)
  return match(split(a:item, '\t\+')[0], a:pat)
endf

fu! s:matchfname(item, pat)
  let parts = split(a:item, '[\/]\ze[^\/]\+$')
  return match(parts[-1], a:pat)
endf

fu! s:cmatcher(lines, input, limit, mmode, ispath, crfile)
python << EOF
lines = vim.eval('a:lines')
searchinp = vim.eval('a:input')
limit = int(vim.eval('a:limit'))
mmode = vim.eval('a:mmode')
ispath = int(vim.eval('a:ispath'))
crfile = vim.eval('a:crfile')

if ispath and crfile:
  try:
    lines.remove(crfile)
  except ValueError:
    pass

try:
  matchlist = fuzzycomt.sorted_match_list(lines, searchinp, limit, mmode)
except:
  matchlist = []
EOF
return s:pyeval("matchlist")
endf

fu! s:highlight(input, mmode, regex)
    " highlight matches
    cal clearmatches()
    if a:regex
      let pat = ""
      if a:mmode == "filename-only"
          let pat = substitute(a:input, '\$\@<!$', '\\ze[^\\/]*$', 'g')
      en
      if empty(pat)
        let pat = substitute(a:input, '\\\@<!\^', '^> \\zs', 'g')
      en
      cal matchadd('CtrlPMatch', '\c'.pat)
    el
      " Build a pattern like /a.*b.*c/ from abc (but with .\{-} non-greedy
      " matchers instead)
      let pat = substitute(a:input, '.\ze.', '\0\\.\\{-}', 'g')
      let previous = ""
      for i in range(len(a:input))
        if a:mmode == "filename-only"
          "TODO Highlights slightly incorrectly
          let pat = substitute(a:input[i], '\$\@<!$', '\\ze[^\\/]*$', 'g')
          cal matchadd('CtrlPMatch', '\p'.pat)
        el
          let index = i * 7
          " Surround our current target letter with \zs and \ze so it only
          " actually matches that one letter, but has all preceding and trailing
          " letters as well.
          let letter = substitute(pat, '^.\{'.index.'}\zs.', '\\zs\0\\ze', '')
          " Now we matchadd for each letter, the basic form being:
          " ^.*\zsx\ze.*$, but with our pattern we built above for the letter,
          " and a negative lookahead ensuring that we only highlight the last
          " occurrence of our letters. We also ensure that our matcher is case
          " insensitive and has 'Very little magic'.
          cal matchadd('CtrlPMatch', '\V\c\^\.\*'.letter.'\(\.\*'.pat.'\)\@!\.\*\$')
        en
      endfor
    en
    cal matchadd('CtrlPLinePre', '^>')
endf

fu! matcher#cmatch(lines, input, limit, mmode, ispath, crfile, regex)
  if a:input == ''
    " Clear matches, that left from previous matches
    cal clearmatches()
    " Hack to clear s:savestr flag in SplitPattern, otherwise matching in
    " 'tag' mode will work only from 2nd char.
    cal ctrlp#call('s:SplitPattern', '')
    let array = a:lines[0:a:limit]
    if a:ispath && !empty(a:crfile)
      cal remove(array, index(array, a:crfile))
    en
    return array
  el
    if a:regex
      let array = []
      let func = a:mmode == "filename-only" ? 's:matchfname' : 'match'
      for item in a:lines
        if call(func, [item, a:input]) >= 0
          cal add(array, item)
        endif
      endfor
      cal sort(array, ctrlp#call('s:mixedsort'))
      cal s:highlight(a:input, a:mmode, a:regex)
      return array
    endif
    " use built-in matcher if mmode set to match until first tab ( in other case
    " tag.vim doesnt work
    if a:mmode == "first-non-tab"
      let array = []
      " call ctrlp.vim function to get proper input pattern
      let pat = ctrlp#call('s:SplitPattern', a:input)
      for item in a:lines
        if call('s:matchtabs', [item, pat]) >= 0
          cal add(array, item)
        en
      endfo
      "TODO add highlight
      cal sort(array, ctrlp#call('s:mixedsort'))
      return array
    en

    let matchlist = s:cmatcher(a:lines, a:input, a:limit, a:mmode, a:ispath, a:crfile)
  en

  cal s:highlight(a:input, a:mmode, a:regex)

  return matchlist
endf
