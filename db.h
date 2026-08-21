#ifndef DB_H
#define DB_H

// Обязательно нужны эти инклюды, иначе C++ не поймет std::string и std::vector в заголовке!
#include <string>
#include <vector>
using namespace std;

// Объявление функции (сигнатура)
void insert_data(string mesto,string imja,string password);
#endif