#include <iostream>
#include <string>
#include <cctype>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")

#include "string_transform.h"
#include "clipboard.h"
#include "arg_parser.h"


namespace constants
{
const wchar_t* CommandArgFromClip     = L"--from_clip";
const wchar_t* CommandArgToClip       = L"--to_clip";
const wchar_t* CommandArgBranchPrefix = L"--prefix";
const wchar_t* CommandArgShort        = L"--short";
const wchar_t* CommandArgMedium       = L"--medium";

const int CountCharsMedium = 30;
} // constants


namespace
{

void ShowHelp(const std::wstring& commandName)
{
    std::wcout << L"Usage: " << commandName <<L" [" <<
        constants::CommandArgFromClip << L"][" <<
        constants::CommandArgToClip << L"][" <<
        constants::CommandArgBranchPrefix << L" team/developer] <title string>" << std::endl;
    std::wcout << '\t' << constants::CommandArgFromClip      << L"\tSource string is taken from clipboard. <title string> is optional" << std::endl;
    std::wcout << '\t' << constants::CommandArgToClip        << L"\tThe result of the conversion to be copied to the clipboard" << std::endl;
    std::wcout << '\t' << constants::CommandArgBranchPrefix  << L"\t\tTeam and developer name will be added to the beginning of the branch name" << std::endl;
}

}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
    if (argc <= 1)
    {
        ShowHelp(::PathFindFileNameW(argv[0]));
        return 2;
    }

    // parse input arguments
    int position = {};
    const auto explicitArgs = arg_parser::ExtractExplicitArguments(argc, argv, position);
    auto inputTitle = arg_parser::ExtractRestArguments(argc, argv, position);

    const bool destinationTitleToClipboard = arg_parser::HasArgument(explicitArgs, constants::CommandArgToClip);
    const bool shrinkShort = arg_parser::HasArgument(explicitArgs, constants::CommandArgShort);
    const bool shrinkMedium = arg_parser::HasArgument(explicitArgs, constants::CommandArgMedium);
    const std::wstring branchPrefix = arg_parser::GetArgumentOrDefault(explicitArgs, constants::CommandArgBranchPrefix);
    if (arg_parser::HasArgument(explicitArgs, constants::CommandArgBranchPrefix) && branchPrefix.empty())
    {
        std::cout << "No prefix specified" << std::endl;
        return 2;
    }
    if (arg_parser::HasArgument(explicitArgs, constants::CommandArgFromClip))
    {
        inputTitle = clipboard::GetClipboard();
    }
    if (inputTitle.empty())
    {
        std::cout << "No title string specified" << std::endl;
        return 0;
    }

    // process
    auto destinationTitle = string_transform::TransformString(inputTitle);
    if (shrinkShort)
    {
        if (string_transform::ShrinkShort(destinationTitle))
        {
            std::cout << "Shrinks to short" << std::endl;
        }
    }
    else if (shrinkMedium)
    {
        if (string_transform::ShrinkN(destinationTitle, constants::CountCharsMedium))
        {
            std::cout << "Shrinks to medium" << std::endl;
        }
    }
    string_transform::RemoveSeparator(destinationTitle);
    string_transform::ReplaceSubstring(destinationTitle, L"change_request", L"cr");

    if (!branchPrefix.empty())
    {
        destinationTitle = branchPrefix + L"/" + destinationTitle;
    }

    // output result
    if (destinationTitleToClipboard)
    {
        clipboard::SetClipboard(destinationTitle);
        std::cout << "Copied to clipboard" << std::endl;
    }
    else
    {
        std::wcout << destinationTitle << std::endl;
    }

    return 0;
}
