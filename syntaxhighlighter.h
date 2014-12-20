#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QTextCharFormat>
#include <QRegExp>
#include <QVector>
#include <QMessageBox>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "mainwindow.h"


class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    struct Rule {
        QRegExp regEx;
        QTextCharFormat format;
    };

public:
    explicit SyntaxHighlighter(QTextDocument* textDocument = 0, MainWindow* mainWindow = 0);
    ~SyntaxHighlighter();
    void highlightBlock(const QString& text);

signals:

private slots:
    void changeSyntax(QString fileName);

private:
    Rule rule;
    QVector<Rule> rulesList;
    QTextDocument * m_document;
    MainWindow * m_MainWindow;  // SyntaxHighlighter needs a reference to the MainWindow so that it can hook in to the MainWindows signals
    QTextCharFormat getFormat(const QXmlStreamReader& xsr);
    int str2int(QString str);
    QVector<Rule>& getRules(const QJsonDocument& jsonDoc);
};

#endif // SYNTAXHIGHLIGHTER_H
