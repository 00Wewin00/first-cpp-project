#include <iostream>
#include <sqlite3.h>
using namespace std;
int main() {
    sqlite3* db;
    
    int rc = sqlite3_open("chat.db", &db);
    
    if (rc == 0) {
        cout << "Baza uspeshno otkryta!\n";
        const char* sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT, password TEXT);";
        sqlite3_exec(db, sql, 0, 0, 0);
        const char* insert_sql = "INSERT INTO users (username, password) VALUES ('ivan', '12345');";
        sqlite3_exec(db, insert_sql, 0, 0, 0);
        sqlite3_close(db);
    } else {
        cout << "Oshibka: " << rc << "\n";
    }
    return 0;
}