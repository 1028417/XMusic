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

#CONFIG -= app_bundle #macOS可执行文件不打包

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
    LIBS    += -L../../../XMusic/libs/armeabi-v7a -lxutil -lxPlaySDK -lxMediaLib -lxmodel

    DESTDIR = ../../../build/XMusic
} else: macx {
    LIBS    += -L../../bin/mac -lxutil.1 -lxPlaySDK.1 -lxMediaLib.1 -lxmodel.1

    DESTDIR = ../../bin/mac
} else {
    ios {
        LIBS    += -L../../../build/ioslib -lCURLTool

        LIBS    += -L../../libs/iossimulator -lcurl -lcrypto -lssl -lnghttp2 -lz

        LIBS    += -lSDL2 -framework AVFoundation -framework GameController -framework CoreMotion

        LIBS    += -lavcodec -lavformat -lavutil -lswresample \
                    -lz -lbz2 -liconv -framework CoreMedia -framework VideoToolbox -framework AVFoundation -framework CoreVideo -framework Security

        DESTDIR = ../../../build/XMusic
    } else {
        LIBS    += -L../../bin

        DESTDIR = ../../bin
    }

    LIBS    += -lxutil -lxPlaySDK -lxMediaLib -lxmodel
}

android {
    font.files += ../../bin/Microsoft-YaHei-Light.ttc
    #font.files += ../../bin/Microsoft-YaHei-Regular.ttc
    font.files += ../../bin/Microsoft-YaHei-Semibold.ttc
    font.path = /assets

    hbkg.files += ../../bin/hbkg/win10
    hbkg.path = /assets/hbkg

    vbkg.files += ../../bin/vbkg/win10
    vbkg.path = /assets/vbkg

    INSTALLS += font hbkg  vbkg

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
            $$PWD/../../libs/armeabi-v7a/libxutil.so \
            $$PWD/../../libs/armeabi-v7a/libxMediaLib.so \
            $$PWD/../../libs/armeabi-v7a/libxPlaySDK.so \
            $$PWD/../../libs/armeabi-v7a/libxModel.so \
            $$PWD/../../libs/armeabi-v7a/libCURLTool.so \
            $$PWD/../../libs/armeabi-v7a/libcurl.so \
            $$PWD/../../libs/armeabi-v7a/libssl.so \
            $$PWD/../../libs/armeabi-v7a/libcrypto.so \
            $$PWD/../../libs/armeabi-v7a/ffmpeg/libavcodec.so \
            $$PWD/../../libs/armeabi-v7a/ffmpeg/libavformat.so \
            $$PWD/../../libs/armeabi-v7a/ffmpeg/libavutil.so \
            $$PWD/../../libs/armeabi-v7a/ffmpeg/libswresample.so
    }
}

ios {
    QMAKE_IOS_DEPLOYMENT_TARGET = 9.0

    #ios_icon.files += $$files(ios/icons/*.png)
    #launch_image.files += xxx
    #QMAKE_BUNDLE_DATA  += ios_icon launch_image
} else {
MOC_DIR = ../../../build/XMusic
RCC_DIR = ../../../build/XMusic
UI_DIR = ../../../build/XMusic
}
OBJECTS_DIR = ../../../build/XMusic
