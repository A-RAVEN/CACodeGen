#pragma once
#include "pch.h"

namespace Utils
{
    static std::vector<std::string> split(std::string input, std::string pat)
    {
        std::vector<std::string> ret_list;
        while (true)
        {
            size_t      index    = input.find(pat);
            std::string sub_list = input.substr(0, index);
            if (!sub_list.empty())
            {
                ret_list.push_back(sub_list);
            }
            input.erase(0, index + pat.size());
            if (index == -1)
            {
                break;
            }
        }
        return ret_list;
    }

    static std::string getFileName(std::string path)
    {
        if (path.size() < 1)
        {
            return std::string();
        }
        std::vector<std::string> result = split(path, "/");
        if (result.size() < 1)
        {
            return std::string();
        }
        return result[result.size() - 1];
    }

    static std::string replace(std::string& source_string, std::string sub_string, const std::string new_string)
    {
        std::string::size_type pos = 0;
        while ((pos = source_string.find(sub_string)) != std::string::npos)
        {
            source_string.replace(pos, sub_string.length(), new_string);
        }
        return source_string;
    }

    static std::string replace(std::string& source_string, char taget_char, const char new_char)
    {
        std::replace(source_string.begin(), source_string.end(), taget_char, new_char);
        return source_string;
    }

    static std::string toUpper(std::string& source_string)
    {
        std::transform(source_string.begin(), source_string.end(), source_string.begin(), ::toupper);
        return source_string;
    }

    static std::string join(std::vector<std::string> context_list, std::string separator)
    {
        std::string ret_string;
        if (context_list.size() == 0)
        {
            return ret_string;
        }
        ret_string = context_list[0];
        for (int index = 1; index < context_list.size(); ++index)
        {
            ret_string += separator + context_list[index];
        }

        return ret_string;
    }

    static void toString(const CXString& str, std::string& output)
    {
        auto cstr = clang_getCString(str);

        output = cstr;

        clang_disposeString(str);
    }
}