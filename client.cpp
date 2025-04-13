/**
 * @file client.cpp
 * @brief Клиент для взаимодействия с сервером триангуляции
 * @details
 * Клиент подключается к серверу, отправляет количество точек для триангуляции
 * и получает подтверждение обработки. Использует Winsock для сетевого взаимодействия.
 */

 #include <iostream>
 #include <winsock2.h>       ///< Основной заголовочный файл Winsock2
 #include <ws2tcpip.h>       ///< Дополнительные функции для работы с TCP/IP
 
 #pragma comment(lib, "ws2_32.lib") ///< Автоматическая линковка с библиотекой Winsock
 
 /**
  * @brief Основная функция клиента
  * @return 0 при успешном завершении, 1 при ошибке подключения
  * @note Алгоритм работы:
  * 1. Инициализация Winsock
  * 2. Создание и настройка сокета
  * 3. Подключение к серверу
  * 4. Ввод данных и взаимодействие с сервером
  * 5. Завершение работы
  */
 int main() {
	 // Инициализация сетевых структур
	 WSADATA wsaData;            ///< Структура для инициализации Winsock
	 SOCKET clientSocket;        ///< Дескриптор клиентского сокета
	 sockaddr_in serverAddr;     ///< Структура адреса сервера
 
	 // Инициализация Winsock (версия 2.2)
	 if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		 std::cerr << "WSAStartup failed" << std::endl;
		 return 1;
	 }
 
	 // Создание сокета (IPv4, TCP)
	 clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	 if (clientSocket == INVALID_SOCKET) {
		 std::cerr << "Socket creation failed" << std::endl;
		 WSACleanup();
		 return 1;
	 }
 
	 // Настройка адреса сервера
	 serverAddr.sin_family = AF_INET;        ///< Семейство адресов IPv4
	 serverAddr.sin_port = htons(8080);      ///< Порт сервера 8080
 
	 // Преобразование IP-адреса (localhost)
	 if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
		 std::cerr << "Invalid address" << std::endl;
		 closesocket(clientSocket);
		 WSACleanup();
		 return 1;
	 }
 
	 // Подключение к серверу
	 if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		 std::cerr << "Connection failed" << std::endl;
		 closesocket(clientSocket);
		 WSACleanup();
		 return 1;
	 }
 
	 std::cout << "Connected to server. Enter numbers to process (-1 to exit)." << std::endl;
 
	 // Основной цикл взаимодействия
	 while (true) {
		 int number;
		 std::cout << "Enter a number of points (or -1 to exit): ";
		 std::cin >> number;
 
		 // Отправка числа серверу
		 if (send(clientSocket, (char*)&number, sizeof(number), 0) == SOCKET_ERROR) {
			 std::cerr << "Send failed" << std::endl;
			 break;
		 }
 
		 // Выход по команде
		 if (number == -1) {
			 std::cout << "Terminating connection..." << std::endl;
			 break;
		 }
 
		 // Получение результата от сервера
		 int result;
		 if (recv(clientSocket, (char*)&result, sizeof(result), 0) <= 0) {
			 std::cerr << "Receive failed" << std::endl;
			 break;
		 }
 
		 // Обработка ответа сервера
		 if (result == 1) {
			 std::cout << "Triangulation completed successfully." << std::endl;
		 } else {
			 std::cout << "Server returned error code: " << result << std::endl;
		 }
	 }
 
	 // Завершение работы
	 closesocket(clientSocket);
	 WSACleanup();
	 return 0;
 }