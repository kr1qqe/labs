#include "lesson_model.h"
#include "parser.h"
#include "command_parser.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>

LessonModel::LessonModel() {}

void LessonModel::AddLesson(const Lesson& lesson) {
    if (!lesson.IsValid()) {
        throw ParseException("Некорректное занятие");
    }
    lessons_.append(lesson);
}

void LessonModel::AddFromString(const QString& line) {
    Parser parser(line);
    Lesson lesson = parser.Parse();
    AddLesson(lesson);
}

bool LessonModel::RemoveLesson(int index) {
    if (index >= 0 && index < lessons_.size()) {
        lessons_.removeAt(index);
        return true;
    }
    return false;
}

Lesson LessonModel::GetLesson(int index) const {
    if (index >= 0 && index < lessons_.size()) {
        return lessons_[index];
    }
    throw ParseException("Индекс вне диапазона");
}

QList<Lesson> LessonModel::GetAllLessons() const {
    return lessons_;
}

QStringList LessonModel::GetLessonsStringList() const {
    QStringList result;
    for (const Lesson& lesson : lessons_) {
        result << lesson.ToString();
    }
    return result;
}

int LessonModel::Count() const {
    return lessons_.size();
}

void LessonModel::Clear() {
    lessons_.clear();
}

void LessonModel::LogError(const QString& errorType, const QString& context, const QString& errorMsg) const {
    QString logEntry = QString("%1 | %2 | %3 | %4")
    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
        .arg(errorType)
        .arg(context)
        .arg(errorMsg);

    qDebug() << logEntry;

    QDir dir;
    if (!dir.exists("logs")) {
        dir.mkdir("logs");
    }

    QFile logFile("logs/errors.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&logFile);
        stream << logEntry << "\n";
        logFile.close();
    }

    error_log_.append(logEntry);
}

void LessonModel::LoadFromFile(const QString& filename) {
    QFile file(filename);

    if (!file.exists()) {
        LogError("FileNotFound", filename, "Файл не найден");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LogError("OpenError", filename, "Не удалось открыть файл");
        return;
    }

    QTextStream stream(&file);
    int lineNum = 0;

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        lineNum++;

        if (line.trimmed().isEmpty()) continue;

        try {
            AddFromString(line);
        } catch (const ParseException& e) {
            LogError(QString(typeid(e).name()),
                     QString("Строка %1: %2").arg(lineNum).arg(line),
                     e.getMessage());
        }
    }

    file.close();
}

bool LessonModel::SaveToFile(const QString& filename) const {
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LogError("SaveError", filename, "Не удалось открыть файл для записи");
        return false;
    }

    QTextStream stream(&file);
    for (const Lesson& lesson : lessons_) {
        stream << lesson.GetDate().toString("yyyy.MM.dd") << " "
               << lesson.GetRoom() << " "
               << lesson.GetTeacher() << "\n";
    }

    file.close();
    return true;
}

QStringList LessonModel::GetErrorLog() const {
    return error_log_;
}

void LessonModel::ClearErrorLog() {
    error_log_.clear();
}

LessonModel::CommandResult LessonModel::ExecuteCommand(const QString& commandLine) {
    CommandResult result;
    result.success = false;
    result.affectedRows = 0;

    try {
        CommandParser parser(commandLine);
        Command cmd = parser.Parse();

        switch (cmd.type) {
        case Command::Type::Add: {
            try {
                Lesson lesson = CommandParser::ParseCsvToLesson(cmd.data);
                AddLesson(lesson);
                result.affectedRows = 1;
                result.message = QString("Добавлено занятие: %1").arg(lesson.ToString());
                result.success = true;
            } catch (const ParseException& e) {
                result.message = QString("Ошибка добавления: %1").arg(e.getMessage());
                LogError("AddError", cmd.data, e.getMessage());
            }
            break;
        }

        case Command::Type::Rem: {
            int removed = RemoveByCondition(cmd.condition);
            result.affectedRows = removed;
            result.message = QString("Удалено занятий: %1").arg(removed);
            result.success = true;
            break;
        }

        case Command::Type::Save: {
            if (SaveToFile(cmd.data)) {
                result.message = QString("Данные сохранены в файл: %1").arg(cmd.data);
                result.success = true;
            } else {
                result.message = QString("Ошибка сохранения в файл: %1").arg(cmd.data);
            }
            break;
        }
        }
    } catch (const ParseException& e) {
        result.message = QString("Ошибка парсинга команды: %1").arg(e.getMessage());
        LogError("CommandParseError", commandLine, e.getMessage());
    }

    return result;
}

int LessonModel::RemoveByCondition(const Condition& condition) {
    int removed = 0;

    for (int i = lessons_.size() - 1; i >= 0; --i) {
        if (CommandParser::LessonMatchesCondition(lessons_[i], condition)) {
            lessons_.removeAt(i);
            removed++;
        }
    }

    return removed;
}
