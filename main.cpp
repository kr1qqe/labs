#include <QApplication>
#include "mainwindow.h"
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Главное окно
    MainWindow window;
    window.show();

    // Второе окно
    QDialog* second_window = new QDialog();
    second_window->setWindowTitle("Загрузка данных");
    second_window->resize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout(second_window);

    QTextEdit* text_edit = new QTextEdit();
    text_edit->setPlaceholderText("Содержимое файла будет здесь");
    layout->addWidget(text_edit);

    QHBoxLayout* btn_layout = new QHBoxLayout();
    QPushButton* open_btn = new QPushButton("Открыть");
    QPushButton* process_btn = new QPushButton("Обработать");

    btn_layout->addStretch();
    btn_layout->addWidget(open_btn);
    btn_layout->addWidget(process_btn);
    btn_layout->addStretch();
    layout->addLayout(btn_layout);

    QString current_data;

    // Открыть файл
    QObject::connect(open_btn, &QPushButton::clicked, [&]() {
        QString path = QFileDialog::getOpenFileName(second_window, "Открыть текстовый файл", "", "Текстовые файлы (*.txt)");

        if (!path.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                current_data = stream.readAll();
                file.close();
                text_edit->setPlainText(current_data);
            } else {
                QMessageBox::warning(second_window, "Ошибка", "Не удалось открыть файл");
            }
        }
    });

    QObject::connect(process_btn, &QPushButton::clicked, [&]() {
        if (current_data.isEmpty()) {
            QMessageBox::warning(second_window, "Ошибка", "Сначала откройте файл с данными");
        } else {
            window.ProcessMultipleLines(current_data);

        }
    });

    second_window->show();

    return app.exec();
}
