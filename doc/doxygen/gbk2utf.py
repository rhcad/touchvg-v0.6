#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

def gbk2utf(dir):
    for fn in os.listdir(dir):
        sfile = os.path.join(dir, fn)
        if os.path.isdir(sfile):
            gbk2utf(sfile)
            continue
        if '.h' in fn or '.cpp' in fn:
            try:
                text = open(sfile,'r',-1,'gbk').read()
            except TypeError:
                text = open(sfile).read()
                try:
                    u = text.decode('gbk')
                    text = u.encode('utf-8')
                except UnicodeDecodeError:
                    continue
            open(sfile, 'w').write(text)
            print(sfile)

if __name__=="__main__":
    gbk2utf(os.path.abspath('../../android'))
