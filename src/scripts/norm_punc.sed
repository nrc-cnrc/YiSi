#!/bin/sed -f

# @file norm_punc.sed
# @brief Normalize punctuation and other non-standard characters.
#
# @author Jackie Lo
#
# Traitement multilingue de textes / Multilingual Text Processing
# Centre de recherche en technologies numériques / Digital Technologies Research Centre
# Conseil national de recherches Canada / National Research Council Canada
# Copyright 2018, Sa Majeste la Reine du Chef du Canada /
# Copyright 2018, Her Majesty in Right of Canada

# Alternative numerals
s/０/0/g
s/１/1/g
s/２/2/g
s/３/3/g
s/４/4/g
s/５/5/g
s/６/6/g
s/７/7/g
s/８/8/g
s/９/9/g
# Numeral bullets
s/⒈/1./g
s/⒉/2./g
s/⒊/3./g
s/⒋/4./g
s/⒌/5./g
s/⒍/6./g
s/⒎/7./g
s/⒏/8./g
s/⒐/9./g
s/⒑/10./g
s/⒒/11./g
s/⒔/13./g
s/⒕/14./g
s/⒖/15./g
s/⒗/16./g
s/⒘/17./g
s/⒙/18./g
s/⒚/19./g
s/⒛/20./g
# Bracketed numerals
s/⑴/(1)/g
s/⑵/(2)/g
s/⑶/(3)/g
s/⑷/(4)/g
s/⑸/(5)/g
s/⑹/(6)/g
s/⑺/(7)/g
s/⑻/(8)/g
s/⑼/(9)/g
s/⑽/(10)/g
s/⑾/(11)/g
s/⑿/(12)/g
s/⒀/(13)/g
s/⒁/(14)/g
s/⒂/(15)/g
s/⒃/(16)/g
s/⒄/(17)/g
s/⒅/(18)/g
s/⒆/(19)/g
s/⒇/(20)/g
# Circled numerals
s/①/1/g
s/②/2/g
s/③/3/g
s/④/4/g
s/⑤/5/g
s/⑥/6/g
s/⑦/7/g
s/⑧/8/g
s/⑨/9/g
s/⑩/10/g
# Bracketed Chinese numerals
s/㈠/(一)/g
s/㈡/(二)/g
s/㈢/(三)/g
s/㈣/(四)/g
s/㈤/(五)/g
s/㈥/(六)/g
s/㈦/(七)/g
s/㈧/(八)/g
s/㈨/(九)/g
s/㈩/(十)/g
# Chinese capital Latin letters
s/Ａ/A/g
s/Ｂ/B/g
s/Ｃ/C/g
s/Ｄ/D/g
s/Ｅ/E/g
s/Ｆ/F/g
s/Ｇ/G/g
s/Ｈ/H/g
s/Ｉ/I/g
s/Ｊ/J/g
s/Ｋ/K/g
s/Ｌ/L/g
s/Ｍ/M/g
s/Ｎ/N/g
s/Ｏ/O/g
s/Ｐ/P/g
s/Ｑ/Q/g
s/Ｒ/R/g
s/Ｓ/S/g
s/Ｔ/T/g
s/Ｕ/U/g
s/Ｖ/V/g
s/Ｗ/W/g
s/Ｘ/X/g
s/Ｙ/Y/g
s/Ｚ/Z/g
# Chinese lower-case Lating letters
s/ａ/a/g
s/ｂ/b/g
s/ｃ/c/g
s/ｄ/d/g
s/ｅ/e/g
s/ｆ/f/g
s/ｇ/g/g
s/ｈ/h/g
s/ｉ/i/g
s/ｊ/j/g
s/ｋ/k/g
s/ｌ/l/g
s/ｍ/m/g
s/ｎ/n/g
s/ｏ/o/g
s/ｐ/p/g
s/ｑ/q/g
s/ｒ/r/g
s/ｓ/s/g
s/ｔ/t/g
s/ｕ/u/g
s/ｖ/v/g
s/ｗ/w/g
s/ｘ/x/g
s/ｙ/y/g
s/ｚ/z/g
# Roman lower-case numerals
s/ⅰ/i/g
s/ⅱ/ii/g
s/ⅳ/iv/g
s/ⅴ/v/g
s/ⅵ/vi/g
s/ⅶ/vii/g
s/ⅷ/viii/g
s/ⅸ/ix/g
s/ⅹ/x/g
s/ⅺ/xi/g
s/ⅻ/xii/g
# Roman capital numerals
s/Ⅰ/I/g
s/Ⅱ/II/g
s/Ⅲ/III/g
s/Ⅳ/IV/g
s/Ⅴ/V/g
s/Ⅵ/VI/g
s/Ⅶ/VII/g
s/Ⅷ/VIII/g
s/Ⅸ/IX/g
s/Ⅹ/X/g
s/Ⅺ/XI/g
s/Ⅻ/XII/g
# Quantity glyphs
s/㏕/mil/g
s/㎜/mm/g
s/㎝/cm/g
s/㎞/km/g
s/㏎/KM/g
s/㎎/mg/g
s/㎏/kg/g
s/㏄/cc/g
# Math function glyphs
s/㏒/log/g
s/㏑/ln/g
# Private Use Area characters
# These are not really part of unicode...
s//，/g
s//:/g
s//！/g
s//|/g
# Alternate punctuation
s/﹖/？/g
s/﹗/！/g
s/﹐/，/g
s/﹑/、/g
s//。/g
s//、/g
s//;/g
s//？/g
s/；/;/g
s/﹔/;/g
s/﹕/:/g
s/︰/:/g
s/｜/|/g
s/︱/|/g
s/︳/|/g
s/︱/|/g
s/︳/|/g
s/┃/|/g
s/┇/|/g
s/┋/|/g
s/　/ /g
s/＋/+/g
s/﹢/+/g
s/﹤/</g
s/﹥/>/g
s/﹦/=/g
s/％/%/g
s/﹪/%/g
s/﹫/@/g
s/＠/@/g
s/﹩/$/g
s/－/-/g # Fullwidth hyphen-minus
s/ˉ/-/g
s/ˉ/-/g
s/–/-/g
s/―/--/g
s/﹣/-/g
s/━/-/g
s/┅/---/g
s/┉/----/g
s/﹉/---/g
s/﹊/---/g
s/﹍/---/g
s/﹎/---/g
s/～/~/g
s/﹟/#/g
s/﹠/\&/g
s/＆/\&/g
s/﹡/\*/g
s/＊/\*/g
s/□/./g
s/•/./g
s/．/./g
s/﹒/./g
s/·/./g
s/˙/./g
s/…/.../g
s/‥/../g
s=／=/=g
s=∕=/=g
s=﹨=\\=g
s/（/(/g
s/）/)/g
s/﹙/(/g
s/﹚/)/g
s/﹛/{/g
s/﹜/}/g
s/﹝/(/g
s/【/(/g
s/】/)/g
s/“/\"/g
s/”/\"/g
s/〝/\"/g
s/〞/\"/g
s/‵/\'/g
s/’/\'/g
s/ˋ/\'/g
s/ˊ/\'/g
s/ˋ/\'/g
s/ˊ/\'/g
# Chinese numerals
s/○/零/g
s/一0/一零/g
s/二00/二零零/g
s/二0/二零/g
s/三0/三零/g
s/四0/四零/g
s/五0/五零/g
s/六0/六零/g
s/七0/七零/g
s/八0/八零/g
s/九0/九零/g
s/零0/零零/g
# Armenian
s/՚/\'/g
s/՜/!/g
s/՝/,/g
s/՞/?/g
s/։/./g
s/֊/-/g
# Arabic
s/،/,/g
s/؛/;/g
s/؞/.../g
s/؟/?/g
s/٪/%/g
s/٫/./g
s/٬/,/g
s/٭/*/g

# Excessive spacing
s/\s\s*/ /g
s/^\s*//g
s/\s*$//g
