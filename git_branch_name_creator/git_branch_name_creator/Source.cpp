#include <iostream>
#include <string>
#include <cctype>
#include <Windows.h>
#include <shlwapi.h>

#define COMAND_ARG_FROM_CLIP        L"-from_clip"
#define COMAND_ARG_TO_CLIP          L"-to_clip"
#define COMAND_ARG_BRANCH_PREFIX    L"-prefix"

#define SEPARATOR_CHAR '_'

std::wstring const rus = L"ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÝÞßàáâãäå¸æçèéêëìíîïðñòóôõö÷øùüûúýþÿ";
std::wstring const lat[] = { L"A", L"B", L"V", L"G", L"D", L"E", L"E", L"Zh", L"Z", L"I", L"I", L"K",
    L"L", L"M", L"N", L"O", L"P", L"R", L"S", L"T", L"U", L"F", L"H", L"C", L"Ch", L"Sh", L"Sh'", L"E",
    L"Yu", L"Ya", L"a", L"b", L"v", L"g", L"d", L"e", L"e", L"zh", L"z", L"i", L"i", L"k", L"l", L"m", L"n",
    L"o", L"p", L"r", L"s", L"t", L"u", L"f", L"h", L"c", L"ch", L"sh", L"sh'", L"", L"i", L"", L"e", L"yu",
    L"ya" };

void SetSeparator(std::wstring& str)
{
    if (!str.empty() && (str.back() != SEPARATOR_CHAR))
        str.push_back(SEPARATOR_CHAR);
}

void RemoveSeparator(std::wstring& str)
{
    if (!str.empty() && (str.back() == SEPARATOR_CHAR))
        str.erase(--str.end());
}

std::wstring CyrillicTranslit(const std::wstring& str)
{
    std::wstring translitSource;

    for (wchar_t ch : str)
    {
        size_t position = rus.find(ch);
        if (position != std::string::npos)
            translitSource += lat[position];
        else
            translitSource += ch;
    }

    return translitSource;
}

std::wstring TransformString(const std::wstring& str)
{
    std::wstring destinationStr;

    std::wstring sourceStr(CyrillicTranslit(str));

    wchar_t prevCh = 0;
    for (wchar_t ch : sourceStr)
    {
        if (std::isalpha(ch))
        {
            if (prevCh && std::islower(prevCh) && std::isupper(ch))
                SetSeparator(destinationStr);

            destinationStr.push_back(std::tolower(ch));
        }
        else if (std::isdigit(ch))
        {
            destinationStr.push_back(ch);
        }
        else
        {
            SetSeparator(destinationStr);
        }

        prevCh = ch;
    }

    return destinationStr;
}

bool IsSpecialParameter(const std::wstring& argv)
{
    return argv.front() == '-';
}

void SetClipboard(const std::wstring& str)
{
    if (::OpenClipboard(NULL))
    {
        ::EmptyClipboard();
        size_t const size = (str.size() + 1) * sizeof(wchar_t);
        HGLOBAL hClipboardData = ::GlobalAlloc(NULL, size);
        if (hClipboardData)
        {
            wchar_t* pchData = (wchar_t*)::GlobalLock(hClipboardData);
            if (pchData)
            {
                memcpy(pchData, (wchar_t*)str.data(), size);
                ::GlobalUnlock(hClipboardData);
                ::SetClipboardData(CF_UNICODETEXT, hClipboardData);
            }
        }
        ::CloseClipboard();
    }
}

std::wstring GetClipboard()
{
    std::wstring text;

    if (::OpenClipboard(NULL))
    {
        HANDLE hData = ::GetClipboardData(CF_UNICODETEXT);
        if (hData)
        {
            wchar_t* pszText = static_cast<wchar_t*>(::GlobalLock(hData));
            if (pszText)
            {
                text = std::wstring(pszText);
                ::GlobalUnlock(hData);
            }
        }

        ::CloseClipboard();
    }

    return text;
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
    if (argc <= 1)
    {
        std::wstring commandName(L"git_branch_name_creator.exe");
        if (argc == 1)
            commandName = ::PathFindFileNameW(argv[0]);

        std::wcout << L"Usage: " << commandName <<L" [" << 
            COMAND_ARG_FROM_CLIP << L"][" << 
            COMAND_ARG_TO_CLIP << L"][" << 
            COMAND_ARG_BRANCH_PREFIX << L" team/developer] <title string>" << std::endl;
        std::wcout << '\t' << COMAND_ARG_FROM_CLIP      << L"\tSource string is taken from clipboard. <title string> is optional" << std::endl;
        std::wcout << '\t' << COMAND_ARG_TO_CLIP        << L"\tThe result of the conversion is copied to the clipboard" << std::endl;
        std::wcout << '\t' << COMAND_ARG_BRANCH_PREFIX  << L"\t\tTeam and developer name will be added to the beginning of the branch name" << std::endl;

        return 0;
    }

    bool sourceTitleFromClipboard       = false;
    bool destinationTitleToClipboard    = false;
    std::wstring branchPrefix;

    int position;
    for (position = 1; (position < argc) && IsSpecialParameter(argv[position]); ++position)
    {
        std::wstring arg(argv[position]);

        if (arg.compare(COMAND_ARG_FROM_CLIP) == 0)
            sourceTitleFromClipboard = true;
        else if (arg.compare(COMAND_ARG_TO_CLIP) == 0)
            destinationTitleToClipboard = true;
        else if (arg.compare(COMAND_ARG_BRANCH_PREFIX) == 0)
        {
            if ((position + 1) < argc)
            {
                position++;
            }
            else
            {
                std::cout << "No prefix specified" << std::endl;
                return 0;
            }

            branchPrefix = argv[position];
        }
        else
            break;
    }

    std::wstring destinationTitle;

    if (sourceTitleFromClipboard)
    {
        destinationTitle = TransformString(GetClipboard());

        if (destinationTitle.empty())
        {
            std::cout << "Clipboard is empty" << std::endl;
            return 0;
        }
    }
    else
    {
        if (position == argc)
        {
            std::cout << "No title string specified" << std::endl;
            return 0;
        }

        for (; position < argc; position++)
        {
            destinationTitle += TransformString(argv[position]);
            SetSeparator(destinationTitle);
        }

        RemoveSeparator(destinationTitle);
    }

    std::wstring const changeRequestString(L"change_request");
    if (destinationTitle.find(changeRequestString) == 0)
    {
        destinationTitle.replace(0, changeRequestString.size(), L"cr");
    }

    if (!branchPrefix.empty())
        destinationTitle = branchPrefix + L"/" + destinationTitle;

    std::wcout << destinationTitle << std::endl;

    if (destinationTitleToClipboard)
    {
        SetClipboard(destinationTitle);
        std::cout << "Copied to clipboard" << std::endl;
    }

    return 1;
}
