/*

============ Source file encoding: Windows 1251 (Cyrillic) ============

*/
#pragma once

#include "PathsParser.hpp"
#include <chrono>
#include <stdexcept>

template <class CharT>
class CopyAccomplisher
{
public:
    CopyAccomplisher(const CharT* filenameWithPaths): m_parser{filenameWithPaths} {}

    std::string accomplish();

private:
    void copyOne(const fsys::path& from, const fsys::path& to);
    /// @brief adds a folder named as a week day in the path
    std::string addDayFolder(fsys::path& path);

    PathsParser<CharT> m_parser;
};

template <class CharT>
std::string CopyAccomplisher<CharT>::accomplish()
{
    auto msg = m_parser.readPathsFromFile().nativeWhat();
    std::string weekDay;
    if (!msg.empty())
    {
        std::wcout << msg << std::endl;
        return {};
    }
    
    for (auto&& [from, to] : m_parser.getResult())
    {
        weekDay = addDayFolder(to);
        copyOne(from, to);
    }
    return weekDay;
}

template <class CharT>
std::string CopyAccomplisher<CharT>::addDayFolder(fsys::path& path)
{
    using namespace std::chrono;
    auto wDay = weekday{time_point_cast<days>(system_clock::now())};

    auto weekDay = std::format("{:%A}", wDay);

    path.replace_filename(weekDay/path.filename());
    return weekDay;
}

template <class CharT>
void CopyAccomplisher<CharT>::copyOne(const fsys::path& from, const fsys::path& to)
{
    std::error_code ec;

    std::wcout << L"Выполняется копирование из " << from << L" в " << to << "...\n";

    if (!fsys::exists(to))
    {
        if (!fsys::create_directories(to))
        {
            throw std::runtime_error{"Failed creating directory.\n"
                                     "It still does not exist."};
        }
    }
    else if (!fsys::remove_all(to))
    {
        throw std::runtime_error{"Unable to remove dir before copying. Remove it manually."};
    }
    else
    {
        std::cout << "Old dir has been removed successfully.";
    }
    std::endl(std::cout);
    fsys::copy(from, to, fsys::copy_options::overwrite_existing |
                         fsys::copy_options::recursive, ec);
    
    if (ec)
    {
        std::cout << "Error value: " << ec.value()
                    << "\nMessage: "  << ec.message();
    }
    else
    {
        std::cout << "The copy has been completed.\n";
    }
}