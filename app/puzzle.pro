QT += widgets multimedia

CONFIG += c++11

HEADERS     = mainwindow.h \
              puzzlepiece.h \
              piecesmodel.h \
              puzzlewidget.h \
              accessgrantedwidget.h \
              accessdeniedwidget.h \
              settingscontainer.h \
              settingsmanager.h

RESOURCES   = puzzle.qrc

SOURCES     = main.cpp \
              mainwindow.cpp \
              piecesmodel.cpp \
              puzzlewidget.cpp \
              accessgrantedwidget.cpp \
              accessdeniedwidget.cpp \
              settingsmanager.cpp

wince* {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}
