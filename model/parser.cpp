#include "parser.h"
#include <QRegularExpression>
#include <QDate>

Parser::Parser(const QString& input) : input_(input.trimmed()) {}

Lesson Parser::Parse() {
    if (input_.isEmpty()) {
        throw LineFormatException("Строка не может быть пустой");
    }

    QStringList tokens = input_.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    ValidateTokens(tokens);

    QString dateStr = ParseDate(tokens[0]);
    QString roomStr = ParseRoom(tokens[1]);

    QString teacherPart;
    for (int i = 2; i < tokens.size(); ++i) {
        if (i > 2) teacherPart += " ";
        teacherPart += tokens[i];
    }
    QString teacherStr = ParseTeacher(teacherPart.split(" "), 0);

    QDate date = QDate::fromString(dateStr, "yyyy.MM.dd");
    if (!date.isValid()) {
        throw DateFormatException("Неверная дата: " + dateStr);
    }

    return Lesson(date, roomStr, teacherStr);
}

void Parser::ValidateTokens(const QStringList& tokens) {
    if (tokens.size() < 3) {
        throw LineFormatException(
            QString("Недостаточно свойств. Найдено %1, ожидается минимум 3")
                .arg(tokens.size())
            );
    }
}

QString Parser::ParseDate(const QString& token) {
    if (!IsValidDateFormat(token)) {
        throw DateFormatException(
            QString("Неверный формат даты: %1. Используйте ГГГГ.ММ.ДД")
                .arg(token)
            );
    }
    return token;
}

QString Parser::ParseRoom(const QString& token) {
    if (!IsValidRoom(token)) {
        throw RoomFormatException(
            QString("Неверный формат аудитории: %1. Допустимы буквы, цифры, -, /")
                .arg(token)
            );
    }
    return token;
}

QString Parser::ParseTeacher(const QStringList& tokens, int startIndex) {
    QString teacherStr;

    for (int i = startIndex; i < tokens.size(); ++i) {
        QString token = tokens[i];

        if (token.startsWith('"') && !token.endsWith('"')) {
            teacherStr = token.mid(1);
            int j = i + 1;
            while (j < tokens.size() && !tokens[j].endsWith('"')) {
                teacherStr += " " + tokens[j];
                ++j;
            }
            if (j < tokens.size()) {
                teacherStr += " " + tokens[j];
                teacherStr.chop(1);
                i = j;
            }
        } else if (token.startsWith('"') && token.endsWith('"')) {
            teacherStr = token.mid(1, token.length() - 2);
        } else {
            if (!teacherStr.isEmpty()) teacherStr += " ";
            teacherStr += token;
        }
    }

    if (!IsValidTeacher(teacherStr)) {
        throw TeacherFormatException(
            QString("Неверный формат преподавателя: %1. Допустимы буквы, пробелы, ., -")
                .arg(teacherStr)
            );
    }

    return teacherStr;
}

bool Parser::IsValidDateFormat(const QString& dateStr) {
    QRegularExpression regex(R"(^\d{4}\.\d{2}\.\d{2}$)");
    return regex.match(dateStr).hasMatch();
}

bool Parser::IsValidDate(const QString& dateStr) {
    if (!IsValidDateFormat(dateStr)) return false;
    QDate date = QDate::fromString(dateStr, "yyyy.MM.dd");
    return date.isValid();
}

bool Parser::IsValidRoom(const QString& roomStr) {
    QRegularExpression regex(R"(^[A-Za-zА-Яа-я0-9\-/]+$)");
    return regex.match(roomStr).hasMatch();
}

bool Parser::IsValidTeacher(const QString& teacherStr) {
    QRegularExpression regex(R"(^[A-Za-zА-Яа-я\s\.\-]+$)");
    return regex.match(teacherStr).hasMatch();
}
