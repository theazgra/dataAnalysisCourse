#-------------------------------------------------
#
# Project created by QtCreator 2019-05-04T12:31:48
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetworkWizard
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
        generate_dialog.cpp \
        girvan_newman_dialog.cpp \
        import_dialog.cpp \
        main.cpp \
        network_wizard.cpp \
        vertex_info_form.cpp \
        vertex_info_model.cpp

HEADERS += \
        generate_dialog.h \
        girvan_newman_dialog.h \
        import_dialog.h \
        network_wizard.h \
        thread_jobs.h \
        vertex_info_form.h \
        vertex_info_model.h \
        wizard_state.h

FORMS += \
        generate_dialog.ui \
        girvan_newman_dialog.ui \
        import_dialog.ui \
        network_wizard.ui \
        vertex_info_form.ui

QMAKE_CXXFLAGS  += -fopenmp
QMAKE_LFLAGS    += -fopenmp

LIBS            += -L $$PWD/../../networkLib/build -l NetworkWizardS
INCLUDEPATH     += $$PWD/../../networkLib/include


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
