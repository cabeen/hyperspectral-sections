CONFIG += qt debug
QT += opengl

LIBS += -lgdal1.6.0 -lqwt-qt4
INCLUDEPATH += src/ui src/geo /usr/include/gdal /usr/include/qwt-qt4

DEPENDPATH += src/ui src/geo
DEFINES += TIXML_USE_STL

OTHER_FILES +=
TARGET = hypsect
TEMPLATE = app
SOURCES += \
    src/main.cpp \
    src/geo/floatimage.cpp \
    src/geo/hypspecimage.cpp \
    src/ui/canvas2d.cpp \
    src/ui/curvecanvas2d.cpp \
    src/ui/mainwindow.cpp \ 
    src/ui/settings.cpp \
    src/ui/colormapwindow.cpp \
    src/ui/colormap.cpp \
    src/ui/sectionwindow.cpp \
    src/ui/spectrumwindow.cpp

HEADERS += \
    src/geo/hypspecimage.h \
    src/geo/floatimage.h \
    src/ui/canvas2d.h \
    src/ui/curvecanvas2d.h \
    src/ui/mainwindow.h \
    src/ui/settings.h \
    src/ui/colormapwindow.h \
    src/ui/colormap.h \
    src/ui/sectionwindow.h \
    src/ui/spectrumwindow.h

# Dont add the -pg flag unless you know what you are doing. It makes QThreadPool freeze on Mac OS X
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat\
  -Wchar-subscripts -Wformat -Wmain -Wmissing-braces -Wparentheses -Wreorder\
  -Wreturn-type -Wsequence-point -Wsign-compare -Wstrict-aliasing\
  -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunused-label\
  -Wunused-variable -Wvolatile-register-var -Wno-extra

# QMAKE_CXX_FLAGS_WARN_ON += -Wunknown-pragmas -Wunused-function -Wmain

macx {
    QMAKE_CXXFLAGS_WARN_ON -= -Warray-bounds -Wc++0x-compat
}
