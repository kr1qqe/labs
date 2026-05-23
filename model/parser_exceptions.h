#ifndef PARSER_EXCEPTIONS_H
#define PARSER_EXCEPTIONS_H

#include <QString>
#include <exception>

class ParseException : public std::exception {
protected:
    QString message_;
public:
    explicit ParseException(const QString& msg) : message_(msg) {}
    virtual ~ParseException() = default;

    const char* what() const noexcept override {
        return message_.toUtf8().constData();
    }

    QString getMessage() const { return message_; }
};

class DateFormatException : public ParseException {
public:
    explicit DateFormatException(const QString& msg) : ParseException(msg) {}
};

class RoomFormatException : public ParseException {
public:
    explicit RoomFormatException(const QString& msg) : ParseException(msg) {}
};

class TeacherFormatException : public ParseException {
public:
    explicit TeacherFormatException(const QString& msg) : ParseException(msg) {}
};

class LineFormatException : public ParseException {
public:
    explicit LineFormatException(const QString& msg) : ParseException(msg) {}
};

#endif
