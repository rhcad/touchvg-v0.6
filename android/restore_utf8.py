#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Run "python core/restore_utf8.py" to rename *.h.utf8 to *.h.
#

import os

def restore_utf8(dir):
    resultfn = ''
    for fn in os.listdir(dir):
        sfile = os.path.join(dir, fn)
        if os.path.isdir(sfile):
            resultfn += restore_utf8(sfile)
            continue
        if fn.endswith('.utf8'):
            orgfile = sfile[:-5]
            try:
                if os.path.exists(orgfile): os.remove(orgfile)
                os.rename(sfile, orgfile)
                resultfn += fn[:-5] + ' '
            except:
                print('except for %s' %(fn,))
    return resultfn

if __name__=="__main__":
    resultfn = restore_utf8(os.path.abspath('../core'))
    resultfn += restore_utf8(os.path.abspath('graph2d/src/touchvg'))
    if resultfn != '': print('restore files: ' + resultfn)
