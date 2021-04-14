cd /d %~dp0

.\curl-7.74.0_2-win64-mingw\curl.exe -T XMusic-android.apk -u 1028417@qq.com:xMusic#2020 "https://xmusic-generic.pkg.coding.net/xmusic/app/XMusic-android.apk" >deployApk.echo
@set ret=fail
@set /p "ret=" <deployApk.echo
@echo %ret%
del deployApk.echo

.\curl-7.74.0_2-win64-mingw\curl.exe -T XMusic-win32.zip -u 1028417@qq.com:xMusic#2020 "https://xmusic-generic.pkg.coding.net/xmusic/app/XMusic-win32.zip" >deployWinZip.echo
@set ret=fail
@set /p "ret=" <deployWinZip.echo
@echo %ret%
del deployWinZip.echo

pause
