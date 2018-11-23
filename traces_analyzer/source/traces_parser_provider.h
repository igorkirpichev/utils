#pragma once

#include "helpers/string.h"

#include "tinyxml/tinyxml.h"

#include <vector>
#include <regex>

enum TraceTemplateParam
{
    Hour = 0,
    Minute,
    Second,
    MSecond,
    Thread,
    TraceLevel,
    Component
};

struct TraceTemplateValue
{
    TraceTemplateValue()
    {};

    TraceTemplateValue(tstring const& regexValue, std::vector<TraceTemplateParam> const& paramsValue) :
        regex(regexValue),
        params(paramsValue)
    {};

    tstring regex;
    std::vector<TraceTemplateParam> params;
};

class TracesParser
{
public:
    TracesParser(
        tstring const& traceTemplateName,
        TraceTemplateValue const& fullTemplate,
        TraceTemplateValue const& fastTemplate);

private:
    typedef std::basic_regex<tstring::value_type>   Regex;
    typedef std::pair<TraceTemplateValue, Regex>    Template;

public:
    bool Parse(tstring const& trace, bool fullMode = true) const;

	tstring GetName() const;
    void GetTemplate(TraceTemplateValue& fullTemplate, TraceTemplateValue& fastTemplate) const;

private:
	tstring const   m_templateName;

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

