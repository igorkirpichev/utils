#include "traces_parser_provider.h"

#include "afxwin.h"

#include <filesystem>
#include <map>

#if !_HAS_CXX17
namespace std
{
    namespace filesystem = std::experimental::filesystem;
} // namespace std
#endif // !_HAS_CXX17

#define TRACES_TEMPLATES_FILE_NAME  TEXT("traces_templates.xml")

#define DEFAULT_TRACE_TEMPLATE_NAME         TEXT("KAV_KIS")
#define DEFAULT_TRACE_FULL_TEMPLATE         TEXT("^(\\d{2}):(\\d{2}):(\\d{2}).(\\d{3})\\t0x([\\da-fA-F]{1,4}?)\\t([A-Z]{3})\\t([\\w\\.]+?)\\t")
#define DEFAULT_TRACE_FULL_TEMPLATE_PARAMS  {Hour, Minute, Second, MSecond, Thread, Level, Component}
#define DEFAULT_TRACE_FAST_TEMPLATE         TEXT("^.+?\t0x([\\da-fA-F]{1,4}?)\\t.+?\\t.+?\\t")
#define DEFAULT_TRACE_FAST_TEMPLATE_PARAMS  {Thread}

// XML tags
#define XML_TAG_ROOT			    "traces_templates_root"
#define XML_TAG_TEMPLATE		    "template"
#define XML_TAG_TEMPLATE_FULL_MODE  "full_mode"
#define XML_TAG_TEMPLATE_FAST_MODE  "fast_mode"
#define XML_TAG_REGEX               "regex"
#define XML_TAG_PARAMS              "params"

// XML attributes
#define XML_ATTR_TEMPLATE_NAME	    "name"
#define XML_ATTR_TEMPLATE_PARAMETER "p"

std::map<std::string, TraceTemplateParam> const templateParameters = {
    {"hour",    Hour},
    {"min",     Minute},
    {"sec",     Second},
    {"msec",    MSecond},
    {"thread",  Thread},
    {"level",   Level},
    {"cmp",     Component} };

std::map<tstring, TraceLevel> const traceLevels = {
    {TEXT("WRN"), WRN},
    {TEXT("DBG"), DBG},
    {TEXT("INF"), INF},
    {TEXT("ERR"), ERR},
    {TEXT("ALW"), ALW},
    {TEXT("IMP"), IMP} };

TracesParser CreateDefaultTracesParser()
{
    TraceTemplateValue const fullTemplate(DEFAULT_TRACE_FULL_TEMPLATE, DEFAULT_TRACE_FULL_TEMPLATE_PARAMS);
    TraceTemplateValue const fastTemplate(DEFAULT_TRACE_FAST_TEMPLATE, DEFAULT_TRACE_FAST_TEMPLATE_PARAMS);

    return TracesParser(DEFAULT_TRACE_TEMPLATE_NAME, fullTemplate, fastTemplate);
}

TracesParser::TracesParser(
    tstring const& traceTemplateName, TraceTemplateValue const& fullTemplate, TraceTemplateValue const& fastTemplate) :
	m_templateName(traceTemplateName)
{
    ASSERT(!m_templateName.empty());
    ASSERT(!fullTemplate.regex.empty() && !fullTemplate.params.empty());
    ASSERT(!fastTemplate.regex.empty() && !fastTemplate.params.empty());

    m_fullTemplate = std::make_pair(fullTemplate, Regex(fullTemplate.regex));
    m_fastTemplate = std::make_pair(fastTemplate, Regex(fastTemplate.regex));
}

bool TracesParser::Parse(
    tstring const& trace, 
    TraceDescription& traceDescription,
    tstring::const_iterator& traceTextBegin,
    bool fullMode/* = true*/) const
{
    if (trace.empty())
        return false;

    traceTextBegin = trace.end();

    Template const& traceTemplate = (fullMode ? m_fullTemplate : m_fastTemplate);

    std::match_results<tstring::const_iterator> result;

    if (std::regex_search(trace.begin(), trace.end(), result, traceTemplate.second))
    {
        std::vector<TraceTemplateParam> const& params = traceTemplate.first.params;

        if ((result.size() - 1) != params.size())
            return false;

        TraceDescription description;

        auto paramIterator = params.begin();
        for (auto resultIterator = ++result.begin();
            (resultIterator != result.end()) && (paramIterator != params.end()); ++resultIterator, ++paramIterator)
        {
            switch (*paramIterator)
            {
                case Hour:
                {
                    description.time.hour = std::stoi(resultIterator->str());
                    break;
                }
                
                case Minute:
                {
                    description.time.minute = std::stoi(resultIterator->str());
                    break;
                }

                case Second:
                {
                    description.time.second = std::stoi(resultIterator->str());
                    break;
                }

                case MSecond:
                {
                    description.time.msecond = std::stoi(resultIterator->str());
                    break;
                }

                case Thread:
                {
                    description.threadId = std::stoi(resultIterator->str(), nullptr, 16);
                    break;
                }

                case Level:
                {
                    auto const& findedLevel = traceLevels.find(resultIterator->str());
                    if (findedLevel != traceLevels.end())
                        description.level = findedLevel->second;
                    else
                        return false;

                    break;
                }

                case Component:
                {
                    description.component = resultIterator->str();
                    break;
                }

                default:
                    return false;
            }
        }

        traceDescription    = description;
        traceTextBegin      = result.begin()->second;
        return true;
    }
    else
        return false;
}

tstring TracesParser::GetName() const
{
	return m_templateName;
}

void TracesParser::GetTemplate(TraceTemplateValue& fullTemplate, TraceTemplateValue& fastTemplate) const
{
    fullTemplate = m_fullTemplate.first;
    fastTemplate = m_fastTemplate.first;
}


bool TracesParserProvider::Create(tstring const& tracesTemplatesPath)
try
{
    ASSERT(!tracesTemplatesPath.empty());

    m_tracesTemplatesFilePath = tracesTemplatesPath;
    m_tracesTemplatesFilePath += TEXT("\\");
    m_tracesTemplatesFilePath += TRACES_TEMPLATES_FILE_NAME;
    
    if (std::filesystem::exists(m_tracesTemplatesFilePath))
    {
        if (Load())
        {
            return true;
        }
        else
        {
            tstring const backupTracesTemplatesFilePath = m_tracesTemplatesFilePath + TEXT(".backup");
            std::filesystem::remove(backupTracesTemplatesFilePath);
            std::filesystem::rename(m_tracesTemplatesFilePath, backupTracesTemplatesFilePath);
        }
    }
    
    m_parsers.push_back(CreateDefaultTracesParser());
    
    return Save();
}   
catch (...)
{
    return false;
}

size_t TracesParserProvider::GetCountParsers() const
{
    return m_parsers.size();
}

TracesParser TracesParserProvider::GetParser(size_t i) const
{
    return m_parsers[i];
}

bool TracesParserProvider::Load()
{
    m_parsers.clear();

    TiXmlDocument document;
    if (document.LoadFile(ToString(m_tracesTemplatesFilePath)))
    {
		TiXmlElement* rootNode = document.RootElement();
		if (!rootNode)
			return false;

		for (TiXmlElement* templateNode = rootNode->FirstChildElement(XML_TAG_TEMPLATE);
			templateNode; templateNode = templateNode->NextSiblingElement(XML_TAG_TEMPLATE))
		{
			const char* traceTemplateName   = templateNode->Attribute(XML_ATTR_TEMPLATE_NAME);
            if (!traceTemplateName)
                return false;

            TiXmlElement* fullTemplateNode = templateNode->FirstChildElement(XML_TAG_TEMPLATE_FULL_MODE);
            if (!fullTemplateNode)
                return false;

            TiXmlElement* fastTemplateNode = fullTemplateNode->NextSiblingElement(XML_TAG_TEMPLATE_FAST_MODE);

            TraceTemplateValue fullTemplate;
            TraceTemplateValue fastTemplate;

            if (fullTemplateNode &&
                fastTemplateNode &&
                LoadTemplate(fullTemplateNode, fullTemplate) &&
                LoadTemplate(fastTemplateNode, fastTemplate))
            {
                TracesParser parser(ToTString(traceTemplateName), fullTemplate, fastTemplate);
                m_parsers.push_back(parser);
            }
            else
            {
                return false;
            }
		}

		if (m_parsers.empty())
			return false;

        return true;
    }
    else
        return false;
}

bool TracesParserProvider::LoadTemplate(TiXmlElement* templateNode, TraceTemplateValue& templateValue) const
{
    if (!templateNode)
        return false;

    TraceTemplateValue traceTemplateValue;

    TiXmlElement* regexNode = templateNode->FirstChildElement(XML_TAG_REGEX);
    if (regexNode)
    {
        if (char const* regexText = regexNode->GetText())
            traceTemplateValue.regex = ToTString(regexText);
        else
            return false;
    }
    else
        return false;

    TiXmlElement* paramsNode = templateNode->FirstChildElement(XML_TAG_PARAMS);
    if (paramsNode)
    {
        for (TiXmlAttribute* attribute = paramsNode->FirstAttribute();
            attribute; attribute = attribute->Next())
        {
            auto templateParameter = templateParameters.find(attribute->ValueStr());
            if (templateParameter == templateParameters.end())
                return false;

            if (std::find(traceTemplateValue.params.begin(), traceTemplateValue.params.end(), templateParameter->second) !=
                traceTemplateValue.params.end())
                return false;

            traceTemplateValue.params.push_back(templateParameter->second);
        }

        if (traceTemplateValue.params.empty())
            return false;
    }
    else
        return false;

    templateValue = traceTemplateValue;
    return true;
}

bool TracesParserProvider::Save() const
{
    ASSERT(!m_parsers.empty());

    TiXmlDocument document;

    document.LinkEndChild(new TiXmlDeclaration("1.0", "utf-8", "yes"));
    TiXmlNode* rootNode = document.LinkEndChild(new TiXmlElement(XML_TAG_ROOT));

    for (TracesParser const& parser : m_parsers)
    {
        TiXmlElement* templateNode = rootNode->LinkEndChild(new TiXmlElement(XML_TAG_TEMPLATE))->ToElement();
        templateNode->SetAttribute(XML_ATTR_TEMPLATE_NAME, ToString(parser.GetName()));

        TiXmlElement* fullTemplateNode =
            templateNode->LinkEndChild(new TiXmlElement(XML_TAG_TEMPLATE_FULL_MODE))->ToElement();
        
        TiXmlElement* fastTemplateNode =
            templateNode->LinkEndChild(new TiXmlElement(XML_TAG_TEMPLATE_FAST_MODE))->ToElement();

        TraceTemplateValue fullTemplate;
        TraceTemplateValue fastTemplate;
        
        parser.GetTemplate(fullTemplate, fastTemplate);

        SaveTemplate(fullTemplateNode, fullTemplate);
        SaveTemplate(fastTemplateNode, fastTemplate);
    }

    return document.SaveFile(ToString(m_tracesTemplatesFilePath));
}

void TracesParserProvider::SaveTemplate(TiXmlElement* templateNode, TraceTemplateValue const& templateValue) const
{
    ASSERT(templateNode);

    TiXmlElement* regexNode =
        templateNode->LinkEndChild(new TiXmlElement(XML_TAG_REGEX))->ToElement();

    regexNode->LinkEndChild(new TiXmlText(ToString(templateValue.regex)));

    TiXmlElement* paramsNode =
        templateNode->LinkEndChild(new TiXmlElement(XML_TAG_PARAMS))->ToElement();

    int i = 0;
    for (TraceTemplateParam const& parameter : templateValue.params)
    {
        std::string parameterText;
        for (auto templateParameter : templateParameters)
        {
            if (parameter == templateParameter.second)
            {
                std::stringstream ss;
                ss << XML_ATTR_TEMPLATE_PARAMETER << i++;

                paramsNode->SetAttribute(ss.str(), templateParameter.first);
                
                break;
            }
        }
    }

    ASSERT(i == templateValue.params.size());
}