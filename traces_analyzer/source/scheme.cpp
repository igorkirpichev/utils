#include "scheme.h"

#include <afxwin.h>

// XML tags
#define XML_TAG_ROOT			    "scheme_templates_root"
#define XML_TAG_TEMPLATE		    "template"
#define XML_TAG_REGEX               "regex"
#define XML_TAG_TRACE_POINT         "trace_point"
#define XML_TAG_BEGIN_TRACE_POINT   "begin_trace_point"
#define XML_TAG_END_TRACE_POINT     "end_trace_point"

// XML attributes
#define XML_ATTR_TEMPLATE_NAME	    "name"
#define XML_ATTR_TEMPLATE_CLASS_ID  "class_id"

SchemeItem::SchemeItem(std::unique_ptr<SchemeTemplate>&& schemeTemplate) :
    m_schemeTemplate(std::move(schemeTemplate)),
    m_enabled(true)
{
}

bool SchemeItem::IsEnabled() const
{
    return m_enabled;
}

bool Scheme::Load(tstring const& filePath)
{
    ASSERT(!filePath.empty());

    m_fileName.clear();

    if (filePath.empty())
        return false;

    TiXmlDocument document;
    if (document.LoadFile(ToString(filePath)))
    {
        TiXmlElement* rootNode = document.RootElement();
        if (!rootNode)
            return false;

        std::vector<SchemeItem> schemeItems;

        for (TiXmlElement* templateNode = rootNode->FirstChildElement(XML_TAG_TEMPLATE);
            templateNode; templateNode = templateNode->NextSiblingElement(XML_TAG_TEMPLATE))
        {
            std::unique_ptr<SchemeTemplate> schemeTemplate;
            if (!MakeTemplate(templateNode, schemeTemplate))
                return false;

            ASSERT(schemeTemplate);
            
            schemeItems.emplace_back(std::move(schemeTemplate));
        }

        m_items = schemeItems;
        m_fileName = filePath;
        return true;
    }

    return false;
}

bool Scheme::MakeTemplate(TiXmlElement* templateNode, std::unique_ptr<SchemeTemplate>& schemeTemplate)
{
    if (!templateNode)
        return false;

    schemeTemplate.reset();

    const char* schemeTemplateName = templateNode->Attribute(XML_ATTR_TEMPLATE_NAME);
    if (!schemeTemplateName)
        return false;

    int schemeTemplateClassId = 0;
    if (!templateNode->Attribute(XML_ATTR_TEMPLATE_CLASS_ID, &schemeTemplateClassId))
        return false;

    std::unique_ptr<SchemeTemplate> newSchemeTemplate;

    switch (static_cast<SchemeTemplateClassID>(schemeTemplateClassId))
    {
        case SingleTemplate:
        {
            tstring regexString;
            
            TiXmlElement* tracePointNode = templateNode->FirstChildElement(XML_TAG_TRACE_POINT);
            if (!MakeTracePoint(tracePointNode, regexString))
                return false;

            newSchemeTemplate.reset(new SingleSchemeTemplate(ToTString(schemeTemplateName), regexString));
            break;
        }

        case MultipleTemplate:
        {
            tstring beginRegexString;
            tstring endRegexString;

            TiXmlElement* beginTracePointNode = templateNode->FirstChildElement(XML_TAG_BEGIN_TRACE_POINT);
            if (!MakeTracePoint(beginTracePointNode, beginRegexString))
                return false;

            TiXmlElement* endTracePointNode = beginTracePointNode->NextSiblingElement(XML_TAG_END_TRACE_POINT);
            if (!MakeTracePoint(endTracePointNode, endRegexString))
                return false;

            newSchemeTemplate.reset(
                new MultipleSchemeTemplate(ToTString(schemeTemplateName), beginRegexString, endRegexString));
            break;
        }

        default:
            return false;
    }

    ASSERT(newSchemeTemplate);
    schemeTemplate.swap(newSchemeTemplate);

    return true;
}

bool Scheme::MakeTracePoint(TiXmlElement* tracePointNode, tstring& regexString)
{
    if (!tracePointNode)
        return false;

    regexString.clear();

    TiXmlElement* regexNode = tracePointNode->FirstChildElement(XML_TAG_REGEX);
    if (regexNode)
    {
        if (char const* regexText = regexNode->GetText())
            regexString = ToTString(regexText);
        else
            return false;
        
        return true;
    }
    else
    {
        return false;
    }
}


bool Scheme::Save(tstring const& filePath)
{
    return true;
}

bool Scheme::IsModified() const
{
    return false;
}

tstring Scheme::GetFileName() const
{
    return m_fileName;
}

tstring Scheme::GetDisplayFileName() const
{
    return m_fileName.substr(m_fileName.find_last_of(TEXT("/\\")) + 1);
}

size_t Scheme::GetCountSchemeItems() const
{
    return m_items.size();
}

SchemeItem const& Scheme::GetSchemeItem(size_t i) const
{
    return m_items[i];
}