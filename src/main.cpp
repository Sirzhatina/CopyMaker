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
    std::cout << "Press Enter to quit";
    std::cin.get();
}

#define EXIT_WITH_FAILURE bye(); return EXIT_FAILURE;

int main(int argc, char* argv[])
{
    
    std::locale::global(std::locale("ru_RU.UTF-8"));

    CopyAccomplisher c{"backups.txt"};
    try
    {
        auto weekday = c.accomplish();

        if (weekday.empty())
        {
            bye();
            return EXIT_FAILURE;
        }
        std::cout << "Archivation...\n";
        
        Compress::rar(weekday);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        EXIT_WITH_FAILURE
    }
    catch(...)
    {
        std::cout << "Unknown error  |-_-|";
        EXIT_WITH_FAILURE
    }
    
    return EXIT_SUCCESS;
}
