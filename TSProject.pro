#-------------------------------------------------
#
# Project created by QtCreator 2012-06-15T02:30:09
#
#-------------------------------------------------

QT       += core gui\
            sql

TARGET = TSProject
TEMPLATE = app

SOURCES += main.cpp\
    tsmodel.cpp \
    tscurvebuffer.cpp \
    tsusb3000reader.cpp \
    tsreaderthread.cpp \
    inputs/tslineedit.cpp \
    inputs/tscombobox.cpp \
    tscontroller.cpp \
    tools/tsvalidationtools.cpp

HEADERS  += \
    tsmodel.h \
    tscurvebuffer.h \
    tsusb3000reader.h \
    tsreaderthread.h \
    inputs/tslineedit.h \
    inputs/tscombobox.h \
    ts_types.h \
    tscontroller.h \
    tools/tsvalidationtools.h
LIBS += -L"dll/*.dll"
FORMS    += \
    patientprofile.ui \
    colibrateDialog.ui \
    tsview.ui

RESOURCES += \
    iconResourse.qrc \
    iconResourse.qrc











