/*

============ Source file encoding: Windows 1251 (Cyrillic) ============

*/

#include <iostream>
#include <locale>
#include "PathsParser.hpp"
#include "CopyAccomplisher.hpp"
#include "Compress.hpp"

void bye()
{
    std::wcout << L"Для выхода нажмите Enter";
    std::cin.get();
}

int main(int argc, char* argv[])
{
    
    std::locale::global(std::locale("ru_RU.UTF-8"));

    CopyAccomplisher c{L"backups.txt"};
    try
    {
        auto weekday = c.accomplish();

        if (weekday.empty())
        {
            bye();
            return EXIT_FAILURE;
        }
        std::wcout << L"Копирование выполнено, выполняется архивация...\n";
        
        Compress::rar(weekday);
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch(...)
    {
        std::cout << "Unknown error  |-_-|";
        bye();
        return EXIT_FAILURE;
    }
    
    
    bye();
    return EXIT_SUCCESS;
}
