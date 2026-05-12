#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "lesson.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void ProcessMultipleLines(const QString& data);

private slots:
    void OnParseButtonClicked();
    void OnClearButtonClicked();
    void OnHelpButtonClicked();

private:
    void SetupUi();
    void SetupConnections();
    void UpdateDisplay(const Lesson& lesson);
    void ShowError(const QString& error_message);
    void ClearDisplay();

    QTextEdit* input_text_edit_;
    QTextEdit* result_line_edit_;
    QPushButton* parse_button_;
    QPushButton* clear_button_;
};

#endif
