QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    src/core/systemUtils.cpp \
    src/model/base/basemonitor.cpp \
    src/model/cpumonitor.cpp \
    src/model/memorymonitor.cpp \
    src/view/widgets/circularprogress.cpp \
    src/view/widgets/metriccard.cpp

HEADERS += \
    src/core/constants.h \
    src/core/systemUtils.h \
    src/model/base/basemonitor.h \
    src/model/cpumonitor.h \
    src/model/memorymonitor.h \
    src/view/widgets/circularprogress.h \
    src/view/widgets/metriccard.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
