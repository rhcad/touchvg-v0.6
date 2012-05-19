#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

def utf8togbk(dir):
    for fn in os.listdir(dir):
        sfile = os.path.join(dir, fn)
        if os.path.isdir(sfile):
            utf8togbk(sfile)
            continue
        if '.h' in fn or '.c' in fn:
            try:
                text = open(sfile,'r',-1,'utf-8').read()
            except UnicodeDecodeError:
                continue
            except TypeError:
                text = open(sfile).read()
                try:
                    u = text.decode('utf-8')
                    text = u.encode('gbk')
                except UnicodeDecodeError:
                    continue
            text = text.replace('html;charset=UTF-8', 'html;charset=gbk')
            open(sfile, 'w').write(text)
            print(sfile)

if __name__=="__main__":
    utf8togbk(os.path.abspath('..'))
    utf8togbk(os.path.abspath('../../core'))
    input("Press <ENTER> to end.")
