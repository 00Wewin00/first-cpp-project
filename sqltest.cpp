#include <iostream>
#include <sqlite3.h>
#include <string>
using namespace std;
template <typename T>
void insert_data(sqlite3 *db, std::string mesto, T peremennaja) {
    if (mesto == "password" || mesto == "username") {
        std::string sql = "INSERT INTO user (" + mesto + ") VALUES(?);";
        sqlite3_stmt* stmt;
        
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
        
        // Если peremennaja это текст (std::string):
        sqlite3_bind_text(stmt, 1, peremennaja.c_str(), -1, SQLITE_STATIC);
        
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        std::cout << "[Успех] Данные записаны в колонку: " << mesto << std::endl;
    } else {
        std::cout << "[Ошибка] Неверное имя колонки!\n";
    }
}
int main() {
    sqlite3* db = nullptr;
    
    // Открываем тестовую базу
    if (sqlite3_open("test.db", &db) != SQLITE_OK) {
        cout << "Ошибка открытия базы!" << endl;
        return 1;
    }
    
    cout << "Песочница готова к работе!" << endl;
    const char* sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT, password TEXT);";
    sqlite3_exec(db, sql, 0, 0, 0);
    
    // Закрываем базу
    sqlite3_close(db);
    return 0;
}