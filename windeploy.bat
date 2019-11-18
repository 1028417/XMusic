
mkdir %~dp0..\XMusic_win32
mkdir %~dp0..\XMusic_win32\font
mkdir %~dp0..\XMusic_win32\hbkg
mkdir %~dp0..\XMusic_win32\vbkg
mkdir %~dp0..\XMusic_win32\iconengines
mkdir %~dp0..\XMusic_win32\imageformats
mkdir %~dp0..\XMusic_win32\platforms
mkdir %~dp0..\XMusic_win32\styles

cd /d bin

copy /Y D3Dcompiler_47.dll ..\..\XMusic_win32
copy /Y opengl32sw.dll ..\..\XMusic_win32

copy /Y avutil-56.dll ..\..\XMusic_win32
copy /Y avformat-58.dll ..\..\XMusic_win32
copy /Y avcodec-58.dll ..\..\XMusic_win32
copy /Y swresample-3.dll ..\..\XMusic_win32

copy /Y SDL2.dll ..\..\XMusic_win32

copy /Y CURLTool.dll ..\..\XMusic_win32

copy /Y XMusic.exe ..\..\XMusic_win32

copy /Y x*.dll ..\..\XMusic_win32

copy /Y lib*.dll ..\..\XMusic_win32

copy /Y Qt5*.dll ..\..\XMusic_win32

copy font\* ..\..\XMusic_win32\font
copy hbkg\win10 ..\..\XMusic_win32\hbkg
copy vbkg\win10 ..\..\XMusic_win32\vbkg
copy iconengines\* ..\..\XMusic_win32\iconengines
copy imageformats\* ..\..\XMusic_win32\imageformats
copy platforms\* ..\..\XMusic_win32\platforms
copy styles\* ..\..\XMusic_win32\styles

pause
