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
    cpp/xmusic.cpp \
    cpp/app.cpp \
    cpp/mainwindow.cpp \
    cpp/centralwidget.cpp \
    cpp/PlayingList.cpp \
    cpp/painter.cpp \
    cpp/widget/widget.cpp \
    cpp/widget/button.cpp \
    cpp/widget/listview.cpp \
    cpp/widget/label.cpp \
    cpp/dlg/dialog.cpp \
    cpp/dlg/msgbox.cpp \
    cpp/dlg/networkWarnDlg.cpp \
    cpp/dlg/bkg/bkgdlg.cpp \
    cpp/dlg/bkg/bkgview.cpp \
    cpp/dlg/bkg/addbkgdlg.cpp \
    cpp/dlg/bkg/imgdir.cpp \
    cpp/dlg/bkg/colordlg.cpp \
    cpp/dlg/mdl/medialibdlg.cpp \
    cpp/dlg/mdl/MedialibView.cpp \
    cpp/dlg/mdl/MLListView.cpp \
    cpp/dlg/mdl/singerimgdlg.cpp \
    cpp/dlg/mdl/wholeTrackDlg.cpp

HEADERS += ../../inc/ControllerDef.h \
    ../../inc/viewdef.h \
    ../OptionMgr.h \
    cpp/app.h \
    cpp/xmusic.h \
    cpp/mainwindow.h \
    cpp/centralwidget.h \
    cpp/PlayingList.h \
    cpp/painter.h \
    cpp/widget/widget.h \
    cpp/widget/button.h \
    cpp/widget/progressbar.h \
    cpp/widget/listview.h \
    cpp/widget/groupframe.h \
    cpp/widget/colorbar.h \
    cpp/widget/label.h \
    cpp/dlg/dialog.h \
    cpp/dlg/msgbox.h \
    cpp/dlg/bkg/bkgdlg.h \
    cpp/dlg/bkg/bkgview.h \
    cpp/dlg/bkg/addbkgdlg.h \
    cpp/dlg/bkg/imgdir.h \
    cpp/dlg/bkg/colordlg.h \
    cpp/dlg/networkWarnDlg.h \
    cpp/dlg/mdl/medialibdlg.h \
    cpp/dlg/mdl/MedialibView.h \
    cpp/dlg/mdl/MLListView.h \
    cpp/dlg/mdl/wholeTrackDlg.h \
    cpp/dlg/mdl/singerimgdlg.h

RESOURCES += res.qrc

FORMS += ui/mainwindow.ui  ui/msgbox.ui  ui/networkWarnDlg.ui \
         ui/bkgdlg.ui  ui/addbkgdlg.ui  ui/colordlg.ui \
         ui/medialibdlg.ui  ui/wholeTrackDlg.ui  ui/singerimgdlg.ui

INCLUDEPATH += ../../inc  cpp  cpp/widget

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
