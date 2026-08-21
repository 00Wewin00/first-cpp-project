#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include "db.h"
using namespace std;
struct clientinfo
{
    int socet;
    string username;
};
void insert_data(string mesto,string imja,string password) 
{
    sqlite3* db = nullptr;
    // Открываем тестовую базу
    if (sqlite3_open("test.db", &db) != SQLITE_OK) 
    {
        cout << "Ошибка открытия базы!" << endl;
        return;
    }
    
    string sql = "INSERT OR IGNORE INTO user (username, password) VALUES (?, ?);";
    sqlite3_stmt* stmt = nullptr;
    int status = 0;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) 
    {
        sqlite3_bind_text(stmt, 1, imja.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        // Проверяем, была ли реально добавлена строка
        if (sqlite3_changes(db) > 0) 
        {
            //soedinenie(client_socet, client);
        } 
        else 
        {
            status = 0; // Ошибка: такой username уже занят
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    cout << "[Успех] Данные записаны в колонку: " << mesto << endl;
}