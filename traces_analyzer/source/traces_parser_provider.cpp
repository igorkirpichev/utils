#include "traces_parser_provider.h"

#include "tinyxml/tinyxml.h"

#include "afxwin.h"

#define TRACES_TEMPLATES_FILE_NAME  TEXT("traces_templates.xml")
#define DEFAULT_TRACE_TEMPLATE      TEXT("DEFAULT_TRACE_TEMPLATE")


TracesParser::TracesParser(tstring const& traceTemplate) :
    m_traceTemplate(traceTemplate)
{
    ASSERT(!m_traceTemplate.empty());
}

void TracesParser::Parse(tstring const& string)
{

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
        m_parsers.push_back(TracesParser(DEFAULT_TRACE_TEMPLATE));

        Save();
    }
}

void TracesParserProvider::Save()
{

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

        return true;
    }
    else
        return false;
}
