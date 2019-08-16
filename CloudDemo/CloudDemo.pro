#-------------------------------------------------
#
# Project created by QtCreator 2019-07-29T15:32:45
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CloudDemo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        buttongroup.cpp \
        common/common.cpp \
        common/des.c \
        common/logininfoinstance.cpp \
        common/uploadlayout.cpp \
        common/uploadtask.cpp \
        dataprogress.cpp \
        login.cpp \
        main.cpp \
        mainwindow.cpp \
        myfilewg.cpp \
        mymenu.cpp \
        rankinglist.cpp \
        sharelist.cpp \
        titlewg.cpp \
        transfer.cpp

HEADERS += \
        buttongroup.h \
        common/common.h \
        common/des.h \
        common/logininfoinstance.h \
        common/uploadlayout.h \
        common/uploadtask.h \
        dataprogress.h \
        login.h \
        mainwindow.h \
        myfilewg.h \
        mymenu.h \
        rankinglist.h \
        sharelist.h \
        titlewg.h \
        transfer.h

FORMS += \
        buttongroup.ui \
        dataprogress.ui \
        login.ui \
        mainwindow.ui \
        myfilewg.ui \
        rankinglist.ui \
        sharelist.ui \
        titlewg.ui \
        transfer.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qrc.qrc
