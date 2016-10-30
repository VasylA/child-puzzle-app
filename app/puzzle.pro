QT += widgets multimedia

CONFIG += c++11

HEADERS     = mainwindow.h \
              gpio_class.h \
              puzzlepiece.h \
              piecesmodel.h \
              puzzlewidget.h \
              testpointscontroller.h \
              startpuzzlewidget.h \
              accessgrantedwidget.h \
              accessdeniedwidget.h \
              settingscontainer.h \
              settingsmanager.h

RESOURCES   = puzzle.qrc

SOURCES     = main.cpp \
              mainwindow.cpp \
              gpio_class.cpp \
              piecesmodel.cpp \
              puzzlewidget.cpp \
              testpointscontroller.cpp \
              startpuzzlewidget.cpp \
              accessgrantedwidget.cpp \
              accessdeniedwidget.cpp \
              settingsmanager.cpp

wince* {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}
