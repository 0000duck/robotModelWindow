######################################################################
# Automatically generated by qmake (3.1) Tue Jun 4 16:20:08 2019
######################################################################

TEMPLATE = app
TARGET = robotModelWindow
INCLUDEPATH += .
win32{
    LIBS += libws2_32 libwsock32
}

QMAKE_CXXFLAGS += -std=c++11
QT += widgets

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += robotModel/robotModelWidget.h \
    robotModel/robotModel.h

SOURCES += main.cpp \
           robotModel/robotModel.cpp \
           robotModel/robotModelWidget.cpp 

INCLUDEPATH += ../robotServer \
            ../robotServer/RobotStructure \
            ../robotServer/RobotStructure/RobSoft \
            ../robotServer/RobotClient \
            ../robotServer/RobotInterpreter \
            ../robotServer/RobotInterpreter/Common \
            ../robotServer/systemLayer \
            ../robotServer/SystemLayer/LinuxAPI \
            ../robotServer/LinkLayer \
            ../robotServer/LinkLayer/Simulation

INCLUDEPATH += /usr/include/libxml2/
LIBS += -L. -Wl,--rpath=./ -lrobotServerExe
LIBS += -lxml2

#win32{
#HEADERS += libClient/Windows/macroDefine.h \
#           libClient/Windows/systemLib.h \
#           libClient/Windows/tcpClient.h
#SOURCES += libClient/Windows/systemLib.cpp \
#           libClient/Windows/tcpClient.cpp
#}

#unix{
#HEADERS += libClient/Linux/macroDefine.h \
#           libClient/Linux/systemLib.h \
#           libClient/Linux/tcpClient.h
#SOURCES += libClient/Linux/systemLib.cpp \
#           libClient/Linux/tcpClient.cpp
#}
