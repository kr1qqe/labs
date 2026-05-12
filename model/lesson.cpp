#include "lesson.h"

Lesson::Lesson() : date_(QDate::currentDate()), room_(""), teacher_("") {}

Lesson::Lesson(const QDate& date, const QString& room, const QString& teacher)
    : date_(date), room_(room), teacher_(teacher) {}

QDate Lesson::GetDate() const { return date_; }
QString Lesson::GetRoom() const { return room_; }
QString Lesson::GetTeacher() const { return teacher_; }

void Lesson::SetDate(const QDate& date) { date_ = date; }
void Lesson::SetRoom(const QString& room) { room_ = room; }
void Lesson::SetTeacher(const QString& teacher) { teacher_ = teacher; }

QString Lesson::ToString() const {
    return QString("%1 | Аудитория: %2 | Преподаватель: %3")
        .arg(date_.toString("yyyy.MM.dd"))
        .arg(room_)
        .arg(teacher_);
}

bool Lesson::IsValid() const {
    return date_.isValid() && !room_.isEmpty() && !teacher_.isEmpty();
}
