#pragma once

#include <string>
#include <iostream>

class Compress
{
    static constexpr auto defaultWinRarPath = R"("C:\Program Files\WinRar\Rar.exe")";
    static constexpr auto defaultArchivePath = R"(E:\Archive\)";
public:
    static void rar(std::string folderName);
};

void Compress::rar(std::string folderName)
{
    std::string fullPath = defaultArchivePath + folderName;

    std::string command = std::string("del") + " " + fullPath + ".rar";
    std::cout << "Executing: " << command << std::endl;
    
    auto code = std::system(command.data());

    command = std::string(defaultWinRarPath) + " a -r -m5 -df " + fullPath + ".rar " + fullPath;
    std::cout << "Executing: " << command << std::endl;

    code += std::system(command.data());

    if (code)
    {
        throw std::runtime_error{"Deleting or archivating has failed"};
    }
}