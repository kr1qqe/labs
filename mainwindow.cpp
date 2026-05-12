#include "mainwindow.h"
#include "parser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFont>
#include <QMessageBox>
#include <QTextBrowser>
#include <QDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    input_text_edit_(nullptr),
    result_line_edit_(nullptr),
    parse_button_(nullptr),
    clear_button_(nullptr)
{
    SetupUi();
    SetupConnections();
    setWindowTitle("Парсер учебных занятий");
    resize(600, 400);
}

MainWindow::~MainWindow() {}

void MainWindow::SetupUi() {
    QWidget* central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);

    // Группа ввода
    QGroupBox* input_group = new QGroupBox("Ввод описания занятия");
    QVBoxLayout* input_layout = new QVBoxLayout(input_group);

    input_text_edit_ = new QTextEdit();
    input_text_edit_->setPlaceholderText(
        "Введите описание занятия в формате:\n"
        "Дата Аудитория Преподаватель\n\n"
        "Примеры:\n"
        "2024.12.25 301 Иванов\n"
        "2024.12.26 405 \"Петрова А.И.\"\n"
        "2024.12.27 101-A Сидоров");
    input_text_edit_->setMaximumHeight(150);
    input_layout->addWidget(input_text_edit_);
    main_layout->addWidget(input_group);

    // Группа результата
    QGroupBox* result_group = new QGroupBox("Результат парсинга");
    QVBoxLayout* result_layout = new QVBoxLayout(result_group);

    result_line_edit_ = new QTextEdit();
    result_line_edit_->setReadOnly(true);
    result_line_edit_->setPlaceholderText("Результат будет отображен здесь");
    QFont font = result_line_edit_->font();
    font.setPointSize(10);
    result_line_edit_->setFont(font);
    result_layout->addWidget(result_line_edit_);
    main_layout->addWidget(result_group);

    // Группа кнопок
    QHBoxLayout* button_layout = new QHBoxLayout();

    parse_button_ = new QPushButton("Разобрать");
    parse_button_->setMinimumHeight(35);

    clear_button_ = new QPushButton("Очистить");
    clear_button_->setMinimumHeight(35);

    QPushButton* help_button = new QPushButton("Справка");
    help_button->setMinimumHeight(35);

    button_layout->addStretch();
    button_layout->addWidget(parse_button_);
    button_layout->addWidget(clear_button_);
    button_layout->addWidget(help_button);
    button_layout->addStretch();

    main_layout->addLayout(button_layout);

    // Подключаем сигналы
    connect(parse_button_, &QPushButton::clicked, this, &MainWindow::OnParseButtonClicked);
    connect(clear_button_, &QPushButton::clicked, this, &MainWindow::OnClearButtonClicked);
    connect(help_button, &QPushButton::clicked, this, &MainWindow::OnHelpButtonClicked);
}

void MainWindow::SetupConnections() {
    connect(parse_button_, &QPushButton::clicked,
            this, &MainWindow::OnParseButtonClicked);
    connect(clear_button_, &QPushButton::clicked,
            this, &MainWindow::OnClearButtonClicked);
}

void MainWindow::ProcessMultipleLines(const QString& data) {
    if (!input_text_edit_ || !result_line_edit_) return;

    QStringList lines = data.split("\n", Qt::SkipEmptyParts);

    if (lines.isEmpty()) {
        ShowError("Файл пуст или не содержит данных");
        return;
    }

    QStringList results;
    int success_count = 0;
    int error_count = 0;

    for (const QString& line : lines) {
        QString trimmed_line = line.trimmed();
        if (trimmed_line.isEmpty()) continue;

        Parser parser(trimmed_line);

        if (parser.Parse()) {
            Lesson lesson = parser.GetLesson();
            results << lesson.ToString();
            success_count++;
        } else {
            results << QString("ОШИБКА: %1\n   %2").arg(trimmed_line).arg(parser.GetErrorMessage());
            error_count++;
        }
    }

    result_line_edit_->setPlainText(results.join("\n\n"));

    // Выводим статистику в заголовок окна
    setWindowTitle(QString("Парсер учебных занятий - Успешно: %1, Ошибок: %2").arg(success_count).arg(error_count));
}

void MainWindow::OnParseButtonClicked() {
    QString input_text = input_text_edit_->toPlainText().trimmed();

    if (input_text.isEmpty()) {
        ShowError("Пожалуйста, введите описание занятия");
        return;
    }

    Parser parser(input_text);

    if (parser.Parse()) {
        Lesson lesson = parser.GetLesson();
        UpdateDisplay(lesson);
    } else {
        ShowError(parser.GetErrorMessage());
    }
}

void MainWindow::OnClearButtonClicked() {
    ClearDisplay();
}

void MainWindow::OnHelpButtonClicked() {
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Справка");
    dialog->resize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    QTextBrowser* browser = new QTextBrowser();
    browser->setHtml("<h2>Справка</h2>"
                     "<p><b>Формат ввода:</b> Дата Аудитория Преподаватель</p>"
                     "<p><b>Примеры:</b></p>"
                     "<ul>"
                     "<li>2024.12.25 301 Иванов</li>"
                     "<li>2024.12.26 405 \"Петрова А.И.\"</li>"
                     "<li>2024.12.27 101-A Сидоров</li>"
                     "</ul>");
    layout->addWidget(browser);

    QPushButton* close_btn = new QPushButton("Закрыть");
    layout->addWidget(close_btn);
    connect(close_btn, &QPushButton::clicked, dialog, &QDialog::close);

    dialog->exec();
}

void MainWindow::UpdateDisplay(const Lesson& lesson) {
    result_line_edit_->setPlainText(lesson.ToString());
    result_line_edit_->setStyleSheet("");
}

void MainWindow::ShowError(const QString& error_message) {
    result_line_edit_->setPlainText(error_message);
    result_line_edit_->setStyleSheet("QTextEdit { color: red; }");
}

void MainWindow::ClearDisplay() {
    input_text_edit_->clear();
    result_line_edit_->clear();
    result_line_edit_->setStyleSheet("");
}
