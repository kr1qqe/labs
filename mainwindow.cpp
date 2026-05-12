#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <iostream>

using namespace std;

// Инициализация статических переменных
int MainWindow::solveCallCount = 0;
int MainWindow::lLessThan1Count = 0;
int MainWindow::lGreaterEqual1Count = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ТОЛЬКО ОДНО ОПРЕДЕЛЕНИЕ функции solve
int MainWindow::solve(int h, int r)
{
    solveCallCount++;

    int L = static_cast<int>(floor(h * sqrt(3.0) / 2.0));

    if (L < 1) {
        L = 1;
        lLessThan1Count++;
    } else {
        lGreaterEqual1Count++;
    }

    int segments = (r + L - 1) / L;
    return segments - 1;
}

void MainWindow::on_pushButton_clicked()
{
    int h = ui->lineEdit_h->text().toInt();
    int r = ui->lineEdit_r->text().toInt();
    int result = solve(h, r);
    ui->label_result->setText(QString::number(result));
}
