#pragma once

#include "helpers/string.h"

#include <vector>

class TracesParser
{
public:
    TracesParser(tstring const& traceTemplate);

public:
    void Parse(tstring const& string);

private:
    tstring const m_traceTemplate;
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

private:
    bool Load();
    void Save();
    
private:
    tstring                     m_tracesTemplatesFilePath;
    std::vector<TracesParser>   m_parsers;
};

