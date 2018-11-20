#include "traces_parser_provider.h"

#include "afxwin.h"

#include <filesystem>

#if !_HAS_CXX17
namespace std
{
    namespace filesystem = std::experimental::filesystem;
} // namespace std
#endif // !_HAS_CXX17

#define TRACES_TEMPLATES_FILE_NAME  TEXT("traces_templates.xml")
#define DEFAULT_TRACE_TEMPLATE      TEXT("^([\\d]{2,2}):([\\d]{2,2}):([\\d]{2,2}).([\\d]{3,3}).*")
#define DEFAULT_TRACE_TEMPLATE_NAME TEXT("KAV_KIS")

// XML tags
#define XML_TAG_ROOT			    "traces_templates_root"
#define XML_TAG_TEMPLATE		    "template"
#define XML_TAG_TEMPLATE_FULL_MODE  "full_mode"
#define XML_TAG_TEMPLATE_FAST_MODE  "fast_mode"

// XML attributes
#define XML_ATTR_TEMPLATE_NAME	    "name"


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

bool TracesParser::Parse(tstring const& trace, bool fullMode/* = true*/) const
{
    if (trace.empty())
        return false;

    Regex const& regex = (fullMode ? m_fullTemplate : m_fastTemplate).second;

    std::match_results<tstring::const_iterator> result;
    if (std::regex_match(trace.begin(), trace.end(), result, regex))
    {
        for (auto const& i : result)
        {
            tstring const sub = i.str();

        }

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
    
    //m_parsers.push_back(TracesParser(DEFAULT_TRACE_TEMPLATE_NAME, DEFAULT_TRACE_TEMPLATE));
    //return Save();
    return true;
}   
catch (...)
{
    return false;
}

bool TracesParserProvider::Save() const
{
    if (m_parsers.empty())
        return false;

    TiXmlDocument document;

	document.LinkEndChild(new TiXmlDeclaration("1.0", "utf-8", "yes"));
	TiXmlNode* rootNode = document.LinkEndChild(new TiXmlElement(XML_TAG_ROOT));

	ASSERT(rootNode);

	for (TracesParser const& parser : m_parsers)
	{
		TiXmlElement* templateNode = rootNode->LinkEndChild(new TiXmlElement(XML_TAG_TEMPLATE))->ToElement();
		ASSERT(templateNode);

		templateNode->SetAttribute(XML_ATTR_TEMPLATE_NAME, ToString(parser.GetName()));
		//templateNode->LinkEndChild(new TiXmlText(ToString(parser.GetTemplate())));
	}

    return document.SaveFile(ToString(m_tracesTemplatesFilePath));
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

            TiXmlElement* fullTemplateNode = templateNode->FirstChildElement(XML_TAG_TEMPLATE_FULL_MODE);
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

bool TracesParserProvider::LoadTemplate(TiXmlElement* templateNode, TraceTemplateValue& templateValue)
{
    if (!templateNode)
        return false;

    return true;
}