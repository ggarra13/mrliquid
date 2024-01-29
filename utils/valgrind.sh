#!/bin/bash --norc



# valgrind --trace-children=yes  --suppressions=utils/python.supp --suppressions=utils/X11.supp  --suppressions=utils/libc.supp  --suppressions=utils/maya.supp --gen-suppressions=yes --leak-check=full --show-reachable=yes /usr/autodesk/maya$MAYA_VERSION/bin/Maya${MAYA_VERSION%-x64} $*

valgrind --trace-children=yes --suppressions=utils/python.supp --suppressions=utils/X11.supp  --suppressions=utils/libc.supp  --suppressions=utils/maya.supp /usr/autodesk/maya$MAYA_VERSION/bin/Maya${MAYA_VERSION%-x64} $*
