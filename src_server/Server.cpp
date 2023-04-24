#include "Server.h"

Server::Server()
{
    length = sizeof(clientAddress);
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_family = AF_INET;
    int listenStatus, bindStatus;
    fdServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdServerSocket == -1)
    {
        std::cout << "Сокет не был создан!" << std::endl;
        exit(1);
    }
    bindStatus = bind(fdServerSocket,  (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (bindStatus == -1)
    {
        std::cout << "Сокет не был привязан!" << std::endl;
        exit(1);
    }
    listenStatus = listen(fdServerSocket, 1);
    if (listenStatus == -1)
    {
        std::cout << "Сокет не слушает порт!" << std::endl;
        exit(1);
    }
    fdClientSocket = accept(fdServerSocket, (struct sockaddr*)&clientAddress, &length);
    close(fdServerSocket);
    sckt = new Socket(fdClientSocket);
}

void Server::listenPort()
{
    flag = true;
    while (flag)
    {
        sckt->getMsg();
        int instruction = sckt->getInstruction(); 
        runInstruction(instruction);  
    }
    close(fdClientSocket);
    delete sckt;
}

void Server::runInstruction(int instruction)
{
    switch (instruction)
    {
        case 1:
        {
            //Выполнить регистрацию
            registration();
            break;
        }
        case 2:
        {
            //Выполнить авторизацию
            authorization();
            break;
        }
        case 3:
        {
            //Завершить работу
            flag = false;
            std::string end = "4";
            logger.writeLine(4, end, end);
            break;
        }
        case 4:
        {
            //послать список пользователей
            sendUsersList();
            std::string end = "5";
            logger.writeLine(5, end, end);
            break;
        }
        case 5:
        {
            //послать список чатов пользователя
            sendChatsList();
            std::string end = "6";
            logger.writeLine(6, end, end);
            break;
        }
        case 6:
        {
            //добавить новый чат в базу
            addNewChat();
            break;
        }
        case 7:
        {
            //добавить новое сообщение в базу
            addNewMessage();
            break;
        }
        case 8:
        {
            //послать сообщения чата
            sendMessagesList();
            break;
        }
    }
}

void Server::registration()
{
    int num;
    std::string login, password, msg;
    sckt->getString(login);
    sckt->getString(password);
    if (db.addUser(login, password) == false)
    {
        msg += "0\n";
        num = 0;
    }
    else
    {
        msg += "1\n";
        num = 1;
    }
    sckt->sendMsg(msg);
    logger.writeLine(num, login, password);
}

void Server::authorization()
{
    std::string login, password, msg;
    sckt->getString(login);
    sckt->getString(password);
    int num;
    if (db.checkUser(login, password, 'a') == true)
    {
        activeUser = login;
        msg += "3\n" + login + '\n';
        sckt->sendMsg(msg);
        num = 3;
    }
    else
    {
        msg += "2\n";
        sckt->sendMsg(msg);
        num = 2;
    }
    logger.writeLine(num, login, password);
}

void Server::sendUsersList()
{
    std::list<std::string> userList;
    db.getUsersList(userList);
    std::string endPack = "-------end-------\n";
    std::list<std::string>::iterator begin = userList.begin();
    std::list<std::string>::iterator end = userList.end();
    std::string pack;
    while (begin != end)
    {
        pack += *begin + '\n';
        begin++;
        sckt->sendMsg(pack);
    }
    sckt->sendMsg(endPack);
}

void Server::sendChatsList()
{
    std::list<std::string> chatsList;
    db.getUserChats(chatsList, activeUser);
    std::string pacEnd = "-------end-------\n";
    std::list<std::string>::iterator begin = chatsList.begin();
    std::list<std::string>::iterator end = chatsList.end();
    std::string package;
    size_t count = 0;
    while (begin != end)
    {
        package += *begin + '\n';
        begin++;
        count++;
        if (count == 3)
        {
            count = 0;
            sckt->sendMsg(package);
        }
    }
    sckt->sendMsg(pacEnd);
}

void Server::addNewChat()
{
    std::string user1, user2, c_id;
    sckt->getString(user1);
    sckt->getString(user2);
    db.addChat(user1, user2, c_id);
    c_id += '\n';
    sckt->sendMsg(c_id);
    logger.writeLine(7, user1, c_id);
}

void Server::addNewMessage()
{
    std::string c_id, author, message;
    sckt->getString(author);
    sckt->getString(message);
    sckt->getString(c_id);
    db.addMessage(c_id, author, message);
    logger.writeLine(8, c_id, message);
}

void Server::sendMessagesList()
{
    std::string pacEnd = "-------end-------\n";
    std::string c_id;
    sckt->getString(c_id);
    std::list<std::string> messagesList;
    db.getChatMessages(c_id, messagesList);
    logger.writeLine(9, c_id, c_id);
    std::list<std::string>::iterator begin = messagesList.begin();
    std::list<std::string>::iterator end = messagesList.end();
    std::string package;
    size_t count = 0;
    while (begin != end)
    {
        package += *begin + '\n';
        begin++;
        count++;
        if (count == 2)
        {
            count = 0;
            sckt->sendMsg(package);
        }
    }
    sckt->sendMsg(pacEnd);
}

void Server::administrator()
{  
    std::string choice;
    while (true)
    {
        std::cout << "1 - Показать логи" << std::endl;
        std::cout << "0 - Завершить работу" << std::endl;
        std::cout << "Ввод: ";
        std::cin >> choice;
        if (choice == "1")
        {
            logger.showLogs();
        }
        else if (choice == "0")
        {
            break;
        }
    }
}   