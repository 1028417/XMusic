
cd ~/Desktop/dev/XMusic/bin/mac

mkdir XMusic.app/Contents/Frameworks

cp libavcodec.58.dylib XMusic.app/Contents/Frameworks
cp libavformat.58.dylib XMusic.app/Contents/Frameworks
cp libavutil.56.dylib XMusic.app/Contents/Frameworks
cp libswresample.3.dylib XMusic.app/Contents/Frameworks

cp -R SDL2.framework XMusic.app/Contents/Frameworks

~/Qt5.13.2/5.13.2/clang_64/bin/macdeployqt XMusic.app -libpath=./ -dmg
