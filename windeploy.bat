
mkdir %~dp0..\XMusic-win32
mkdir %~dp0..\XMusic-win32\font
mkdir %~dp0..\XMusic-win32\hbkg
mkdir %~dp0..\XMusic-win32\vbkg
mkdir %~dp0..\XMusic-win32\iconengines
mkdir %~dp0..\XMusic-win32\imageformats
mkdir %~dp0..\XMusic-win32\platforms
mkdir %~dp0..\XMusic-win32\styles

cd /d bin

copy /Y D3Dcompiler_47.dll ..\..\XMusic-win32
copy /Y opengl32sw.dll ..\..\XMusic-win32

copy /Y avutil-56.dll ..\..\XMusic-win32
copy /Y avformat-58.dll ..\..\XMusic-win32
copy /Y avcodec-58.dll ..\..\XMusic-win32
copy /Y swresample-3.dll ..\..\XMusic-win32

copy /Y SDL2.dll ..\..\XMusic-win32

copy /Y CURLTool.dll ..\..\XMusic-win32

copy /Y XMusic.exe ..\..\XMusic-win32

copy /Y x*.dll ..\..\XMusic-win32

copy /Y lib*.dll ..\..\XMusic-win32

copy /Y Qt5*.dll ..\..\XMusic-win32

copy font\* ..\..\XMusic-win32\font
copy hbkg\win10 ..\..\XMusic-win32\hbkg
copy vbkg\win10 ..\..\XMusic-win32\vbkg
copy iconengines\* ..\..\XMusic-win32\iconengines
copy imageformats\* ..\..\XMusic-win32\imageformats
copy platforms\* ..\..\XMusic-win32\platforms
copy styles\* ..\..\XMusic-win32\styles

pause
