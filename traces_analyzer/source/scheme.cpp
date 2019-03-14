#include "scheme.h"

#include <afxwin.h>

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

// XML tags
#define XML_TAG_ROOT			    "scheme_templates_root"
#define XML_TAG_TEMPLATE		    "template"
#define XML_TAG_REGEX               "regex"
#define XML_TAG_TRACE_POINT         "trace_point"
#define XML_TAG_BEGIN_TRACE_POINT   "begin_trace_point"
#define XML_TAG_END_TRACE_POINT     "end_trace_point"
#define XML_TAG_EXPRESSION          "expression"

// XML attributes
#define XML_ATTR_TEMPLATE_NAME	    "name"
#define XML_ATTR_TEMPLATE_CLASS_ID  "class_id"
#define XML_ATTR_TRACE_POINT_REGEX  "regex"

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

        SchemeTemplates schemeTemplates;

        for (TiXmlElement* templateNode = rootNode->FirstChildElement(XML_TAG_TEMPLATE);
            templateNode; templateNode = templateNode->NextSiblingElement(XML_TAG_TEMPLATE))
        {
            std::unique_ptr<SchemeTemplate> schemeTemplate;
            if (!MakeTemplate(templateNode, schemeTemplate))
                return false;

            ASSERT(schemeTemplate);

            schemeTemplates.emplace_back(std::move(schemeTemplate));
        }

        m_schemeTemplates.swap(schemeTemplates);
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
            TracePoint tracePoint;
            
            TiXmlElement* tracePointNode = templateNode->FirstChildElement(XML_TAG_TRACE_POINT);
            if (!MakeTracePoint(tracePointNode, tracePoint))
                return false;

            newSchemeTemplate.reset(new SingleSchemeTemplate(ToTString(schemeTemplateName), tracePoint));
            break;
        }

        case MultipleTemplate:
        {
            TracePoint beginTracePoint;
            TracePoint endTracePoint;

            TiXmlElement* beginTracePointNode = templateNode->FirstChildElement(XML_TAG_BEGIN_TRACE_POINT);
            if (!MakeTracePoint(beginTracePointNode, beginTracePoint))
                return false;

            TiXmlElement* endTracePointNode = beginTracePointNode->NextSiblingElement(XML_TAG_END_TRACE_POINT);
            if (!MakeTracePoint(endTracePointNode, endTracePoint))
                return false;

            newSchemeTemplate.reset(
                new MultipleSchemeTemplate(ToTString(schemeTemplateName), beginTracePoint, endTracePoint));
            break;
        }

        default:
            return false;
    }

    ASSERT(newSchemeTemplate);
    schemeTemplate.swap(newSchemeTemplate);

    return true;
}

bool Scheme::MakeTracePoint(TiXmlElement* tracePointNode, TracePoint& tracePoint)
{
    if (!tracePointNode)
        return false;

    TracePoint localTracePoint;

    int regexMode = 0;
    if (!tracePointNode->Attribute(XML_ATTR_TRACE_POINT_REGEX, &regexMode))
        return false;

    localTracePoint.regex = !!regexMode;

    TiXmlElement* expressionNode = tracePointNode->FirstChildElement(XML_TAG_EXPRESSION);
    if (expressionNode)
    {
        if (char const* expressionText = expressionNode->GetText())
            localTracePoint.expression = expressionText;
        else
            return false;

        tracePoint = localTracePoint;
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

size_t Scheme::GetCountSchemeTemplates() const
{
    return m_schemeTemplates.size();
}

SchemeTemplate* Scheme::GetSchemeTemplate(size_t i) const
{
    return m_schemeTemplates[i].get();
}
