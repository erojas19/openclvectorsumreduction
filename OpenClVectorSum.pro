TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp
win32: LIBS += -L"C:/Intel/OpenCL/sdk/lib/x64" -lOpenCL

INCLUDEPATH += "C:/Intel/OpenCL/sdk/lib/x64"
DEPENDPATH += "C:/Intel/OpenCL/sdk/lib/x64"

DISTFILES += \
    kernel.cl \
    Readme.md
