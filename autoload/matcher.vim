fu! s:matchtabs(item, pat)
	retu match(split(a:item, '\t\+')[0], a:pat)
endf

fu! matcher#MatchItPy(lines,input,limit,mmode, ispath, crfile, regex)
  if a:input == ''
    let array = a:lines[0:a:limit]
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

#TODO Move it to external plugin to autoload/ and call matching as a global func
matchlist = fuzzycomt.match(lines, searchinp, limit, mmode)
vim.command('let matchlist = %s' % matchlist)
EOF
  let array = []

  for line in matchlist
    if line['value'] > 0
      "This is needed to fix \\ in paths on Windows
      cal add(array, substitute(line['line'],'\\\\','\\','g'))
    endif
  endfor
en

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

retu array
endf
