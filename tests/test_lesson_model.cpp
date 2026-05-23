#include <QtTest>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include "../model/lesson_model.h"
#include "../model/parser.h"

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
            writeLog("ЗАПУСК МОДУЛЬНЫХ ТЕСТОВ");
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

class TestLessonModel : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        if (!testLogger) {
            testLogger = new TestLogger();
        }
    }

    void cleanupTestCase() {
        if (testLogger) {
            testLogger->writeSummary();
            delete testLogger;
            testLogger = nullptr;
        }
    }

    void testParseValidLesson() {
        QString testName = "testParseValidLesson";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("2024.12.25 301 Иванов");
            Lesson lesson = parser.Parse();

            QCOMPARE(lesson.GetDate(), QDate(2024, 12, 25));
            QCOMPARE(lesson.GetRoom(), QString("301"));
            QCOMPARE(lesson.GetTeacher(), QString("Иванов"));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testParseInvalidDateFormatThrows() {
        QString testName = "testParseInvalidDateFormatThrows";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("25.12.2024 301 Иванов");
            QVERIFY_THROWS_EXCEPTION(DateFormatException, parser.Parse());

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testAddValidLesson() {
        QString testName = "testAddValidLesson";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            LessonModel model;
            Lesson lesson(QDate(2024, 12, 25), "301", "Иванов");

            QVERIFY_NO_THROW(model.AddLesson(lesson));
            QCOMPARE(model.Count(), 1);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testLoadFromFileWithErrors() {
        QString testName = "testLoadFromFileWithErrors";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            QString filename = "temp_test_file.txt";
            QFile file(filename);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&file);
            out << "2024.12.25 301 Иванов\n";
            out << "25.12.2024 302 Петров\n";
            out << "2024.12.27 303\n";
            out << "2024.12.28 304 Сидоров\n";
            file.close();

            LessonModel model;
            model.LoadFromFile(filename);

            QCOMPARE(model.Count(), 2);

            QStringList errors = model.GetErrorLog();
            QVERIFY(errors.size() >= 2);

            QFile::remove(filename);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testSaveAndLoadRoundTrip() {
        QString testName = "testSaveAndLoadRoundTrip";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            QString filename = "temp_roundtrip.txt";

            LessonModel originalModel;
            originalModel.AddFromString("2024.12.25 301 Иванов");
            originalModel.AddFromString("2024.12.26 302 Петров");

            originalModel.SaveToFile(filename);

            LessonModel loadedModel;
            loadedModel.LoadFromFile(filename);

            QCOMPARE(loadedModel.Count(), 2);
            QCOMPARE(loadedModel.GetLesson(0).GetTeacher(), QString("Иванов"));
            QCOMPARE(loadedModel.GetLesson(1).GetTeacher(), QString("Петров"));

            QFile::remove(filename);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }
};

QTEST_MAIN(TestLessonModel)
#include "test_lesson_model.moc"
