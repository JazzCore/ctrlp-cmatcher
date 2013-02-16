" =============================================================================
" File:          autoload/matcher.vim
" Description:   CtrlP C matching extension
" Author:        Stanislav Golovanov <stgolovanov@gmail.com>
" Version:       0.5.1
" =============================================================================

fu! s:matchtabs(item, pat)
	retu match(split(a:item, '\t\+')[0], a:pat)
endf

fu! s:cmatcher(lines,input,limit,mmode, ispath, crfile, regex)
python << EOF
import vim
import re
import fuzzycomt
lines = vim.eval('a:lines')
searchinp = vim.eval('a:input')
limit = int(vim.eval('a:limit'))
mmode = vim.eval('a:mmode')
ispath = int(vim.eval('a:ispath'))
crfile = vim.eval('a:crfile')
#TODO fallback to ctrlp matching?
regex = vim.eval('a:regex')

if ispath and crfile:
  lines.remove(crfile)

matchlist = fuzzycomt.match(lines, searchinp, limit, mmode)
vim.command('let matchlist = %s' % matchlist)
EOF
retu matchlist
endf

fu! s:highlight(input, mmode)
    " highlight matches
    " TODO make it case-unsensitive
    cal clearmatches()
    if a:mmode == "filename-only"
      for i in range(len(a:input))
        let pat = substitute(a:input[i], '\[\^\(.\{-}\)\]\\{-}', '[^\\/\1]\\{-}', 'g')
        let pat = substitute(a:input[i], '\$\@<!$', '\\ze[^\\/]*$', 'g')
        cal matchadd('CtrlPMatch', '\C'.pat)
      endfor
    el
      for i in range(len(a:input))
        cal matchadd('CtrlPMatch', '\M'.a:input[i])
      endfor
    en
    cal matchadd('CtrlPLinePre', '^>')
endf

fu! matcher#cmatch(lines,input,limit,mmode, ispath, crfile, regex)
  if a:input == ''
    let array = a:lines[0:a:limit]
    if a:ispath && !empty(a:crfile)
      cal remove(array, index(array, a:crfile))

    retu array
    en
  el
    " use built-in matcher if mmode set to match until first tab ( in other case
    " tag.vim doesnt work
    if a:mmode == "first-non-tab"
      let array = []
      for item in a:lines
        if call(s:matchtabs, [item,a:input]) >= 0
          cal add(array,item)
        en
      endfo
      retu array
    en
    let matchlist = s:cmatcher(a:lines,a:input,a:limit,a:mmode, a:ispath, a:crfile, a:regex)
    let array = []

    " Show only lines where we matched something ( value > 0 )
    for line in matchlist
      if line['value'] > 0
        "This is needed to fix \\ in paths on Windows
        cal add(array, substitute(line['line'],'\\\\','\\','g'))
      endif
    endfor
  en

  cal s:highlight(a:input, a:mmode)

  retu array
endf
