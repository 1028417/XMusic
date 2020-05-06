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

SOURCES += ../controller.cpp \
    cpp/MedialibView.cpp \
    cpp/painter.cpp \
    cpp/button.cpp \
    cpp/colordlg.cpp \
    cpp/main.cpp \
    cpp/app.cpp \
    cpp/mainwindow.cpp \
    cpp/PlayingList.cpp \
    cpp/widget.cpp \
    cpp/bkgdlg.cpp \
    cpp/listview.cpp \
    cpp/medialibdlg.cpp \
    cpp/addbkgdlg.cpp \
    cpp/label.cpp \
    cpp/dialog.cpp \
    cpp/msgbox.cpp \
    cpp/wholeTrackDlg.cpp \
    cpp/MLListView.cpp

HEADERS += cpp/app.h \
    cpp/MedialibView.h \
    cpp/groupframe.h \
    cpp/painter.h \
    cpp/colorbar.h \
    cpp/colordlg.h \
    cpp/mainwindow.h \
    cpp/label.h \
    cpp/PlayingList.h \
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
    cpp/MLListView.h

RESOURCES += res.qrc

FORMS += mainwindow.ui  bkgdlg.ui  medialibdlg.ui  addbkgdlg.ui \
    colordlg.ui \
    msgbox.ui \
    wholeTrackDlg.ui

INCLUDEPATH += ../../inc  cpp

BuildDir = ../../../build

BinDir = ../../bin

android {
    LIBS += -L../../libs/armeabi-v7a

    platform = android
    DESTDIR = ../../libs/armeabi-v7a
} else: macx {
    LIBS += -L$$BinDir/mac

    platform = mac
    DESTDIR = $$BinDir/mac

    libDir = $$PWD/../../../PlaySDK/bin/mac
    macDir = $$PWD/$$BinDir/mac
    frameworkDir = $$macDir/XMusic.app/Contents/Frameworks
    QMAKE_POST_LINK += \
        rm -f $$macDir/XMusic.dmg \
        && rm -rf $$frameworkDir \
        && mkdir $$frameworkDir \
        && echo $$macDir $$frameworkDir | xargs -n 1 cp -rfv \
            $$libDir/{SDL2.framework,libavcodec.58.dylib,libavformat.58.dylib,libavutil.56.dylib,libswresample.3.dylib} \
        && ~/Qt5.13.2/5.13.2/clang_64/bin/macdeployqt $$macDir/XMusic.app -libpath=$$macDir -dmg
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

#CONFIG += debug_and_release
CONFIG(debug, debug|release) {
BuildDir = $$BuildDir/XMusicd/$$platform
} else {
BuildDir = $$BuildDir/XMusic/$$platform
}

font.files += $$BinDir/font/msyhl-6.23.ttc
#font.files += $$BinDir/font/Microsoft-YaHei-Semilight-11.0.ttc
font.files += $$BinDir/font/Microsoft-YaHei-SemiBold-11.0.ttc

bkg.files += ../../bkg/*

android {
    #CONFIG += mobility
    #MOBILITY =

    font.path = /assets/font
    bkg.path = /assets/bkg
    INSTALLS += font bkg

    DISTFILES += \
        android/AndroidManifest.xml \
        android/res/values/libs.xml \
        android/build.gradle \
        android/gradlew.bat \
        android/gradlew \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradle/wrapper/gradle-wrapper.properties

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    v7aLibDir = $$PWD/../../libs/armeabi-v7a
    ffmpegLibDir= $$PWD/../../../PlaySDK/libs/armeabi-v7a/ffmpeg
    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        ANDROID_EXTRA_LIBS = \
            $$v7aLibDir/libxutil.so \
            $$v7aLibDir/libxMediaLib.so \
            $$v7aLibDir/libxModel.so \
            $$v7aLibDir/libssl.so \
            $$v7aLibDir/libcrypto.so \
            $$v7aLibDir/libxPlaySDK.so \
            $$ffmpegLibDir/libavcodec.so \
            $$ffmpegLibDir/libavformat.so \
            $$ffmpegLibDir/libavutil.so \
            $$ffmpegLibDir/libswresample.so
    }
}

macx {
    ICON = xmusic.iconset/xmusic.icns

    QMAKE_INFO_PLIST += mac.plist

    font.path = Contents/MacOS/font
    bkg.path = Contents/MacOS/bkg
    QMAKE_BUNDLE_DATA  += font bkg hbkg
}

ios {
    QMAKE_IOS_DEPLOYMENT_TARGET = 9.0

    #QMAKE_INFO_PLIST += iossimulator.plist #ios.plist

    font.path = /font
    bkg.path = /bkg
    QMAKE_BUNDLE_DATA  += font bkg
} else {
MOC_DIR = $$BuildDir
RCC_DIR = $$BuildDir
UI_DIR = $$BuildDir
}
OBJECTS_DIR = $$BuildDir

DISTFILES += android/src/xmusic/XActivity.java
