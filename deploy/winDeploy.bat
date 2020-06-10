
rd /S /Q XMusic-win32\bin

mkdir XMusic-win32\bin
mkdir XMusic-win32\bin\font
mkdir XMusic-win32\bin\bkg
mkdir XMusic-win32\bin\bkg\hbkg
mkdir XMusic-win32\bin\bkg\vbkg
mkdir XMusic-win32\bin\bkg\hbkg\city
mkdir XMusic-win32\bin\bkg\vbkg\city
mkdir XMusic-win32\bin\iconengines
mkdir XMusic-win32\bin\imageformats
mkdir XMusic-win32\bin\platforms

copy bkg\*				XMusic-win32\bin\bkg
copy bkg\hbkg\*			XMusic-win32\bin\bkg\hbkg
copy bkg\vbkg\*			XMusic-win32\bin\bkg\vbkg
copy bkg\hbkg\city\*	XMusic-win32\bin\bkg\hbkg\city
copy bkg\vbkg\city\*	XMusic-win32\bin\bkg\vbkg\city

cd /d ../bin

copy /Y avutil-56.dll 	%~dp0XMusic-win32\bin
copy /Y avformat-58.dll %~dp0XMusic-win32\bin
copy /Y avcodec-58.dll 	%~dp0XMusic-win32\bin
copy /Y swresample-3.dll %~dp0XMusic-win32\bin

copy /Y SDL2.dll 		%~dp0XMusic-win32\bin

copy /Y XMusic.exe 		%~dp0XMusic-win32\bin

copy /Y x*.dll 			%~dp0XMusic-win32\bin

copy /Y lib*.dll 		%~dp0XMusic-win32\bin

copy /Y Qt5*.dll 		%~dp0XMusic-win32\bin

rem copy font\msyhl-6.23.ttc %~dp0XMusic-win32\bin\font

copy platforms\* 		%~dp0XMusic-win32\bin\platforms
copy imageformats\* 	%~dp0XMusic-win32\bin\imageformats
rem copy iconengines\* 	%~dp0XMusic-win32\bin\iconengines

pause
