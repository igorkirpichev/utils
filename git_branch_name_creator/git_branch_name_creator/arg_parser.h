#pragma once
#include <string>
#include <map>
#include <sstream>

namespace arg_parser
{

using CmdArguments = std::map<std::wstring, std::wstring>;

inline bool HasArgument(const CmdArguments& arguments, const std::wstring& arg)
{
    return arguments.count(arg) > 0;
}

inline std::wstring GetArgumentOrDefault(const CmdArguments& arguments, const std::wstring& arg)
{
    const auto itFind = arguments.find(arg);
    if (itFind != arguments.end())
    {
        return itFind->second;
    }
    return {};
}

inline bool IsExplicitArgument(const std::wstring& argv)
{
    const std::wstring matchPrefix = L"--";
    return argv.size() > matchPrefix.size() && argv.find_first_of(matchPrefix) == 0;
}

inline CmdArguments ExtractExplicitArguments(int argc, wchar_t* argv[], int& position)
{
    CmdArguments result;
    for (position = 1; (position < argc) && IsExplicitArgument(argv[position]); ++position)
    {
        std::wstring arg(argv[position]);
        std::wstring argValue;

        // get value if exists
        if ((position + 1) < argc)
        {
            std::wstring nextArg(argv[position + 1]);
            if (!IsExplicitArgument(nextArg))
            {
                argValue = nextArg;
                position++;
            }
        }

        result[arg] = argValue;
    }

    return result;
}

inline std::wstring ExtractRestArguments(int argc, wchar_t* argv[], int sincePosition = 1)
{
    std::wstringstream result;
    for (int position = sincePosition; position < argc; position++)
    {
        result << argv[position] << " ";
    }
    return result.str();
}

} // arg_parser
