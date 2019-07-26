rd /S /Q .vs

cd /d %~dp0bin

del *.dmp

del *.log

del *.pdb
del *.ipdb
del *.iobj
del *.ilk

del *.cpp
del *.o
del *.a


cd /d %~dp0src/2player

del qrc_res.cpp
del *.obj
del makefile*

pause
