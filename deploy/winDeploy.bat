
rd /S /Q %~dp0XMusic-win32\bin

mkdir %~dp0XMusic-win32\bin
mkdir %~dp0XMusic-win32\bin\font
mkdir %~dp0XMusic-win32\bin\bkg
mkdir %~dp0XMusic-win32\bin\bkg\hbkg
mkdir %~dp0XMusic-win32\bin\bkg\vbkg
mkdir %~dp0XMusic-win32\bin\bkg\hbkg\city
mkdir %~dp0XMusic-win32\bin\bkg\vbkg\city
mkdir %~dp0XMusic-win32\bin\iconengines
mkdir %~dp0XMusic-win32\bin\imageformats
mkdir %~dp0XMusic-win32\bin\platforms

cd /d ../bin

copy /Y avutil-56.dll ..\deploy\XMusic-win32\bin
copy /Y avformat-58.dll ..\deploy\XMusic-win32\bin
copy /Y avcodec-58.dll ..\deploy\XMusic-win32\bin
copy /Y swresample-3.dll ..\deploy\XMusic-win32\bin

copy /Y SDL2.dll ..\deploy\XMusic-win32\bin

copy /Y XMusic.exe ..\deploy\XMusic-win32\bin

copy /Y x*.dll ..\deploy\XMusic-win32\bin

copy /Y lib*.dll ..\deploy\XMusic-win32\bin

copy /Y Qt5*.dll ..\deploy\XMusic-win32\bin

copy font\msyhl-6.23.ttc ..\deploy\XMusic-win32\bin\font

copy  %~dp0..\..\bkg\* ..\deploy\XMusic-win32\bin\bkg
copy  %~dp0..\..\bkg\hbkg\* ..\deploy\XMusic-win32\bin\bkg\hbkg
copy  %~dp0..\..\bkg\vbkg\* ..\deploy\XMusic-win32\bin\bkg\vbkg
copy  %~dp0..\..\bkg\hbkg\city\* ..\deploy\XMusic-win32\bin\bkg\hbkg\city
copy  %~dp0..\..\bkg\vbkg\city\* ..\deploy\XMusic-win32\bin\bkg\vbkg\city

copy iconengines\* ..\deploy\XMusic-win32\bin\iconengines
copy imageformats\* ..\deploy\XMusic-win32\bin\imageformats
copy platforms\* ..\deploy\XMusic-win32\bin\platforms

pause
