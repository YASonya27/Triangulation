/**
 * @file server.cpp
 * @brief Сервер для обработки триангуляции данных, полученных от клиента
 * @details 
 * Сервер принимает от клиента количество точек, генерирует случайные точки,
 * выполняет их триангуляцию и отправляет результат обратно клиенту.
 * Использует Winsock для сетевого взаимодействия.
 */

 #include "triang.h"
 #include <iostream>
 #include <winsock2.h>
 #include <ws2tcpip.h>
 
 #pragma comment(lib, "ws2_32.lib") ///< Автоматическая линковка библиотеки Winsock
 
 /**
  * @brief Основная функция сервера
  * @return 0 при успешном завершении
  * @note Последовательность работы:
  * 1. Инициализация Winsock
  * 2. Создание и настройка сокета
  * 3. Ожидание подключения клиента
  * 4. Получение данных от клиента и обработка
  * 5. Завершение работы
  */
 int main() {
     WSADATA wsaData;                     ///< Структура для инициализации Winsock
     SOCKET serverSocket, clientSocket;   ///< Дескрипторы сокетов
     sockaddr_in serverAddr, clientAddr;   ///< Структуры адресов
     int addrLen = sizeof(clientAddr);     ///< Размер структуры адреса клиента
 
     // Инициализация Winsock
     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
         std::cerr << "WSAStartup failed" << std::endl;
         return 1;
     }
 
     // Создание сокета
     serverSocket = socket(AF_INET, SOCK_STREAM, 0);
     if (serverSocket == INVALID_SOCKET) {
         std::cerr << "Socket creation failed" << std::endl;
         WSACleanup();
         return 1;
     }
 
     // Настройка адреса сервера
     serverAddr.sin_family = AF_INET;
     serverAddr.sin_addr.s_addr = INADDR_ANY; ///< Принимаем соединения на все интерфейсы
     serverAddr.sin_port = htons(8080);       ///< Порт 8080
 
     // Привязка сокета к адресу
     if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
         std::cerr << "Bind failed" << std::endl;
         closesocket(serverSocket);
         WSACleanup();
         return 1;
     }
 
     // Ожидание подключений
     if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
         std::cerr << "Listen failed" << std::endl;
         closesocket(serverSocket);
         WSACleanup();
         return 1;
     }
 
     std::cout << "Server started. Waiting for client connection..." << std::endl;
 
     // Принятие подключения клиента
     clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);
     if (clientSocket == INVALID_SOCKET) {
         std::cerr << "Accept failed" << std::endl;
         closesocket(serverSocket);
         WSACleanup();
         return 1;
     }
     std::cout << "The client is connected." << std::endl;
 
     // Основной цикл обработки данных
     while (true) {
         int result;
         int number;
 
         // Получение количества точек от клиента
         if (recv(clientSocket, (char*)&number, sizeof(number), 0) <= 0) {
             std::cerr << "Receive failed" << std::endl;
             break;
         }
 
         if (number == -1) break; ///< Код завершения работы
 
         // Генерация случайных точек
         Point a;
         std::vector<Point> points;
         for (int i = 0; i < number; i++) {
             a = {
                 1.0 + 4.0 * rand() / (float)RAND_MAX, ///< X-координата (1.0..5.0)
                 1.0 + 4.0 * rand() / (float)RAND_MAX  ///< Y-координата (1.0..5.0)
             };
             points.push_back(a);
         }
 
         // Выполнение триангуляции
         std::set<Edge> edges = triangulate(points);
         writeEdgesToFile(edges); ///< Запись рёбер в файл
         result = 1;              ///< Код успешного выполнения
 
         // Отправка результата клиенту
         if (send(clientSocket, (char*)&result, sizeof(result), 0) == SOCKET_ERROR) {
             std::cerr << "Send failed" << std::endl;
             break;
         }
     }
 
     // Завершение работы
     closesocket(clientSocket);
     closesocket(serverSocket);
     WSACleanup();
     
     return 0;
 }