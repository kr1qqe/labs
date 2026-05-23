#include <iostream>
#include <sstream>
#include <locale>

using namespace std;

struct Lesson
{
    string date;
    string room;
    string teacher;
};

int main()
{
    setlocale(LC_ALL, "");

    string input;
    getline(cin, input);

    Lesson lesson;

    stringstream ss(input);

    ss >> lesson.date;

    char quote;

    ss >> quote;
    getline(ss, lesson.room, '"');

    ss >> quote;
    getline(ss, lesson.teacher, '"');

    cout << "Дата: " << lesson.date << endl;
    cout << "Аудитория: " << lesson.room << endl;
    cout << "Преподаватель: " << lesson.teacher << endl;

    return 0;
}
