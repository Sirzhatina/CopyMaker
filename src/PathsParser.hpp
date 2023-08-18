/*

============ Source file encoding: Windows 1251 (Cyrillic) ============

*/

#pragma once

#include <map>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>


namespace 
{
    namespace fsys = std::filesystem;
}

// user-friendly message class that can be used as an exception as well with different char types
template <class CharT>
class NativeMessage : public std::runtime_error
{
public:
    using string = std::basic_string<CharT>;


    NativeMessage(string what): std::runtime_error{""}, m_what(std::move(what)) { }
    NativeMessage(std::string what = ""): std::runtime_error{what} {}

    const string& nativeWhat() const { return m_what; }

private:
    string m_what;
};

/*
Grammar for parsing the file:
    paths:
        line '\n'
        paths
    line:
        path_from [' '...] '>' [' '...] path_to
*/

template <class CharT>
class PathsParser
{
public:
    using string_view = std::basic_string_view<CharT>;
    using ifstream    = std::basic_ifstream<CharT>;
    using string      = std::basic_string<CharT>;

public:
    PathsParser(const CharT* filename): m_filename(filename) {}
    PathsParser(string_view filename):  m_filename(filename) {}
    // main method of the class
    NativeMessage<CharT> readPathsFromFile();

    [[nodiscard]] auto getResult() const noexcept { return m_result; }

    void resetResult() noexcept { m_result.clear(); }

private:
    const string m_filename;

    std::map<fsys::path, fsys::path> m_result;

    std::pair<fsys::path, fsys::path> getLineFromPaths(ifstream& f);
};


// ============== Implementation =============

template <class CharT>
NativeMessage<CharT> PathsParser<CharT>::readPathsFromFile()
{
    ifstream f(string{m_filename.begin(), m_filename.end()});

    NativeMessage<CharT> errorMsg;
    if (!f)
    {
        if (!fsys::exists(fsys::current_path()/m_filename))
        {
            errorMsg = {L"Файл backups.txt должен существовать в той же директории, что и программа,\n"
                         "и должен содержать пути для копирования. Текущий путь: " + fsys::current_path().wstring()};
        }
        else
        {
            errorMsg = {L"Не удалось открыть файл. Убедитесь, что он существует и "
                         "находится в той же директории, что и программа."};
        } 
        return errorMsg;
    }

    std::pair success = m_result.insert(std::move(getLineFromPaths(f)));
    while (!f.eof())
    {
        if (!success.second)
        {
            return NativeMessage<CharT>{L"Не удалось добавить путь"};
        }
        success = m_result.insert(std::move(getLineFromPaths(f)));
    }
    if (f.fail())
    {
        return NativeMessage<CharT>{L"Непредвиденный сбой при чтении файла. Убедитесь, что файл заполнен в формате "
                                "\n\"путь_из > путь_в\" (два пути, посередине правая угловая скобка, по типу стрелочки, число пробелов вокруг скобки не имеет значения)"
                                "\n... (то же, что и сверху, опционально)"
                                "\nЕсли ошибка продолжает появляться, значит разработчик не удосужился качественно отладить софт.\nСорямба \t"
                                LR"(\_("-")_/)"};
    }

    return errorMsg;    // ok if it's empty
}


template <class CharT>
std::pair<fsys::path, fsys::path> PathsParser<CharT>::getLineFromPaths(ifstream& f)
{
    string from, to;

    std::getline(f, from, L'>');
    // unexpected eof-state leads to passing the fail-state
    if (f.eof())
    {
        f.setstate(std::ios::failbit);
        return {};
    }
    // here, eof is allowed
    std::getline(f, to);

    // trimming side whitespaces
    from.erase(from.find_last_not_of(' ') + 1);
    to.erase(0, to.find_first_not_of(' '));

    return {std::move(from), std::move(to)};
}
