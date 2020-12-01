#-------------------------------------------------
#
# Project created by QtCreator 2019-03-27T01:29:13
#
#-------------------------------------------------

QT       += core gui widgets
android: QT += androidextras

RC_ICONS = "xmusic.ico"

TARGET = XMusic
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 #c++1y #gnu++1y

DEFINES += QT_DEPRECATED_WARNINGS

android {
HEADERS += cpp/androidutil.h
SOURCES += cpp/androidutil.cpp
}

SOURCES += ../controller.cpp \
    ../OptionMgr.cpp \
    cpp/MedialibView.cpp \
    cpp/centralwidget.cpp \
    cpp/painter.cpp \
    cpp/button.cpp \
    cpp/colordlg.cpp \
    cpp/app.cpp \
    cpp/mainwindow.cpp \
    cpp/PlayingList.cpp \
    cpp/singerimgdlg.cpp \
    cpp/widget.cpp \
    cpp/bkgdlg.cpp \
    cpp/listview.cpp \
    cpp/medialibdlg.cpp \
    cpp/addbkgdlg.cpp \
    cpp/label.cpp \
    cpp/dialog.cpp \
    cpp/msgbox.cpp \
    cpp/wholeTrackDlg.cpp \
    cpp/MLListView.cpp \
    cpp/networkWarnDlg.cpp \
    cpp/xmusic.cpp

HEADERS += cpp/app.h \
    ../OptionMgr.h \
    cpp/MedialibView.h \
    cpp/groupframe.h \
    cpp/painter.h \
    cpp/colorbar.h \
    cpp/colordlg.h \
    cpp/mainwindow.h \
    cpp/label.h \
    cpp/PlayingList.h \
    cpp/singerimgdlg.h \
    cpp/widget.h \
    cpp/button.h \
    cpp/progressbar.h \
    cpp/bkgdlg.h \
    cpp/listview.h \
    cpp/dialog.h \
    cpp/medialibdlg.h \
    cpp/addbkgdlg.h \
    ../../inc/ControllerDef.h \
    ../../inc/viewdef.h \
    cpp/msgbox.h \
    cpp/centralwidget.h \
    cpp/wholeTrackDlg.h \
    cpp/MLListView.h \
    cpp/networkWarnDlg.h \
    cpp/xmusic.h

RESOURCES += res.qrc

FORMS += mainwindow.ui  msgbox.ui  networkWarnDlg.ui \
         bkgdlg.ui  addbkgdlg.ui  colordlg.ui \
         medialibdlg.ui  wholeTrackDlg.ui \
         singerimgdlg.ui

INCLUDEPATH += ../../inc  cpp

BuildDir = ../../../build

BinDir = $$PWD/../../bin

macx {
    LIBS += -L$$BinDir/mac

    platform = mac
    DESTDIR = $$BinDir/mac

    libDir = $$PWD/../../../PlaySDK/bin/mac
    macDir = $$BinDir/mac
    frameworkDir = $$macDir/XMusic.app/Contents/Frameworks
    QMAKE_POST_LINK += \
            rm -f $$macDir/XMusic.dmg \
            && rm -rf $$frameworkDir \
            && mkdir $$frameworkDir \
#            && cp -rf $$PWD/../../deploy/bkg $$macDir/XMusic.app/Contents/MacOS/bkg \
            && echo $$macDir $$frameworkDir | xargs -n 1 cp -rfv \
            $$libDir/{SDL2.framework,libavcodec.58.dylib,libavformat.58.dylib,libavutil.56.dylib,libswresample.3.dylib} \
            && ~/Qt5.13.2/5.13.2/clang_64/bin/macdeployqt $$macDir/XMusic.app -libpath=$$macDir #-dmg

    LIBS += -lxutil.1  -lxPlaySDK.1  -lxMediaLib.1  -lxmodel.1
} else {
android {
    CommonLibDir = $$PWD/../../../Common2.1/libs/armeabi-v7a
    PlaySDKLibDir = $$PWD/../../../PlaySDK/libs/armeabi-v7a
    v7aLibDir = $$PWD/../../libs/armeabi-v7a
    LIBS += -L$$v7aLibDir -L$$CommonLibDir -L$$PlaySDKLibDir

    platform = android
    DESTDIR = $$v7aLibDir
} else: ios {
    LIBS += -L../../../Common2.1/libs/mac  -lssl  -lcrypto  #-lnghttp2

    LIBS += -L../../../PlaySDK/libs/ios/simulator  -lSDL2 \
            -framework AVFoundation  -framework GameController  -framework CoreMotion \
#
            -lavcodec  -lavformat  -lavutil  -lswresample \
            -framework CoreMedia  -framework VideoToolbox  -framework AVFoundation \
            -framework CoreVideo  -framework Security  -liconv  #-lbz2  -lz

    LIBS += -L$$BuildDir/ioslib

    platform = ios
    DESTDIR = $$BuildDir/XMusic
} else {
    SOURCES += ../Dump.cpp

    LIBS += -L$$BinDir -lDbghelp

    platform = win
    DESTDIR = $$BinDir
}

LIBS += -lxutil  -lxPlaySDK  -lxMediaLib  -lxmodel
}

#CONFIG += debug_and_release
CONFIG(debug, debug|release) {
BuildDir = $$BuildDir/XMusicd/$$platform
} else {
BuildDir = $$BuildDir/XMusic/$$platform
}

font.files += $$BinDir/font/msyhl-6.23.ttc \
              $$BinDir/font/Microsoft-YaHei-Regular-11.0.ttc

bkg.files += ../../deploy/bkg/

android {
    #CONFIG += mobility
    #MOBILITY =

    font.path = /assets/font/
    bkg.path = /assets/
    INSTALLS += font bkg

    DISTFILES += \
        android/src/xmusic/XActivity.java \
        android/AndroidManifest.xml \
        android/res/xml/file_paths.xml \        android/res/values/libs.xml \
        android/gradlew.bat \
        android/gradlew \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradle/wrapper/gradle-wrapper.properties \
        android/build.gradle

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        ANDROID_EXTRA_LIBS = \
            $$CommonLibDir/libssl.so \
            $$CommonLibDir/libcrypto.so \
            $$CommonLibDir/libxutil.so \
            $$PlaySDKLibDir/libxPlaySDK.so \
            $$PlaySDKLibDir/libavcodec.so \
            $$PlaySDKLibDir/libavformat.so \
            $$PlaySDKLibDir/libavutil.so \
            $$PlaySDKLibDir/libswresample.so \
            $$v7aLibDir/libxMediaLib.so \
            $$v7aLibDir/libxModel.so
    }
}

macx {
    ICON = xmusic.iconset/xmusic.icns

    QMAKE_INFO_PLIST += mac.plist

    font.path = Contents/MacOS/font
    bkg.path = Contents/MacOS/bkg
    QMAKE_BUNDLE_DATA += font bkg
}

ios {
    QMAKE_IOS_DEPLOYMENT_TARGET = 9.0

    #QMAKE_INFO_PLIST += iossimulator.plist #ios.plist

    font.path = /font
    bkg.path = /bkg
    QMAKE_BUNDLE_DATA += font bkg
} else {
MOC_DIR = $$BuildDir
RCC_DIR = $$BuildDir
UI_DIR = $$BuildDir
}
OBJECTS_DIR = $$BuildDir
