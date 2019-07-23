"%systemroot%\system32\regsvr32" /s softpub.dll
"%systemroot%\system32\regsvr32" /s wintrust.dll

cd /d %~dp0
"%systemroot%\system32\regsvr32" /s bin/MsgHook.dll
"%systemroot%\system32\regsvr32" /s bin/Shadow.dll
"%systemroot%\system32\regsvr32" /s bin/Qocx.ocx
"%systemroot%\system32\regsvr32" /s bin/QPlayer.ocx

"bin/PlaySpirit.exe" /regserver
"bin/PlaySpirit_dev.exe" /regserver

pause

start "" "bin/XMusicHost.exe"
