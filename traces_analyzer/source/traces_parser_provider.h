#pragma once

#include "helpers/string.h"

#include "tinyxml/tinyxml.h"

#include <vector>
#include <regex>
#include <chrono>

enum TraceTemplateParam
{
    Hour = 0,
    Minute,
    Second,
    MSecond,
    Thread,
    Level,
    Component
};

enum TraceLevel
{
    WRN = 0,
    DBG,
    INF,
    ERR,
    ALW,
    IMP
};

struct TraceTime
{
    TraceTime() :
        hour(0),
        minute(0),
        second(0),
        msecond(0)
    {}

    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t msecond;
};

struct TraceDescription
{
    TraceDescription() :
        threadId(0),
        level(WRN)
    {}

    TraceTime   time;
    uint32_t    threadId;
    TraceLevel  level;
    std::string component;
};

struct TraceTemplateValue
{
    TraceTemplateValue()
    {};

    TraceTemplateValue(std::string const& regexValue, std::vector<TraceTemplateParam> const& paramsValue) :
        regex(regexValue),
        params(paramsValue)
    {};

	std::string regex;
    std::vector<TraceTemplateParam> params;
};

class TracesParser
{
public:
    TracesParser(
		std::string const& traceTemplateName,
        TraceTemplateValue const& fullTemplate,
        TraceTemplateValue const& fastTemplate);

private:
    typedef std::basic_regex<std::string::value_type>   Regex;
    typedef std::pair<TraceTemplateValue, Regex>		Template;

public:
    bool Parse(
		std::string const& trace,
        TraceDescription& traceDescription,
		std::string::const_iterator& traceTextBegin,
        bool fullMode = true) const;

	std::string GetName() const;
    void GetTemplate(TraceTemplateValue& fullTemplate, TraceTemplateValue& fastTemplate) const;

private:
	std::string const   m_templateName;

    Template    m_fullTemplate;
    Template    m_fastTemplate;
};

class TracesParserProvider
{
public:
    bool                        Create(tstring const& tracesTemplatesPath);
    size_t                      GetCountParsers() const;
    TracesParser                GetParser(size_t i) const;

private:
    bool Load();
    bool LoadTemplate(TiXmlElement* templateNode, TraceTemplateValue& templateValue) const;
    bool Save() const;
    void SaveTemplate(TiXmlElement* templateNode, TraceTemplateValue const& templateValue) const;
    
private:
    tstring                     m_tracesTemplatesFilePath;
    std::vector<TracesParser>   m_parsers;
};

