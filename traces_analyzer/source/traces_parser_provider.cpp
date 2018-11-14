#include "traces_parser_provider.h"

#include "tinyxml/tinyxml.h"

#include "afxwin.h"

#define TRACES_TEMPLATES_FILE_NAME  TEXT("traces_templates.xml")
#define DEFAULT_TRACE_TEMPLATE      TEXT("^([\\d]{2,2}):([\\d]{2,2}):([\\d]{2,2}).([\\d]{3,3}).*")
#define DEFAULT_TRACE_TEMPLATE_NAME TEXT("KAV_KIS")

// XML tags
#define XML_TAG_ROOT			"traces_templates_root"
#define XML_TAG_TEMPLATE		"template"

// XML attributes
#define XML_ATTR_TEMPLATE_NAME	"name"


TracesParser::TracesParser(tstring const& traceTemplateName, tstring const& traceTemplate) :
	m_templateName(traceTemplateName),
    m_fullTemplate(traceTemplate),
    m_fullRegex(traceTemplate)
{
    ASSERT(!m_templateName.empty() && !m_fullTemplate.empty());
}

bool TracesParser::Parse(tstring const& trace)
{
    if (!trace.empty())
        return false;

    std::match_results<tstring::const_iterator> result;
    if (std::regex_match(trace.begin(), trace.end(), result, m_fullRegex))
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

tstring TracesParser::GetTemplate() const
{
	return m_fullTemplate;
}

TracesParserProvider::TracesParserProvider()
{
}

TracesParserProvider::~TracesParserProvider()
{
}

TracesParserProvider& TracesParserProvider::GetInstance()
{
    static TracesParserProvider tracesParserProvider;
    return tracesParserProvider;
}

void TracesParserProvider::Create(tstring const& tracesTemplatesPath)
try
{
    ASSERT(!tracesTemplatesPath.empty());

    m_tracesTemplatesFilePath = tracesTemplatesPath;
    m_tracesTemplatesFilePath += TEXT("\\");
    m_tracesTemplatesFilePath += TRACES_TEMPLATES_FILE_NAME;
    
    if (!Load())
    {
        m_parsers.push_back(TracesParser(DEFAULT_TRACE_TEMPLATE_NAME, DEFAULT_TRACE_TEMPLATE));

        Save();
    }
}
catch (...)
{
}

void TracesParserProvider::Save() const
{
    TiXmlDocument document;

	document.LinkEndChild(new TiXmlDeclaration("1.0", "utf-8", "yes"));
	TiXmlNode* rootNode = document.LinkEndChild(new TiXmlElement(XML_TAG_ROOT));

	ASSERT(rootNode);

	for (TracesParser const& parser : m_parsers)
	{
		TiXmlElement* templateNode = rootNode->LinkEndChild(new TiXmlElement(XML_TAG_TEMPLATE))->ToElement();
		ASSERT(templateNode);

		templateNode->SetAttribute(XML_ATTR_TEMPLATE_NAME, ToString(parser.GetName()));
		templateNode->LinkEndChild(new TiXmlText(ToString(parser.GetTemplate())));
	}

    document.SaveFile(ToString(m_tracesTemplatesFilePath));
}

size_t TracesParserProvider::GetCountParsers() const
{
    return m_parsers.size();
}

TracesParser& TracesParserProvider::GetParser(size_t i)
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
			const char* traceTemplate       = templateNode->GetText();

			if (traceTemplateName && traceTemplate)
			{
                try
                {
                    TracesParser parser(ToTString(traceTemplateName), ToTString(traceTemplate));
                    m_parsers.push_back(parser);
                }
                catch (...)
                {
                }
			}
		}

		if (m_parsers.empty())
			return false;

        return true;
    }
    else
        return false;
}
