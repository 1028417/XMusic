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

QMAKE_CXXFLAGS += -std=c++11 #c++1y #gnu++1y

SOURCES += ../controller.cpp \
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

    macDir = $$PWD/../../bin/mac
    frameworkDir = $$macDir/XMusic.app/Contents/Frameworks
    QMAKE_POST_LINK += mkdir $$frameworkDir \
        && cp -f $$macDir/libavcodec.58.dylib $$frameworkDir/ \
        && cp -f $$macDir/libavformat.58.dylib $$frameworkDir/ \
        && cp -f $$macDir/libavutil.56.dylib $$frameworkDir/ \
        && cp -f $$macDir/libswresample.3.dylib $$frameworkDir/ \
        && mkdir $$frameworkDir/SDL2.framework \
        && cp -R $$macDir/SDL2.framework/ $$frameworkDir/SDL2.framework \
        && ~/Qt5.13.2/5.13.2/clang_64/bin/macdeployqt $$macDir/XMusic.app -libpath=$$macDir -dmg
} else: ios {
    LIBS += -L../../libs/ios/simulator  -lssl  -lcrypto  -lnghttp2 \
#
            -L../../../PlaySDK/libs/ios/simulator  -lSDL2 \
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

    platform =win
    DESTDIR = ../../bin
}

LIBS += -lxutil  -lxPlaySDK  -lxMediaLib  -lxmodel

build_dir = ../../../build/XMusic/$$platform

font.files += ../../bin/font/Microsoft-YaHei-Light.ttc
#font.files += ../../bin/font/Microsoft-YaHei-Regular.ttc
font.files += ../../bin/font/Microsoft-YaHei-Semibold.ttc

hbkg.files += ../../bin/hbkg/win10
vbkg.files += ../../bin/vbkg/win10
upgradeConf.files += upgrade.conf

android {
    #CONFIG += mobility
    #MOBILITY =

    font.path = /assets/font
    hbkg.path = /assets/hbkg
    vbkg.path = /assets/vbkg
    upgradeConf.path = /assets
    INSTALLS += font hbkg vbkg upgradeConf

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
} else: macx {
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
