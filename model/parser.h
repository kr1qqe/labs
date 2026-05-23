#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>
#include "lesson.h"
#include "parser_exceptions.h"

class Parser {
public:
    explicit Parser(const QString& input);

    Lesson Parse();

    static bool IsValidDateFormat(const QString& dateStr);
    static bool IsValidDate(const QString& dateStr);
    static bool IsValidRoom(const QString& roomStr);
    static bool IsValidTeacher(const QString& teacherStr);

private:
    void ValidateTokens(const QStringList& tokens);
    QString ParseDate(const QString& token);
    QString ParseRoom(const QString& token);
    QString ParseTeacher(const QStringList& tokens, int startIndex);

    QString input_;
};

#endif
