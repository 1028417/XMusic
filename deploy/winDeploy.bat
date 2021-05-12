
rd /S /Q XMusic-win32\bin

mkdir XMusic-win32\bin
rem mkdir XMusic-win32\bin\font
rem mkdir XMusic-win32\bin\iconengines
mkdir XMusic-win32\bin\imageformats
mkdir XMusic-win32\bin\platforms

mkdir XMusic-win32\bin\bkg
mkdir XMusic-win32\bin\bkg\hbkg
mkdir XMusic-win32\bin\bkg\vbkg
mkdir XMusic-win32\bin\bkg\hbkg\city
mkdir XMusic-win32\bin\bkg\vbkg\city

copy ..\..\des\bkg\*				XMusic-win32\bin\bkg
copy ..\..\des\bkg\hbkg\*			XMusic-win32\bin\bkg\hbkg
copy ..\..\des\bkg\vbkg\*			XMusic-win32\bin\bkg\vbkg
copy ..\..\des\bkg\hbkg\city\*	XMusic-win32\bin\bkg\hbkg\city
copy ..\..\des\bkg\vbkg\city\*	XMusic-win32\bin\bkg\vbkg\city

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
rem copy iconengines\* 	%~dp0XMusic-win32\bin\iconengines
copy imageformats\* 	%~dp0XMusic-win32\bin\imageformats
copy platforms\* 		%~dp0XMusic-win32\bin\platforms

pause
