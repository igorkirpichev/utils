#pragma once

#include "helpers/string.h"

#include <vector>
#include <regex>


class TracesParser
{
public:
    TracesParser(tstring const& traceTemplateName, tstring const& traceTemplate);

public:
    bool Parse(tstring const& trace);

	tstring GetName() const;
	tstring GetTemplate() const;

private:
	tstring const                               m_templateName;

    tstring const                               m_fullTemplate;
    std::basic_regex<tstring::value_type> const m_fullRegex;
};

class TracesParserProvider
{
private:
    TracesParserProvider();
    ~TracesParserProvider();

public:
    static TracesParserProvider& GetInstance();

public:
    void Create(tstring const& tracesTemplatesPath);
    size_t GetCountParsers() const;
    TracesParser& GetParser(size_t i);

private:
    bool Load();
    void Save() const;
    
private:
    tstring                     m_tracesTemplatesFilePath;
    std::vector<TracesParser>   m_parsers;
};

