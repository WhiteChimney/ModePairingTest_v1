QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    auxiliaryfunction.cpp \
    source/customizedFunctions.cpp \
    source/tekawg5200.cpp \
    main.cpp \
    source/mainwindow.cpp \
    source/tdcqutag.cpp \
    source/tdcqutagconfig.cpp \
    source/tdcqutagdataprocess.cpp \
    tool/afg3000.cpp \
    tool/btnbool.cpp \
    tool/lneunit.cpp \
    tool/qcustomplot.cpp \
    tool/spd300qsky.cpp \
    tool/typeconversion.cpp

HEADERS += \
    auxiliaryfunction.h \
    include/TekVisa/TekVisa.h \
    include/TekVisa/visa.h \
    include/TekVisa/visatype.h \
    include/TekVisa/vpptype.h \
    include/QutagTdc/tdcbase.h \
    include/QutagTdc/tdcdecl.h \
    include/QutagTdc/tdchbt.h \
    include/QutagTdc/tdclifetm.h \
    include/QutagTdc/tdcmultidev.h \
    include/QutagTdc/tdcstartstop.h \
    source/mainwindow.h \
    source/tdcqutag.h \
    source/tdcqutagconfig.h \
    source/tdcqutagdataprocess.h \
    source/tekawg5200.h \
    tool/afg3000.h \
    tool/btnbool.h \
    tool/qcustomplot.h \
    tool/lneunit.h \
    tool/spd300qsky.h \
    tool/typeconversion.h

FORMS += \
    source/mainwindow.ui \
    source/tdcqutagconfig.ui \
    source/tekawg5200.ui

OTHER_FILES += \
    config.ini

INCLUDEPATH += $$PWD/include/
INCLUDEPATH += $$PWD/include/TekVisa
INCLUDEPATH += $$PWD/include/QutagTdc
INCLUDEPATH += $$PWD/lib/
INCLUDEPATH += $$PWD/source/
INCLUDEPATH += $$PWD/tool/

#unix|win32: LIBS += -L$$PWD/tdc_lib -ltdcbase
#INCLUDEPATH += $$PWD/tdc_lib/
#DEPENDPATH += $$PWD/tdc_lib/



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target




unix|win32: LIBS += -L$$PWD/lib/tdc_lib/ -ltdcbase

INCLUDEPATH += $$PWD/lib/tdc_lib
DEPENDPATH += $$PWD/lib/tdc_lib


unix|win32: LIBS += -L$$PWD/lib/tekVisa_lib/x64/ -ltkVisa64

unix|win32: LIBS += -L$$PWD/lib/tekVisa_lib/x64/ -lvisa64

INCLUDEPATH += $$PWD/lib/tekVisa_lib/x64
DEPENDPATH += $$PWD/lib/tekVisa_lib/x64


