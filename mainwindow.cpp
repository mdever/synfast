#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(600,480);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QTextDocument * MainWindow::getTextDocument() {
    QTextDocument* document_ptr = ui->textEdit->document();
    return document_ptr;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString("&Open File"), QString(), QString());
    QFile file(fileName);
    try {
        file.open(QIODevice::ReadOnly);
        QTextStream ts(&file);
        QString text = ts.readAll();
        ui->textEdit->setText(text);
        ts.flush();
        file.close();
    } catch(QException e) {
        QMessageBox::critical( this, QString("Error"), QString("There was an error opening file: %1").arg(fileName) );
    }

    this->lastFile = fileName;  // We save the fileName in a member field so that we can retrieve it later for the File->Save option
}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString("$Save File"), QString(), QString());
    QFile file(fileName);
    try {
        file.open(QIODevice::WriteOnly);
        QTextStream ts(&file);
        QString text = ui->textEdit->document()->toPlainText();
        ts << text;
        ts.flush();
        file.close();
    } catch(QException e) {
        QMessageBox::critical(this, QString("Error"), QString("There was an erro saving file: %1").arg(fileName));
    }

    this->lastFile = fileName;  // We save the fileName in a member field so that we can retrieve it later for the File->Save option
}

void MainWindow::on_actionSave_triggered()
{
    if(lastFile == NULL) {
        on_actionOpen_triggered();
    } else {
        QFile file(lastFile);
        try {
            file.open(QIODevice::WriteOnly);
            QTextStream ts(&file);
            QString text = ui->textEdit->document()->toPlainText();
            ts << text;
            ts.flush();
            file.close();
        } catch(QException e) {
            QMessageBox::critical(this, QString("File Save Error"), QString("There was an error saving file: %1").arg(lastFile));
        }
    }
}

void MainWindow::on_actionImport_Template_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString("Open Template"), QString(), QString("*.tax"));
    emit syntaxChanged(fileName);
}
