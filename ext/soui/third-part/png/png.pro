######################################################################
# Automatically generated by qmake (2.01a) ?? ?? 30 16:24:02 2014
######################################################################

TEMPLATE = lib
TARGET = png
CONFIG(x64){
TARGET = $$TARGET"64"
}
DEPENDPATH += .
INCLUDEPATH += . \
	       ../zlib

CONFIG += staticlib

dir = ../..
include($$dir/common.pri)

DEFINES += _CRT_SECURE_NO_WARNINGS

# Input
HEADERS += png.h \
           pngconf.h \
           pngdebug.h \
           pnginfo.h \
           pnglibconf.h \
           pngpriv.h \
           pngstruct.h
SOURCES += png.c \
           pngerror.c \
           pngget.c \
           pngmem.c \
           pngpread.c \
           pngread.c \
           pngrio.c \
           pngrtran.c \
           pngrutil.c \
           pngset.c \
           pngtrans.c \
           pngwio.c \
           pngwrite.c \
           pngwtran.c \
           pngwutil.c
