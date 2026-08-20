#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <algorithm>
#include <netinet/in.h>
#include <sqlite3.h>
#include <sstream>
using namespace std;
mutex clients_mutex;
void rassylka(const char*buffer,const vector<int> &client,int socket){
    lock_guard<mutex> lock(clients_mutex);
    for(int sock:client){
        if(sock!=socket){
            send(sock, buffer, strlen(buffer), 0);
            }
        }
    }
void pryem_sms(int socket,vector<int>& client)
{
    char buffer[1024];
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) 
        {
            cout << "Клиент отключился.\n";
            {
                lock_guard<mutex> lock(clients_mutex);
                auto it = find(client.begin(), client.end(), socket);
                if (it != client.end()) 
                {
                    // 3. Удаляем элемент по найденному итератору
                    client.erase(it);
                }
            }
            break; 
        }
        else
        {
            cout<<"user"<<socket<<" : "<<buffer<<"\n";
            rassylka(buffer,client,socket);
        }
    }
}
void soedinenie(int client_socket,vector<int>&client)
{
    {
        lock_guard<mutex> lock(clients_mutex);
        client.push_back(client_socket);
    }
    thread msg(pryem_sms,client_socket,ref(client));
    msg.detach();
    cout << "Клиент подключился!" << endl;
}
void insert_data(string mesto,string imja,string password,vector<int>&client,int client_socet) 
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
void registracyja(int client_socet,vector<int>&client)
{
    char buffer [1024];
    int whfjfjcjohvo=1;
    while(1==1)
    {
        memset(buffer, 0, sizeof(buffer));
        int baeyt=recv(client_socet,buffer,sizeof(buffer) -1,0);
        if (baeyt>0)
        {
            string msg(buffer);
            if (msg=="/reg")
            {
                string otwet="<usernamy> <password> <password>";
                send(client_socet,otwet.c_str(),otwet.size(),0);
                char buffer2[1024];
                int baeyt2=recv(client_socet,buffer2,sizeof(buffer2) -1,0);
                if (baeyt2>0)
                { 
                    string dany(buffer2,baeyt2);
                    stringstream ss(dany);
                    
                    string username, pass1, pass2;
                 // Считываем данные по очереди
                    if (ss >> username >> pass1 >> pass2) 
                    {
                        // Проверка прошла: все три переменные успешно заполнились
                        if(pass1==pass2)
                        {
                            cout << "Успех! Ник: " << username << ", Пароль: " << pass1 << endl;
                            insert_data("user", username, pass1, client, client_socet);
                            string otwet ="waszy danye sohraneny";
                            send(client_socet,otwet.c_str(),otwet.size(),0);
                            soedinenie(client_socet, client);
                            whfjfjcjohvo=1;
                        }
                        else if(pass1!=pass2)
                        {
                            string oszybka ="paroli dolrzny sowpadat";
                            send(client_socet,oszybka.c_str(),oszybka.size(),0);
                        }
                    }   
                    else 
                    {
                        // Ошибка: слов в строке оказалось меньше, чем нужно
                        string oszybka ="paroli dolrzny sowpadat";
                        send(client_socet,oszybka.c_str(),oszybka.size(),0);
                        cout << "Ошибка разделения: не хватает данных!" << endl;
                    }
                }
            }
            else if(msg=="/log")
            {
                string otwet="<usernamy> <password>";
                send(client_socet,otwet.c_str(),otwet.size(),0);
                char buffer2[1024];
                int baeyt2=recv(client_socet,buffer2,sizeof(buffer2) -1,0);
                if (baeyt2>0)
                { 
                    string dany(buffer2,baeyt2);
                    stringstream ss(dany);
                    
                    string username, pass;
                 // Считываем данные по очереди
                    if (ss >> username >> pass) 
                    {
                        sqlite3_stmt* stmt = nullptr;
                        sqlite3* db = nullptr;
                        sqlite3_open("test.db", &db);
                        cout<<1;
                        string sql ="SELECT id FROM user WHERE username = ? AND password = ?;";
                        cout<<2;
                        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                        sqlite3_bind_text(stmt,1,username.c_str(),-1,SQLITE_STATIC);
                        cout<<3;
                        sqlite3_bind_text(stmt,2,pass.c_str(),-1,SQLITE_STATIC);
                        if (sqlite3_step(stmt) == SQLITE_ROW) 
                        {
                            cout<<4;
                            soedinenie(client_socet, client);
                            //whfjfjcjohvo=2;
                        }
                        else if(1==1)
                        {
                            cout<<"3\n";
                            string sms1232="неверный логин или пароль";
                            send(client_socet,sms1232.c_str(),sms1232.size(),0);
                        }
                        sqlite3_finalize(stmt);
                        sqlite3_close(db);
                    }
                }
            }
            else if(true)
            {
                string otwet="reg/log";
                send(client_socet,otwet.c_str(),otwet.size(),0);
            }
        }
    }
}
void podkluczenie(vector<int>&client,int serverfd)
{
    if (listen(serverfd, 5) < 0) 
    {
        cout << "Ошибка при вызове listen!\n";
        return;
    }
    cout << "Сервер слушает порт и ждет клиентов...\n";
    while(true)
    {
        int client_socket = accept(serverfd, nullptr, nullptr);
        std::string response = "reg/log";
        send(client_socket, response.c_str(), response.size(), 0);
        thread reg(registracyja,client_socket,ref(client));
        reg.detach();
    }
}
int main() 
{
    // 1. Создаем "телефонный аппарат"
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // 2. Настраиваем номер (порт 8080)
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    // 3. Подключаем аппарат к розетке
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    // 4. Переводим в режим "жду звонка"
    vector<int>clients;
    thread prosluszka(podkluczenie,ref(clients),server_fd);
    prosluszka.detach();
    //close(clients);
    while(true)
    {
        sleep(1);
    }
    close(server_fd);
    return 0;
}