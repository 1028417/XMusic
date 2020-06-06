
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

copy /Y avutil-56.dll ..\release\XMusic-win32\bin
copy /Y avformat-58.dll ..\release\XMusic-win32\bin
copy /Y avcodec-58.dll ..\release\XMusic-win32\bin
copy /Y swresample-3.dll ..\release\XMusic-win32\bin

copy /Y SDL2.dll ..\release\XMusic-win32\bin

copy /Y XMusic.exe ..\release\XMusic-win32\bin

copy /Y x*.dll ..\release\XMusic-win32\bin

copy /Y lib*.dll ..\release\XMusic-win32\bin

copy /Y Qt5*.dll ..\release\XMusic-win32\bin

copy font\msyhl-6.23.ttc ..\release\XMusic-win32\bin\font

copy  %~dp0..\..\bkg\* ..\release\XMusic-win32\bin\bkg
copy  %~dp0..\..\bkg\hbkg\* ..\release\XMusic-win32\bin\bkg\hbkg
copy  %~dp0..\..\bkg\vbkg\* ..\release\XMusic-win32\bin\bkg\vbkg
copy  %~dp0..\..\bkg\hbkg\city\* ..\release\XMusic-win32\bin\bkg\hbkg\city
copy  %~dp0..\..\bkg\vbkg\city\* ..\release\XMusic-win32\bin\bkg\vbkg\city

copy iconengines\* ..\release\XMusic-win32\bin\iconengines
copy imageformats\* ..\release\XMusic-win32\bin\imageformats
copy platforms\* ..\release\XMusic-win32\bin\platforms

pause
