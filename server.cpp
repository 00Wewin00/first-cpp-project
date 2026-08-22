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
#include "db.h"
#include <sstream>
using namespace std;
mutex clients_mutex;
struct ClientInfo
{
    int socket;
    string username;
};
void w(const string sms,const vector<ClientInfo>& client,int socket,string username)
{
    int recipient_socket=-1;
    vector<ClientInfo> copyclient;
    string final_sms;
    {
        lock_guard<mutex> lock(clients_mutex);
        copyclient=client;
    }
        auto it = find_if(copyclient.begin(),copyclient.end(),[username](const auto& c)
    {
        return c.username == username;
    });
    if(it!=copyclient.end())
    {
        auto it1=find_if(copyclient.begin(),copyclient.end(),[socket](const auto& c)
        {
            return c.socket==socket;
        });
        if(it1!=copyclient.end())
        {
            string sender_name = it1->username;
            final_sms = "\"" + sender_name + "\" : " + sms;
        }
        else 
        {
            final_sms="\"Неизвестный\" : " + sms;
        }
        final_sms+="\n";
        recipient_socket = it ->socket;
        send(recipient_socket,final_sms.c_str(),final_sms.size(),0);
    }
}

void broadcast(const char*buffer,const vector<ClientInfo>& client,int socket)
{
    vector<ClientInfo> copyclient;
    string sender_name= "Неизвестный";
    {lock_guard<mutex> lock(clients_mutex);
    copyclient=client;
    }
    auto it =find_if(copyclient.begin(),copyclient.end(),[socket](const auto& c)
    {
        return c.socket==socket;
    });
    if(it!=copyclient.end())
    {
        sender_name= it ->username;
    }
    string msg(buffer);
    msg += "\n";
    sender_name += " ]  :";
    sender_name = '['+sender_name;
    msg = sender_name + msg;
    for(const auto&sock:copyclient)
    {
        if(sock.socket!=socket)
        {
            send(sock.socket, msg.c_str(), msg.size(), 0);
        }
    }
}
void handle_client(int socket,vector<ClientInfo>& client)
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
                auto it = find_if(client.begin(), client.end(), [socket](const ClientInfo& c) 
                {
                    return c.socket == socket;
                });
                if (it != client.end())
                {
                    // 3. Удаляем элемент по найденному итератору
                    client.erase(it);
                }
            }
            break; 
        }
        string command(buffer, 10);
        if (command.starts_with("/w"))
        {
            string cmd,username,sms;
            stringstream ss(buffer);
            if (ss>>cmd>>username&&getline(ss,sms))
            {
                w(sms,client,socket,username);
            }
                
        }
        else
        {
            cout<<"user"<<socket<<" : "<<buffer<<"\n";
            broadcast(buffer,client,socket);
        }
    }
}
void soedinenie(int client_socket,vector<ClientInfo>&client,string username)
{
    {
        lock_guard<mutex> lock(clients_mutex);
        client.push_back({client_socket,username});
    }
    thread msg(handle_client,client_socket,ref(client));
    msg.detach();
    cout << "Клиент подключился!" << endl;
}
void handle_auth(int client_socet,vector<ClientInfo>&client)
{
    char buffer [1024];
    bool login=true;
    while(login)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received=recv(client_socet,buffer,sizeof(buffer) -1,0);
        if (bytes_received>0)
        {
            string msg(buffer);
            if (msg=="/reg")
            {
                string response="<usernamy> <password> <password>\n";
                send(client_socet,response.c_str(),response.size(),0);
                char buffer2[1024];
                int bytes_received2=recv(client_socet,buffer2,sizeof(buffer2) -1,0);
                if (bytes_received2>0)
                { 
                    string input_data(buffer2,bytes_received2);
                    stringstream ss(input_data);
                    
                    string username, pass1, pass2;
                 // Считываем данные по очереди
                    if (ss >> username >> pass1 >> pass2) 
                    {
                        // Проверка прошла: все три переменные успешно заполнились
                        if(pass1==pass2)
                        {
                            cout << "Успех! Ник: " << username << ", Пароль: " << pass1 << endl;
                            insert_data("user", username, pass1);
                            string response ="waszy input_datae sohraneny";
                            send(client_socet,response.c_str(),response.size(),0);
                            soedinenie(client_socet, client,username);
                            login=false;
                        }
                        else if(pass1!=pass2)
                        {
                            string error_msg ="paroli dolrzny sowpadat";
                            send(client_socet,error_msg.c_str(),error_msg.size(),0);
                        }
                    }   
                    else 
                    {
                        // Ошибка: слов в строке оказалось меньше, чем нужно
                        string error_msg ="<usernamy> <password> <password>\n";
                        send(client_socet,error_msg.c_str(),error_msg.size(),0);
                        cout << "Ошибка разделения: не хватает данных!" << endl;
                    }
                }
            }
            else if(msg=="/log")
            {
                string response="<usernamy> <password>\n";
                send(client_socet,response.c_str(),response.size(),0);
                char buffer2[1024];
                int bytes_received2=recv(client_socet,buffer2,sizeof(buffer2) -1,0);
                if (bytes_received2>0)
                { 
                    string input_data(buffer2,bytes_received2);
                    stringstream ss(input_data);
                    string username, pass;
                    bool razdel = false;
                    if (ss >> username >> pass)
                    {
                        razdel=true;
                    }
                    bool is_already_online=false;
                    if (razdel)
                    {
                        {
                            lock_guard<mutex> lock(clients_mutex);
                            auto it=find_if(client.begin(),client.end(),[&username](const ClientInfo c)
                            {
                                return c.username==username;
                            });
                            if(it!=client.end()) is_already_online=true;
                        }
                    }
                    if (!is_already_online) 
                    {
                        sqlite3_stmt* stmt = nullptr;
                        sqlite3* db = nullptr;
                        sqlite3_open("test.db", &db);
                        string sql ="SELECT id FROM user WHERE username = ? AND password = ?;";
                        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
                        sqlite3_bind_text(stmt,1,username.c_str(),-1,SQLITE_STATIC);
                        sqlite3_bind_text(stmt,2,pass.c_str(),-1,SQLITE_STATIC);
                        if (sqlite3_step(stmt) == SQLITE_ROW) 
                        {
                            soedinenie(client_socet, ref(client),username);
                            login=false;
                        }
                        if(is_already_online){
                            string respone="Этот пользователь уже в сети\n";
                            send(client_socet,respone.c_str(),respone.size(),0);
                        }
                        else if(!razdel)
                        {
                            string sms1232="неверный логин или пароль\n";
                            send(client_socet,sms1232.c_str(),sms1232.size(),0);
                        }
                        sqlite3_finalize(stmt);
                        sqlite3_close(db);
                    }
                    if(is_already_online)
                    {
                        string respone="Этот пользователь уже в сети\n";
                        send(client_socet,respone.c_str(),respone.size(),0);
                    }
                }
            }
            if(login)
            {
                string response="\nreg/log";
                send(client_socet,response.c_str(),response.size(),0);
            }
        }
    }
}
void podkluczenie(vector<ClientInfo>&client,int serverfd)
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
        std::string response = "reg/log\n";
        send(client_socket, response.c_str(), response.size(), 0);
        thread reg(handle_auth,client_socket,ref(client));
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
    vector<ClientInfo>clients;
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