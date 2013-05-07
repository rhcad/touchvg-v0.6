#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Run "python utf8togbk.py" to convert source code files to the GBK format on Windows.
#

import os, codecs, sys

reload(sys)
sys.setdefaultencoding('gbk')

def utf8togbk(dir):
    resultfn = ''
    for fn in os.listdir(dir):
        sfile = os.path.join(dir, fn)
        if os.path.isdir(sfile):
            resultfn += utf8togbk(sfile)
            continue
        if fn.endswith('.h') or fn.endswith('.cpp'):
            if os.path.exists(sfile + '.utf8'):
                continue
            try:
                text = open(sfile,'r',-1,'utf-8').read()
                oldtext = text
            except UnicodeDecodeError:
                continue
            except TypeError:
                text = open(sfile).read()
                oldtext = text
                try:
                    if text[:3] == codecs.BOM_UTF8:
                        u = text[3:].decode('utf-8')
                        text = u.encode('gbk')
                except UnicodeEncodeError:
                    continue
                except UnicodeDecodeError:
                    continue
            try:
                text = text.replace('\r\n','\n')
                text = text.replace('\n','\r\n')
                if cmp(text, oldtext) != 0:
                    os.rename(sfile, sfile + '.utf8')
                    open(sfile, 'wb').write(text)
                    resultfn += fn + ' '
                    st = os.stat(sfile + '.utf8')
                    os.utime(sfile, (st.st_atime, st.st_mtime))
            except:
                print('except for %s' %(fn,))
    return resultfn

if __name__=="__main__":
    resultfn = utf8togbk(os.path.abspath('.'))
    resultfn += utf8togbk(os.path.abspath('../core'))
    resultfn += utf8togbk(os.path.abspath('../android'))
    if resultfn != '': print('utf8->gbk: ' + resultfn)
