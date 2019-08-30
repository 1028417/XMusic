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

#CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++1y #gnu++1y

android: CONFIG += mobility
android: MOBILITY =

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
    ProgressBar.h \
    bkgdlg.h \
    listview.h \
    dialog.h \
    medialibdlg.h \
    addbkgdlg.h \
    ListViewEx.h \
    app.h

RESOURCES += res.qrc

FORMS    += mainwindow.ui \
    bkgdlg.ui \
    medialibdlg.ui \
    addbkgdlg.ui

INCLUDEPATH += \
    ../../inc \
    ../../inc/util #../../inc/curl

android {
LIBS    += -L$$PWD/../../../XMusic/lib/armeabi-v7a -lxutil -lxMediaLib -lxmodel -lxPlaySDK #-lssl -lcrypto -lcurl

DESTDIR = $$PWD/../../build/XMusic

    font.files += ../../bin/Microsoft-YaHei-Light.ttc
#    font.files += ../../bin/Microsoft-YaHei-Regular.ttc
    font.files += ../../bin/Microsoft-YaHei-Semibold.ttc
    font.path = /assets
    INSTALLS += font

    hbkg.files += ../../bin/hbkg/win10
    hbkg.path = /assets/hbkg
    INSTALLS += hbkg

    vbkg.files += ../../bin/vbkg/win10
    vbkg.path = /assets/vbkg
    INSTALLS += vbkg

} else {
LIBS    += -L$$PWD/../../bin -lxutil

LIBS    += -L$$PWD/../../bin -lxMediaLib -lxmodel -lxPlaySDK

DESTDIR = $$PWD/../../bin
}

MOC_DIR = $$PWD/../../build/XMusic
RCC_DIR = $$PWD/../../build/XMusic
UI_DIR = $$PWD/../../build/XMusic
OBJECTS_DIR = $$PWD/../../build/XMusic

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
        $$PWD/../../lib/armeabi-v7a/ffmpeg/libavcodec.so \
        $$PWD/../../lib/armeabi-v7a/ffmpeg/libavformat.so \
        $$PWD/../../lib/armeabi-v7a/ffmpeg/libavutil.so \
        $$PWD/../../lib/armeabi-v7a/ffmpeg/libswresample.so
}
