QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    EventActionExecutor/eventactionexecutor.cpp \
    EventActionExecutor/eventactionexecutor_copyfile.cpp \
    EventActionExecutor/eventactionexecutor_deletefile.cpp \
    EventActionExecutor/eventactionexecutor_getvarfromfilename.cpp \
    EventActionExecutor/eventactionexecutor_makecontainer.cpp \
    EventActionExecutor/eventactionexecutor_parsefile.cpp \
    EventActionExecutor/eventactionexecutor_parsemanual.cpp \
    EventActionExecutor/eventactionexecutor_replacefile.cpp \
    EventActionExecutor/eventactionexecutor_startscen.cpp \
    EventActionExecutor/eventactionexecutor_unpackcontainer.cpp \
    EventActionExecutor/eventactionexecutor_varsarifmethicoperations.cpp \
    ScenExecutor/ActionExecutors/actionexecutor.cpp \
    ScenExecutor/ActionExecutors/branchingactionexecutor.cpp \
    ScenExecutor/ActionExecutors/simpleactionexecutor.cpp \
    ScenExecutor/ActionParsers/actionparser.cpp \
    ScenExecutor/ActionParsers/branchingactionparser.cpp \
    ScenExecutor/ActionParsers/copyactionparser.cpp \
    ScenExecutor/ActionParsers/deleteactionparser.cpp \
    ScenExecutor/ActionParsers/getactionparser.cpp \
    ScenExecutor/ActionParsers/sendactionparser.cpp \
    ScenExecutor/ActionParsers/simpleactionparser.cpp \
    ScenExecutor/ActionParsers/startactionparser.cpp \
    ScenExecutor/action.cpp \
    ScenExecutor/scenexecutor.cpp \
    TCPServer.cpp \
    clientwidget.cpp \
    directorymanager.cpp \
    directorymonitor.cpp \
    eventexecutor.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    settingsparsers.cpp \
    timer.cpp \
    waitlist.cpp

HEADERS += \
    EventActionExecutor/eventactionexecutor_replacefile.h \
    EventActionExecutor/eventactionexecutor_unpackcontainer.h \
    GlobalData/branchingcommand.h \
    GlobalData/datapack.h \
    GlobalData/globalenums.h \
    GlobalData/globalfunctions.h \
    EventActionExecutor/eventactionexecutor.h \
    EventActionExecutor/eventactionexecutor_copyfile.h \
    EventActionExecutor/eventactionexecutor_deletefile.h \
    EventActionExecutor/eventactionexecutor_getvarfromfilename.h \
    EventActionExecutor/eventactionexecutor_makecontainer.h \
    EventActionExecutor/eventactionexecutor_parsefile.h \
    EventActionExecutor/eventactionexecutor_parsemanual.h \
    EventActionExecutor/eventactionexecutor_startscen.h \
    EventActionExecutor/eventactionexecutor_varsarifmethicoperations.h \
    ScenExecutor/ActionExecutors/actionexecutor.h \
    ScenExecutor/ActionExecutors/branchingactionexecutor.h \
    ScenExecutor/ActionExecutors/simpleactionexecutor.h \
    ScenExecutor/ActionParsers/actionparser.h \
    ScenExecutor/ActionParsers/branchingactionparser.h \
    ScenExecutor/ActionParsers/copyactionparser.h \
    ScenExecutor/ActionParsers/deleteactionparser.h \
    ScenExecutor/ActionParsers/getactionparser.h \
    ScenExecutor/ActionParsers/sendactionparser.h \
    ScenExecutor/ActionParsers/simpleactionparser.h \
    ScenExecutor/ActionParsers/startactionparser.h \
    ScenExecutor/action.h \
    ScenExecutor/scenexecutor.h \
    TCPServer.h \
    clientwidget.h \
    directorymanager.h \
    directorymonitor.h \
    eventexecutor.h \
    mainwindow.h \
    settings.h \
    settingsparsers.h \
    timer.h \
    waitlist.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
