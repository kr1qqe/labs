#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <QString>
#include <QStringList>
#include "lesson.h"

struct Condition {
    enum class Type {
        DateEqual,
        DateLess,
        DateGreater,
        RoomEqual,
        RoomContains,
        TeacherEqual,
        TeacherContains
    };

    Type type;
    QString value;
};

struct Command {
    enum class Type {
        Add,
        Rem,
        Save
    };

    Type type;
    QString data;
    Condition condition;
};

class CommandParser {
public:
    explicit CommandParser(const QString& line);

    Command Parse();

    static Lesson ParseCsvToLesson(const QString& csvLine);

    static bool LessonMatchesCondition(const Lesson& lesson, const Condition& condition);

private:
    void ParseAddCommand(const QStringList& tokens);
    void ParseRemCommand(const QStringList& tokens);
    void ParseSaveCommand(const QStringList& tokens);

    Condition ParseCondition(const QString& conditionStr);

    QString input_;
    Command command_;
};

#endif
