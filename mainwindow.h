#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int solve(int h, int r);

    // Статические переменные для подсчёта покрытия
    static int solveCallCount;
    static int lLessThan1Count;
    static int lGreaterEqual1Count;

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif
