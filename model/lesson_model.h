#ifndef LESSON_MODEL_H
#define LESSON_MODEL_H

#include <QString>
#include <QList>
#include "lesson.h"

struct Condition;

class LessonModel {
public:
    LessonModel();

    void AddLesson(const Lesson& lesson);
    void AddFromString(const QString& line);

    bool RemoveLesson(int index);

    Lesson GetLesson(int index) const;
    QList<Lesson> GetAllLessons() const;
    QStringList GetLessonsStringList() const;
    int Count() const;
    void Clear();

    void LoadFromFile(const QString& filename);
    bool SaveToFile(const QString& filename) const;

    QStringList GetErrorLog() const;
    void ClearErrorLog();

    struct CommandResult {
        bool success;
        QString message;
        int affectedRows;

        CommandResult() : success(false), affectedRows(0) {}
    };

    CommandResult ExecuteCommand(const QString& commandLine);

    int RemoveByCondition(const Condition& condition);

private:
    QList<Lesson> lessons_;
    mutable QStringList error_log_;

    void LogError(const QString& errorType, const QString& context, const QString& errorMsg) const;
};

#endif
