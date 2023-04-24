#include "Logger.h"

Logger::Logger()
{
    ofd.open("log.txt", std::ios::app);
    if (ofd.is_open() == 0)
    {
        system("touch log.txt");
        system("chmod 700 log.txt");
    }
}

Logger::~Logger()
{
    ofd.close();
}

void Logger::writeLine(int num, std::string & str1, std::string & str2)
{
    mutex.lock();
    switch (num)
    {
        case 0:
        {
            ofd << "Попытка регистрации по существующему логину " << str1 << std::endl;
            break;
        } 
        case 1:
        {
            ofd << "Пользователь " << str1 << " зарегестрирован" << std::endl;
            break;
        }
        case 2:
        {
            ofd << "Неверные данные при авторизации логин: " << str1 << " пароль: " << str2 << std::endl; 
            break;
        }
        case 3:
        {
            ofd << "Пользователь " << str1 << " авторизовался" << std::endl;
            break;
        }
        case 4:
        {
            ofd << "Клиент отключился, завешение работы сервера" << std::endl;
            break;
        }
        case 5:
        {
            ofd << "Отправка списка пользователей клиенту" << std::endl;
            break;
        }
        case 6:
        {
            ofd << "Отправка списка чатов клиенту" << std::endl;
            break;
        }
        case 7:
        {
            ofd << "Пользователь " << str1 << " создал чат с индексом " << str2 << std::endl;
            break;
        }
        case 8:
        {
            ofd << "В чат " << str1 << " добавлено сообщение: " << str2 << std::endl;
            break;
        }
        case 9:
        {
            ofd << "Запрос истории сообщений чата: " << str1 << std::endl;
            break;
        }
    }
    mutex.unlock();
}

void Logger::showLogs()
{
    mutex.lock_shared();
    ifd.open("log.txt");
    std::string line;
    while (true)
    {
        readLine(line);
        if (line.size() == 0)
        {
            break;
        }
        std::cout << line << std::endl;
    }
    ifd.close();
    mutex.unlock();
}

void Logger::readLine(std::string & line)
{
    std::getline(ifd, line, '\n');
}