#include "mainwindow.h"
#include "syntaxhighlighter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    SyntaxHighlighter * highlighter = new SyntaxHighlighter(w.getTextDocument(), &w);
    w.show();

    return a.exec();
}

