#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Run "python cp2txt.py" to copy source code files as TXT files.
#

import os

def utf8togbk(dir, dest, pre):
    for fn in os.listdir(dir):
        sfile = os.path.join(dir, fn)
        if os.path.isdir(sfile):
            if fn[0] != '.':
                utf8togbk(sfile, dest, pre + fn[:2] + '-')
            continue
        if fn.endswith('.h') or '.cpp' in fn or '.mm' in fn or '.java' in fn:
            text = open(sfile,'r').read()
            dfile = os.path.join(dest, '%s%s.txt' % (pre,fn))
            open(dfile, 'w').write(text)
            print(dfile)

if __name__=="__main__":
    dest = os.path.abspath('txt')
    utf8togbk(os.path.abspath('.'), dest, '')