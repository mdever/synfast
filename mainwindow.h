#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextDocument>
#include <QFileDialog>
#include <QTextStream>
#include <QException>
#include <QMessageBox>


/***************************************
 * Qt generates this Ui::MainWindow    *
 * class from the mainwindow.ui file   *
 * It is used in our constructor to    *
 * do all the work related to setting  *
 * up our UI using the                 *
 * Ui::MainWindow::setupUi() fuction   *
 **************************************/
namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QTextDocument* getTextDocument();
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_As_triggered();
    void on_actionSave_triggered();
    void on_actionImport_Template_triggered();

private:
    /****************************************************
     * The Ui::MainWindow is an object constructed by   *
     * the Qt precompiler. It gives us access to our    *
     * UI widgets and conveniently seperates UI and     *
     * application code.                                *
     ***************************************************/
    Ui::MainWindow *ui;
    QString lastFile;

signals:
    void syntaxChanged(QString fileName);
};

#endif // MAINWINDOW_H
