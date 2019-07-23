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
    ../PlayerApp/controller.cpp \
    main.cpp \
    view.cpp \
    PlayingList.cpp \
    widget.cpp

HEADERS  += mainwindow.h \
    ../PlayerApp/controller.h \
    view.h \
    label.h \
    PlayingList.h \
    widget.h \
    button.h

RESOURCES += res.qrc

FORMS    += mainwindow.ui

INCLUDEPATH += \
    ../../inc \
    ../../inc/util

android {
LIBS    += -L$$PWD/../../../XMusic/lib/armeabi-v7a -lxutil -lxMediaLib -lxmodel -lxPlaySDK

DESTDIR = $$PWD/../../build

    rc.files += ../../bin/data.db
    rc.files += ../../2player/msyhl.ttc
    rc.path = /assets
    INSTALLS += rc
} else {
LIBS    += -L$$PWD/../../bin -lxutil

LIBS    += -L$$PWD/../../bin -lxMediaLib -lxmodel -lxPlaySDK

DESTDIR = $$PWD/../../bin
}

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
