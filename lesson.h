#ifndef LESSON_H
#define LESSON_H

#include <QString>
#include <QDate>
#include <QTime>

class Lesson {
public:
    Lesson();
    Lesson(const QDate& date, const QString& room, const QString& teacher);

    QDate GetDate() const;
    QString GetRoom() const;
    QString GetTeacher() const;

    void SetDate(const QDate& date);
    void SetRoom(const QString& room);
    void SetTeacher(const QString& teacher);

    QString ToString() const;

    bool IsValid() const;

private:
    QDate date_;
    QString room_;
    QString teacher_;
};

#endif
