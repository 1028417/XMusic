#-------------------------------------------------
#
# Project created by QtCreator 2019-03-27T01:29:13
#
#-------------------------------------------------

QT       += core gui

RC_ICONS = "icon.ico"

android: QT += androidextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XMusic
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++1y #gnu++1y

#CONFIG -= app_bundle #macx可执行文件不打包

android {
    CONFIG += mobility
    MOBILITY =
}

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
LIBS    += -L$$PWD/../../../XMusic/lib/armeabi-v7a -lxutil -lxPlaySDK -lxMediaLib -lxmodel

DESTDIR = $$PWD/../../build/XMusic

} else {

macx {
    LIBS    += -L$$PWD/../../bin/macx -lxutil.1 -lxPlaySDK.1 -lxMediaLib -lxmodel

    DESTDIR = $$PWD/../../bin/macx
} else {
    ios {
        LIBS    += -L$$PWD/../../../build/ioslib
    } else {
        LIBS    += -L$$PWD/../../bin
    }

    LIBS    += -lxutil -lxPlaySDK -lxMediaLib -lxmodel

    DESTDIR = $$PWD/../../bin
}
}

android {
    font.files += ../../bin/Microsoft-YaHei-Light.ttc
    #font.files += ../../bin/Microsoft-YaHei-Regular.ttc
    font.files += ../../bin/Microsoft-YaHei-Semibold.ttc
    font.path = /assets
    INSTALLS += font

    hbkg.files += ../../bin/hbkg/win10
    hbkg.path = /assets/hbkg
    INSTALLS += hbkg

    vbkg.files += ../../bin/vbkg/win10
    vbkg.path = /assets/vbkg
    INSTALLS += vbkg

    DISTFILES += \
        android/AndroidManifest.xml \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradlew \
        android/res/values/libs.xml \
        android/build.gradle \
        android/gradle/wrapper/gradle-wrapper.properties \
        android/gradlew.bat

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        ANDROID_EXTRA_LIBS = \
            $$PWD/../../lib/armeabi-v7a/libxutil.so \
            $$PWD/../../lib/armeabi-v7a/libxMediaLib.so \
            $$PWD/../../lib/armeabi-v7a/libxModel.so \
            $$PWD/../../lib/armeabi-v7a/libxPlaySDK.so \
            $$PWD/../../lib/armeabi-v7a/libcrypto.so \
            $$PWD/../../lib/armeabi-v7a/libssl.so \
            $$PWD/../../lib/armeabi-v7a/libcurl.so \
            $$PWD/../../lib/armeabi-v7a/ffmpeg/libavcodec.so \
            $$PWD/../../lib/armeabi-v7a/ffmpeg/libavformat.so \
            $$PWD/../../lib/armeabi-v7a/ffmpeg/libavutil.so \
            $$PWD/../../lib/armeabi-v7a/ffmpeg/libswresample.so
    }
}

ios {
    QMAKE_IOS_DEPLOYMENT_TARGET = 9.0

    #ios_icon.files += $$files($$PWD/ios/icons/*.png)
    #launch_image.files += xxx
    #QMAKE_BUNDLE_DATA  += ios_icon launch_image

    LIBS    +=  -L$$PWD/../../../PlaySDK/ioslib

    LIBS    +=  -lavcodec -lavformat -lavutil -lswresample \
                -lz -lbz2 -liconv -framework CoreMedia -framework VideoToolbox -framework AVFoundation -framework CoreVideo -framework Security

    LIBS    +=  -lSDL2 -framework AVFoundation -framework GameController -framework CoreMotion

} else {
MOC_DIR = $$PWD/../../../build/XMusic
RCC_DIR = $$PWD/../../../build/XMusic
UI_DIR = $$PWD/../../../build/XMusic
}
OBJECTS_DIR = $$PWD/../../../build/XMusic
