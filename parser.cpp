#include "parser.h"
#include <QRegularExpression>
#include <QStringList>
#include <QDate>

Parser::Parser(const QString& input)
    : input_(input.trimmed()), error_message_(""), current_index_(0) {}

bool Parser::Parse() {
    if (input_.isEmpty()) {
        error_message_ = "Ошибка: строка не может быть пустой";
        return false;
    }

    QStringList tokens = input_.split(QRegularExpression("\\s+"),
                                      Qt::SkipEmptyParts);

    if (tokens.size() < 3) {
        error_message_ = "Ошибка: недостаточно свойств. "
                         "Необходимо указать дату, аудиторию и преподавателя";
        return false;
    }

    if (!ParseDate(tokens[0])) {
        return false;
    }

    if (!ParseRoom(tokens[1])) {
        return false;
    }

    QString teacher_part;
    for (int i = 2; i < tokens.size(); ++i) {
        if (i > 2) teacher_part += " ";
        teacher_part += tokens[i];
    }

    if (!ParseTeacher(teacher_part.split(" "), 0)) {
        return false;
    }

    return true;
}

bool Parser::ParseDate(const QString& token) {
    if (!IsValidDate(token)) {
        error_message_ = "Ошибка: неверный формат даты. "
                         "Используйте формат ГГГГ.ММ.ДД (например, 2024.12.25)";
        return false;
    }

    QDate date = QDate::fromString(token, "yyyy.MM.dd");
    if (!date.isValid()) {
        error_message_ = "Ошибка: неверная дата";
        return false;
    }

    lesson_.SetDate(date);
    return true;
}

bool Parser::ParseRoom(const QString& token) {
    if (!IsValidRoom(token)) {
        error_message_ = "Ошибка: неверный формат аудитории. "
                         "Аудитория должна содержать буквы, цифры и символы -/";
        return false;
    }

    lesson_.SetRoom(token);
    return true;
}

bool Parser::ParseTeacher(const QStringList& tokens, int start_index) {
    QString teacher_str;

    for (int i = start_index; i < tokens.size(); ++i) {
        QString token = tokens[i];

        if (token.startsWith('"') && !token.endsWith('"')) {
            teacher_str = token.mid(1);
            int j = i + 1;
            while (j < tokens.size() && !tokens[j].endsWith('"')) {
                teacher_str += " " + tokens[j];
                ++j;
            }
            if (j < tokens.size()) {
                teacher_str += " " + tokens[j];
                teacher_str.chop(1);
                i = j;
            }
        } else if (token.startsWith('"') && token.endsWith('"')) {
            teacher_str = token.mid(1, token.length() - 2);
        } else {
            if (!teacher_str.isEmpty()) teacher_str += " ";
            teacher_str += token;
        }
    }

    if (!IsValidTeacher(teacher_str)) {
        error_message_ = "Ошибка: неверный формат имени преподавателя. "
                         "Имя должно содержать только буквы, пробелы, точки и дефисы";
        return false;
    }

    lesson_.SetTeacher(teacher_str);
    return true;
}

bool Parser::IsValidDate(const QString& date_str) const {
    QRegularExpression regex(R"(^\d{4}\.\d{2}\.\d{2}$)");
    return regex.match(date_str).hasMatch();
}

bool Parser::IsValidRoom(const QString& room_str) const {
    // Аудитория: буквы, цифры, дефис, слеш
    QRegularExpression regex(R"(^[A-Za-zА-Яа-я0-9\-/]+$)");
    return regex.match(room_str).hasMatch();
}

bool Parser::IsValidTeacher(const QString& teacher_str) const {
    // Имя преподавателя: буквы, пробелы, точки, дефисы
    QRegularExpression regex(R"(^[A-Za-zА-Яа-я\s\.\-]+$)");
    return regex.match(teacher_str).hasMatch();
}

Lesson Parser::GetLesson() const {
    return lesson_;
}

QString Parser::GetErrorMessage() const {
    return error_message_;
}
