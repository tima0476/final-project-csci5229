#  Project file for the mountain meadow sim
#  Timothy Mason
#
#  List of header files
HEADERS = gl-widget.h main-window.h util.h
#  List of source files
SOURCES = main.cpp gl-widget.cpp main-window.cpp util.cpp
#  Include OpenGL support
QT += opengl

LIBS += -lglut -lGLU