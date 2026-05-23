#include "command_parser.h"
#include "parser_exceptions.h"
#include <QRegularExpression>
#include <QDate>

CommandParser::CommandParser(const QString& line) : input_(line.trimmed()) {}

Command CommandParser::Parse() {
    if (input_.isEmpty()) {
        throw ParseException("Пустая команда");
    }

    QStringList tokens = input_.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    if (tokens.isEmpty()) {
        throw ParseException("Не указана команда");
    }

    QString cmd = tokens[0].toUpper();

    if (cmd == "ADD") {
        ParseAddCommand(tokens);
    } else if (cmd == "REM") {
        ParseRemCommand(tokens);
    } else if (cmd == "SAVE") {
        ParseSaveCommand(tokens);
    } else {
        throw ParseException(QString("Неизвестная команда: %1").arg(cmd));
    }

    return command_;
}

void CommandParser::ParseAddCommand(const QStringList& tokens) {
    if (tokens.size() < 2) {
        throw ParseException("Команде ADD требуется аргумент (данные в формате CSV)");
    }

    QString csvData;
    for (int i = 1; i < tokens.size(); ++i) {
        if (i > 1) csvData += " ";
        csvData += tokens[i];
    }

    command_.type = Command::Type::Add;
    command_.data = csvData;
}

void CommandParser::ParseRemCommand(const QStringList& tokens) {
    if (tokens.size() < 2) {
        throw ParseException("Команде REM требуется условие");
    }

    QString conditionStr;
    for (int i = 1; i < tokens.size(); ++i) {
        if (i > 1) conditionStr += " ";
        conditionStr += tokens[i];
    }

    command_.type = Command::Type::Rem;
    command_.condition = ParseCondition(conditionStr);
}

void CommandParser::ParseSaveCommand(const QStringList& tokens) {
    if (tokens.size() < 2) {
        throw ParseException("Команде SAVE требуется имя файла");
    }

    command_.type = Command::Type::Save;
    command_.data = tokens[1];
}

Condition CommandParser::ParseCondition(const QString& conditionStr) {
    Condition cond;

    QRegularExpression dateEqualRegex(R"(^date\s*==\s*(\d{4}\.\d{2}\.\d{2})$)");
    QRegularExpression dateLessRegex(R"(^date\s*<\s*(\d{4}\.\d{2}\.\d{2})$)");
    QRegularExpression dateGreaterRegex(R"(^date\s*>\s*(\d{4}\.\d{2}\.\d{2})$)");
    QRegularExpression roomEqualRegex(R"(^room\s*==\s*(.+)$)");
    QRegularExpression roomContainsRegex(R"(^room\s+contains\s+(.+)$)");
    QRegularExpression teacherEqualRegex(R"(^teacher\s*==\s*(.+)$)");
    QRegularExpression teacherContainsRegex(R"(^teacher\s+contains\s+(.+)$)");

    QRegularExpressionMatch match;

    if ((match = dateEqualRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::DateEqual;
        cond.value = match.captured(1);
    } else if ((match = dateLessRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::DateLess;
        cond.value = match.captured(1);
    } else if ((match = dateGreaterRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::DateGreater;
        cond.value = match.captured(1);
    } else if ((match = roomEqualRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::RoomEqual;
        cond.value = match.captured(1).trimmed();
        if (cond.value.startsWith('"') && cond.value.endsWith('"')) {
            cond.value = cond.value.mid(1, cond.value.length() - 2);
        }
    } else if ((match = roomContainsRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::RoomContains;
        cond.value = match.captured(1).trimmed();
        if (cond.value.startsWith('"') && cond.value.endsWith('"')) {
            cond.value = cond.value.mid(1, cond.value.length() - 2);
        }
    } else if ((match = teacherEqualRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::TeacherEqual;
        cond.value = match.captured(1).trimmed();
        if (cond.value.startsWith('"') && cond.value.endsWith('"')) {
            cond.value = cond.value.mid(1, cond.value.length() - 2);
        }
    } else if ((match = teacherContainsRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::TeacherContains;
        cond.value = match.captured(1).trimmed();
        if (cond.value.startsWith('"') && cond.value.endsWith('"')) {
            cond.value = cond.value.mid(1, cond.value.length() - 2);
        }
    } else {
        throw ParseException(QString("Неизвестное условие: %1").arg(conditionStr));
    }

    return cond;
}

Lesson CommandParser::ParseCsvToLesson(const QString& csvLine) {
    QStringList parts = csvLine.split(';');

    if (parts.size() < 3) {
        throw ParseException(QString("CSV строка должна содержать 3 поля (дата; аудитория; преподаватель). Найдено: %1")
                                 .arg(parts.size()));
    }

    QString dateStr = parts[0].trimmed();
    QString roomStr = parts[1].trimmed();
    QString teacherStr = parts[2].trimmed();

    if (teacherStr.startsWith('"') && teacherStr.endsWith('"')) {
        teacherStr = teacherStr.mid(1, teacherStr.length() - 2);
    }

    QDate date = QDate::fromString(dateStr, "yyyy.MM.dd");
    if (!date.isValid()) {
        date = QDate::fromString(dateStr, "dd.MM.yyyy");
    }
    if (!date.isValid()) {
        throw ParseException(QString("Неверный формат даты: %1").arg(dateStr));
    }

    if (roomStr.isEmpty()) {
        throw ParseException("Аудитория не может быть пустой");
    }

    if (teacherStr.isEmpty()) {
        throw ParseException("Преподаватель не может быть пустым");
    }

    return Lesson(date, roomStr, teacherStr);
}

bool CommandParser::LessonMatchesCondition(const Lesson& lesson, const Condition& condition) {
    switch (condition.type) {
    case Condition::Type::DateEqual: {
        QDate condDate = QDate::fromString(condition.value, "yyyy.MM.dd");
        return lesson.GetDate() == condDate;
    }
    case Condition::Type::DateLess: {
        QDate condDate = QDate::fromString(condition.value, "yyyy.MM.dd");
        return lesson.GetDate() < condDate;
    }
    case Condition::Type::DateGreater: {
        QDate condDate = QDate::fromString(condition.value, "yyyy.MM.dd");
        return lesson.GetDate() > condDate;
    }
    case Condition::Type::RoomEqual:
        return lesson.GetRoom().compare(condition.value, Qt::CaseInsensitive) == 0;
    case Condition::Type::RoomContains:
        return lesson.GetRoom().contains(condition.value, Qt::CaseInsensitive);
    case Condition::Type::TeacherEqual:
        return lesson.GetTeacher().compare(condition.value, Qt::CaseInsensitive) == 0;
    case Condition::Type::TeacherContains:
        return lesson.GetTeacher().contains(condition.value, Qt::CaseInsensitive);
    }

    return false;
}
