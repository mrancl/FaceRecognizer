#-------------------------------------------------
#
# Project created by QtCreator 2014-09-15T14:43:26
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FaceRecognition
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    utilities.cpp \
    actionbar.cpp \
    menustyle.cpp \
    camera.cpp \
    facerecognition.cpp \
    selectionwidget.cpp \
    searchlineedit.cpp

HEADERS  += mainwindow.h \
    utilities.h \
    actionbar.h \
    menustyle.h \
    camera.h \
    facerecognition.h \
    selectionwidget.h \
    searchlineedit.h \
    customcompleter.h

FORMS    += mainwindow.ui

#INCLUDEPATH += E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/jni/include

#LIBS += \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_highgui.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_androidcamera.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_features2d.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_objdetect.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_contrib.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_features2d.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_imgproc.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_core.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/3rdparty/libs/armeabi-v7a/liblibtiff.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/3rdparty/libs/armeabi-v7a/libtbb.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/3rdparty/libs/armeabi-v7a/libIlmImf.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/3rdparty/libs/armeabi-v7a/liblibpng.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/3rdparty/libs/armeabi-v7a/liblibjpeg.a \
#        E:/kituri/OpenCV-2.4.9-android-sdk/sdk/native/3rdparty/libs/armeabi-v7a/liblibjasper.a

INCLUDEPATH += E:/opencv/build/include \
    E:/opencv/build/include/opencv \
    E:/opencv/build/include/opencv2 \

LIBS += -LE:/opencv/build/x86/vc11/lib/ \
    -lopencv_core246 \
    -lopencv_highgui246 \
    -lopencv_imgproc246 \
    -lopencv_features2d246 \
    -lopencv_contrib246 \
    -lopencv_objdetect246

#CONFIG += mobility
#MOBILITY =

#deployment.files += faces_training_set.sqlite haarcascade_frontalface_alt.xml
#deployment.path = /assets
#INSTALLS += deployment

#ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

#OTHER_FILES += \
#    android/AndroidManifest.xml

#contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
#    ANDROID_EXTRA_LIBS = \
#        C:/Qt/Qt5.3.1/Tools/QtCreator/bin/FaceRecognition/libnative_camera_r4.2.0.so \
#        $$PWD/libopencv_java.so
#                         $$PWD/libopencv_java.so
#}

RESOURCES += \
    app.qrc

