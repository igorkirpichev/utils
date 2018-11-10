#include "traces_parser_provider.h"

#include "tinyxml/tinyxml.h"

#include "afxwin.h"

#define TRACES_TEMPLATES_FILE_NAME  TEXT("traces_templates.xml")
#define DEFAULT_TRACE_TEMPLATE      TEXT("DEFAULT_TRACE_TEMPLATE")
#define DEFAULT_TRACE_TEMPLATE_NAME TEXT("DEFAULT_TRACE_TEMPLATE_NAME")

// XML tags
#define XML_TAG_ROOT			"traces_templates_root"
#define XML_TAG_TEMPLATE		"template"

// XML attributes
#define XML_ATTR_TEMPLATE_NAME	"name"


TracesParser::TracesParser(tstring const& traceTemplateName, tstring const& traceTemplate) :
	m_traceTemplateName(traceTemplateName),
    m_traceTemplate(traceTemplate)
{
    ASSERT(!m_traceTemplateName.empty() && !m_traceTemplate.empty());
}

void TracesParser::Parse(tstring const& string)
{

}

tstring TracesParser::GetName() const
{
	return m_traceTemplateName;
}

tstring TracesParser::GetTemplate() const
{
	return m_traceTemplate;
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
                TracesParser parser(ToTString(traceTemplateName), ToTString(traceTemplate));
                m_parsers.push_back(parser);
			}
		}

		if (m_parsers.empty())
			return false;

        return true;
    }
    else
        return false;
}
