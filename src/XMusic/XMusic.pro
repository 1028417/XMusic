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
    cpp/ListViewEx.cpp \
    cpp/label.cpp \
    cpp/dialog.cpp \
    cpp/msgbox.cpp

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
    cpp/ListViewEx.h \
    ../../inc/ControllerDef.h \
    ../../inc/viewdef.h \
    cpp/msgbox.h

RESOURCES += res.qrc

FORMS += mainwindow.ui  bkgdlg.ui  medialibdlg.ui  addbkgdlg.ui \
    colordlg.ui \
    msgbox.ui

INCLUDEPATH += ../../inc  cpp

android {
    LIBS += -L../../libs/armeabi-v7a

    platform = android
    DESTDIR = ../../../build/XMusic
} else: macx {
    LIBS += -L../../bin/mac

    platform = mac
    DESTDIR = ../../bin/mac

    libDir = $$PWD/../../../PlaySDK/bin/mac
    macDir = $$PWD/../../bin/mac
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

    LIBS += -L../../../build/ioslib

    platform = ios
    DESTDIR = ../../../build/XMusic
} else {
    SOURCES += ../Dump.cpp

    LIBS += -L../../bin -lDbghelp

    platform = win
    DESTDIR = ../../bin
}

LIBS += -lxutil  -lxPlaySDK  -lxMediaLib  -lxmodel

build_dir = TARGET

#CONFIG += debug_and_release
CONFIG(debug, debug|release) {
    build_dir = $$join($$build_dir,,,d)
}

build_dir = ../../../build/$$build_dir/$$platform

font.files += ../../bin/font/msyhl-6.23.ttc
#font.files += ../../bin/font/Microsoft-YaHei-Semilight-11.0.ttc
font.files += ../../bin/font/Microsoft-YaHei-SemiBold-11.0.ttc

hbkg.files += ../../release/hbkg/*
vbkg.files += ../../release/vbkg/*

android {
    #CONFIG += mobility
    #MOBILITY =

    font.path = /assets/font
    hbkg.path = /assets/hbkg
    vbkg.path = /assets/vbkg
    INSTALLS += font hbkg vbkg

    DISTFILES += \
        android/AndroidManifest.xml \
        android/res/values/libs.xml \
        android/build.gradle \
        android/gradlew.bat \
        android/gradlew \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradle/wrapper/gradle-wrapper.properties

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        ANDROID_EXTRA_LIBS = \
            $$PWD/../../libs/armeabi-v7a/libxutil.so \
            $$PWD/../../libs/armeabi-v7a/libxMediaLib.so \
            $$PWD/../../libs/armeabi-v7a/libxModel.so \
            $$PWD/../../libs/armeabi-v7a/libssl.so \
            $$PWD/../../libs/armeabi-v7a/libcrypto.so \
            $$PWD/../../libs/armeabi-v7a/libxPlaySDK.so \
            $$PWD/../../../PlaySDK/libs/armeabi-v7a/ffmpeg/libavcodec.so \
            $$PWD/../../../PlaySDK/libs/armeabi-v7a/ffmpeg/libavformat.so \
            $$PWD/../../../PlaySDK/libs/armeabi-v7a/ffmpeg/libavutil.so \
            $$PWD/../../../PlaySDK/libs/armeabi-v7a/ffmpeg/libswresample.so
    }
}

macx {
    ICON = xmusic.iconset/xmusic.icns

    QMAKE_INFO_PLIST += mac.plist

    font.path = Contents/MacOS/font
    hbkg.path = Contents/MacOS/hbkg
    vbkg.path = Contents/MacOS/vbkg
    QMAKE_BUNDLE_DATA  += font hbkg vbkg
}

ios {
    QMAKE_IOS_DEPLOYMENT_TARGET = 9.0

    #QMAKE_INFO_PLIST += iossimulator.plist #ios.plist

    font.path = /font
    hbkg.path = /hbkg
    vbkg.path = /vbkg
    QMAKE_BUNDLE_DATA  += font hbkg vbkg
} else {
MOC_DIR = $$build_dir
RCC_DIR = $$build_dir
UI_DIR = $$build_dir
}
OBJECTS_DIR = $$build_dir

DISTFILES += \
    android/src/xmusic/XActivity.java \
    android/src/xmusic/XActivity.java
