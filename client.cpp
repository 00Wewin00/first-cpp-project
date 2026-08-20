#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>

using namespace std;

// Функция для приема сообщений в фоновом потоке
void pryjom_msg(int sock) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes <= 0) {
            cout << "\n[Система]: Соединение с сервером потеряно.\n";
            break;
        }
        
        // Выводим то, что пришло от других клиентов через сервер
        cout << "\n" << buffer << flush;
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Ошибка подключения к серверу!\n";
        return 1;
    }
    cout << "Connected to the server!\n";

    // Запускаем поток для приема сообщений
    thread recv_thread(pryjom_msg, sock);
    recv_thread.detach();

    // Основной цикл отправки сообщений
    while (true) {
        cout << "Ты: ";
        string message;
        getline(cin, message);

        // Защита от пустой строки (чтобы программа не падала при нажатии Enter)
        if (message.empty()) {
            continue;
        }

        // Обработка команд (начинающихся с '/')
        if (message[0] == 'w') {
            if (message == "/exit") {
                cout << "Выход из чата...\n";
                close(sock);
                break;
            } else {
                cout << "Неизвестная команда. Введите /exit для выхода.\n";
            }
        } 
        else {
            // Обычное сообщение — отправляем всем через сервер
            send(sock, message.c_str(), message.length(), 0);
        }
    }

    close(sock);
    return 0;
}