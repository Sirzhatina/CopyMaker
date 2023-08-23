/*

============ Source file encoding: Windows 1251 (Cyrillic) ============

*/

#pragma once

#include <map>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <type_traits>

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


    NativeMessage(string what = ""): std::runtime_error{std::is_same_v<string, std::string> ? what : ""}, m_what(std::move(what)) { }

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
            errorMsg = {"The backups.txt file must exist in the same directory as the program is,\n"
                         "and must contain paths to copy from and to. Current path: " + fsys::current_path().string()};
        }
        else
        {
            errorMsg = {"Failed opening file. Make sure it exists and is in the same directory as the program is."};
        } 
        return errorMsg;
    }

    std::pair success = m_result.insert(std::move(getLineFromPaths(f)));
    while (!f.eof())
    {
        if (!success.second)
        {
            return NativeMessage<CharT>{"Failed adding a path"};
        }
        success = m_result.insert(std::move(getLineFromPaths(f)));
    }
    if (f.fail())
    {
        return NativeMessage<CharT>{"Unexpected failure when reading the file. Make sure the file is in the format "
                                "\n\"path_from > path_to\" (two paths, with angle bracket in the middle, whitespaces around angle brackets don't matter)"
                                "\n... (the same as above, optionally)"
                                "\nIf the error continues to occur, that means the developer didn't debug the software good enough.\nWell, I'm so sorry \t"
                                R"(\_("-")_/)"};
    }

    return errorMsg;    // ok if it's empty
}


template <class CharT>
std::pair<fsys::path, fsys::path> PathsParser<CharT>::getLineFromPaths(ifstream& f)
{
    string from, to;

    std::getline(f, from, '>');
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
