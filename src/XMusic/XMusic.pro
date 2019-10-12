#-------------------------------------------------
#
# Project created by QtCreator 2019-03-27T01:29:13
#
#-------------------------------------------------

QT       += core gui
#android: QT += androidextras

RC_ICONS = "xmusic.ico"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XMusic
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++1y #gnu++1y

SOURCES +=\
    mainwindow.cpp \
    ../XMusicHost/controller.cpp \
    PlayingList.cpp \
    widget.cpp \
    bkgdlg.cpp \
    listview.cpp \
    medialibdlg.cpp \
    addbkgdlg.cpp \
    ListViewEx.cpp \
    app.cpp

HEADERS  += mainwindow.h \
    ../XMusicHost/controller.h \
    label.h \
    PlayingList.h \
    widget.h \
    button.h \
    progressbar.h \
    bkgdlg.h \
    listview.h \
    dialog.h \
    medialibdlg.h \
    addbkgdlg.h \
    ListViewEx.h \
    app.h \
    ../../inc/ControllerDef.h \
    ../../inc/viewdef.h

RESOURCES += res.qrc

FORMS    += mainwindow.ui \
    bkgdlg.ui \
    medialibdlg.ui \
    addbkgdlg.ui

INCLUDEPATH += ../../inc

android {
    LIBS    += -L../../libs/armeabi-v7a

    platform = android
    DESTDIR = ../../../build/XMusic
} else: macx {
    LIBS    += -L../../bin/mac

    platform = mac
    DESTDIR = ../../bin/mac
} else: ios {
    LIBS    += -L../../libs/ios/simulator -lcurl -lssl -lcrypto -lnghttp2 -lz
    LIBS    += -L../../libs/ios -lCURLTool

    LIBS    += -L../../../PlaySDK/libs/ios/simulator -lSDL2 \
                -framework AVFoundation -framework GameController -framework CoreMotion
    LIBS    += -lavcodec -lavformat -lavutil -lswresample \
                -lz -lbz2 -liconv -framework CoreMedia -framework VideoToolbox -framework AVFoundation -framework CoreVideo -framework Security

    LIBS    += -L../../../build/ioslib

    platform = ios
    DESTDIR = ../../../build/XMusic
} else {
    LIBS    += -L../../bin

    platform =win
    DESTDIR = ../../bin
}

LIBS    += -lxutil -lxPlaySDK -lxMediaLib -lxmodel

font.files += ../../bin/font/Microsoft-YaHei-Light.ttc
#font.files += ../../bin/font/Microsoft-YaHei-Regular.ttc
font.files += ../../bin/font/Microsoft-YaHei-Semibold.ttc

hbkg.files += ../../bin/hbkg/win10
vbkg.files += ../../bin/vbkg/win10

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
        android/build.gradle

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        ANDROID_EXTRA_LIBS = \
            $$PWD/../../libs/armeabi-v7a/libxutil.so \
            $$PWD/../../libs/armeabi-v7a/libxMediaLib.so \
            $$PWD/../../libs/armeabi-v7a/libxModel.so \
            $$PWD/../../libs/armeabi-v7a/libCURLTool.so \
            $$PWD/../../libs/armeabi-v7a/libcurl.so \
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

    #target.path = /Application
    #INSTALLS += target

    font.path = Contents/MacOS/font
    hbkg.path = Contents/MacOS/hbkg
    vbkg.path = Contents/MacOS/vbkg
    QMAKE_BUNDLE_DATA  += font hbkg vbkg
}

build_dir = ../../../build/XMusic/$$platform

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
