#include <QtTest>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <iostream>
#include "mainwindow.h"

#define QVERIFY_THROWS_EXCEPTION(ExceptionType, expression) \
do { \
        bool caught = false; \
    try { expression; } \
        catch (const ExceptionType&) { caught = true; } \
        catch (const std::exception& e) { \
            qDebug() << "Unexpected exception:" << e.what(); \
    } \
        catch (...) {} \
        QVERIFY2(caught, "Expected exception " #ExceptionType " not thrown"); \
} while (0)

#define QVERIFY_NO_THROW(expression) \
    do { \
    try { expression; } \
        catch (const std::exception& e) { \
            QFAIL(QString("Expected no exception, but got: %1").arg(e.what()).toUtf8()); \
    } \
        catch (...) { \
            QFAIL("Expected no exception, but got unknown exception"); \
    } \
} while (0)

    // Функция для вывода покрытия
    void printCoverage(const QString& testName) {
        std::cout << "\n  --- ПОКРЫТИЕ после " << testName.toStdString() << " ---" << std::endl;
        std::cout << "  Вызовов solve(): " << MainWindow::solveCallCount << std::endl;
        std::cout << "  Ветка L < 1: " << MainWindow::lLessThan1Count << std::endl;
        std::cout << "  Ветка L >= 1: " << MainWindow::lGreaterEqual1Count << std::endl;

        int coveredBranches = 0;
        if (MainWindow::lLessThan1Count > 0) coveredBranches++;
        if (MainWindow::lGreaterEqual1Count > 0) coveredBranches++;
        std::cout << "  Покрытие веток: " << (coveredBranches * 50) << "%" << std::endl;
        std::cout << "  ------------------------------------" << std::endl;
    }

class TestLogger {
private:
    QString logFileName;
    int totalTests;
    int passedTests;
    int failedTests;

public:
    TestLogger() : totalTests(0), passedTests(0), failedTests(0) {
        QDir dir;
        if (!dir.exists("test_logs")) {
            dir.mkdir("test_logs");
        }

        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
        logFileName = QString("test_logs/test_results_%1.log").arg(timestamp);

        writeLog("");
        writeLog("ЗАПУСК МОДУЛЬНЫХ ТЕСТОВ (ДОМИНО)");
        writeLog("Время: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        writeLog("");
    }

    void writeLog(const QString& message) {
        QFile file(logFileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << message << "\n";
            file.close();
        }
    }

    void testStarted(const QString& testName) {
        totalTests++;
        writeLog("Запуск теста " + QString::number(totalTests) + ": " + testName);
    }

    void testPassed(const QString& testName, int duration) {
        passedTests++;
        QString msg = "Тест " + QString::number(passedTests) + " ПРОЙДЕН: " + testName + " (за " + QString::number(duration) + " мс)";
        writeLog(msg);
        qDebug().noquote() << msg;
    }

    void testFailed(const QString& testName, const QString& error, int duration) {
        failedTests++;
        QString msg = "Тест " + QString::number(failedTests) + " ПРОВАЛЕН: " + testName + " (за " + QString::number(duration) + " мс)";
        writeLog(msg);
        writeLog("Ошибка: " + error);
        qDebug().noquote() << msg;
        qDebug().noquote() << "Ошибка: " << error;
    }

    void writeSummary() {
        writeLog("");
        writeLog("ИТОГИ ТЕСТИРОВАНИЯ");
        writeLog("");
        writeLog("Всего тестов: " + QString::number(totalTests));
        writeLog("Пройдено: " + QString::number(passedTests));
        writeLog("Провалено: " + QString::number(failedTests));
        writeLog("Процент успеха: " + QString::number(totalTests > 0 ? (passedTests * 100 / totalTests) : 0) + "%");
        writeLog("");
        writeLog("Время завершения: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

        qDebug().noquote() << "";
        qDebug().noquote() << "ИТОГИ ТЕСТОВ:";
        qDebug().noquote() << "Всего: " + QString::number(totalTests) + ", Пройдено: " + QString::number(passedTests) + ", Провалено: " + QString::number(failedTests) + ", Успех: " + QString::number(totalTests > 0 ? (passedTests * 100 / totalTests) : 0) + "%";
    }

    int getFailedTests() const { return failedTests; }
};

static TestLogger* testLogger = nullptr;

class TestMainWindow : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        if (!testLogger) {
            testLogger = new TestLogger();
        }
        // Обнуляем счётчики перед началом всех тестов
        MainWindow::solveCallCount = 0;
        MainWindow::lLessThan1Count = 0;
        MainWindow::lGreaterEqual1Count = 0;

        std::cout << "\n========== НАЧАЛО ТЕСТИРОВАНИЯ ==========" << std::endl;
    }

    void cleanupTestCase() {
        // Итоговое покрытие
        std::cout << "\n========== ИТОГОВОЕ ПОКРЫТИЕ ==========" << std::endl;
        std::cout << "Всего вызовов solve(): " << MainWindow::solveCallCount << std::endl;
        std::cout << "Ветка L < 1: " << MainWindow::lLessThan1Count << std::endl;
        std::cout << "Ветка L >= 1: " << MainWindow::lGreaterEqual1Count << std::endl;

        int coveredBranches = 0;
        if (MainWindow::lLessThan1Count > 0) coveredBranches++;
        if (MainWindow::lGreaterEqual1Count > 0) coveredBranches++;
        std::cout << "Покрытие веток: " << (coveredBranches * 50) << "%" << std::endl;
        std::cout << "=====================================\n" << std::endl;

        if (testLogger) {
            testLogger->writeSummary();
            delete testLogger;
            testLogger = nullptr;
        }
    }

    // Тест 1: Обычные значения
    void testNormalValues() {
        QString testName = "testNormalValues";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            MainWindow window;
            int result = window.solve(10, 100);
            QCOMPARE(result, 12);
            testLogger->testPassed(testName, timer.elapsed());
            printCoverage(testName);  // Вывод покрытия после теста
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    // Тест 2: Маленькая высота (L становится 1)
    void testSmallHeight() {
        QString testName = "testSmallHeight";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            MainWindow window;
            int result = window.solve(1, 5);
            QCOMPARE(result, 4);
            testLogger->testPassed(testName, timer.elapsed());
            printCoverage(testName);
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    // Тест 3: Радиус меньше длины сегмента
    void testRadiusLessThanSegment() {
        QString testName = "testRadiusLessThanSegment";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            MainWindow window;
            int result = window.solve(100, 5);
            QCOMPARE(result, 0);
            testLogger->testPassed(testName, timer.elapsed());
            printCoverage(testName);
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    // Тест 4: Нулевая высота
    void testZeroHeight() {
        QString testName = "testZeroHeight";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            MainWindow window;
            int result = window.solve(0, 100);
            QCOMPARE(result, 99);
            testLogger->testPassed(testName, timer.elapsed());
            printCoverage(testName);
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    // Тест 5: Отрицательный радиус
    void testNegativeRadius() {
        QString testName = "testNegativeRadius";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            MainWindow window;
            int result = window.solve(10, -50);
            QCOMPARE(result, -6);
            testLogger->testPassed(testName, timer.elapsed());
            printCoverage(testName);
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }
};

QTEST_MAIN(TestMainWindow)
#include "test_mainwindow.moc"
