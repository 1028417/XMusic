rd /S /Q .vs

cd /d %~dp0bin

del *.dmp

del *.log

del *.pdb
del *.ipdb
del *.iobj
del *.ilk

del *.a

del XMusic.exe
del xmodel.dll
del xMedialib.dll
del xutil.dll
del xPlaySDK.dll

cd /d %~dp0src/XMusic

del *makefile*

pause
