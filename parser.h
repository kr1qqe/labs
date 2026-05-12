#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>
#include "lesson.h"

class Parser {
public:
    explicit Parser(const QString& input);

    bool Parse();

    Lesson GetLesson() const;

    QString GetErrorMessage() const;

private:
    bool ParseDate(const QString& token);
    bool ParseRoom(const QString& token);
    bool ParseTeacher(const QStringList& tokens, int start_index);

    bool IsValidDate(const QString& date_str) const;
    bool IsValidRoom(const QString& room_str) const;
    bool IsValidTeacher(const QString& teacher_str) const;

    QString input_;
    QString error_message_;
    Lesson lesson_;
    int current_index_;
};

#endif
