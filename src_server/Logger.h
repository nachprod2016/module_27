#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <shared_mutex>

class Logger
{
private:
    std::ofstream ofd;
    std::ifstream ifd;
    std::shared_mutex mutex;
public:
    Logger();
    ~Logger();
    void writeLine(int, std::string &, std::string &);
    void readLine(std::string &);
    void showLogs();
};