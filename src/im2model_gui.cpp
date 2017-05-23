
#include "configwin.h"

#include <iostream>

#include <QtWidgets>
#include <QTreeView>
#include <QtCore>
#include <QApplication>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QAction>

int main(int argc, char *argv[])
{
  // to access resources
  Q_INIT_RESOURCE(im2model);

  QApplication app(argc, argv);

      QApplication::setOrganizationName("di.uminho");
      QApplication::setOrganizationDomain("di.uminho.pt");

  // to ease the load process
  QCommandLineParser parser;
  parser.setApplicationDescription(QCoreApplication::applicationName());
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("file", "The file to open.");
  parser.process(app);
  MainWindow window;
  if (!parser.positionalArguments().isEmpty())
    window.loadFile(parser.positionalArguments().first());
  window.show();
  return app.exec();
}
