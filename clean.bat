rd /S /Q .vs

cd /d %~dp0bin

del *.dmp

del *.log

del *.pdb
del *.ipdb
del *.iobj
del *.ilk

del *.a

cd /d %~dp0src/XMusic

del *makefile*

pause
