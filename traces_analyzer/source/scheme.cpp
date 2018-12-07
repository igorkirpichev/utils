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

Scheme::Scheme()
{
}


Scheme::~Scheme()
{
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

        for (TiXmlElement* templateNode = rootNode->FirstChildElement(XML_TAG_TEMPLATE);
            templateNode; templateNode = templateNode->NextSiblingElement(XML_TAG_TEMPLATE))
        {
            std::unique_ptr<SchemeTemplate> schemeTemplate;
            if (!LoadTemplate(templateNode, schemeTemplate))
                return false;

            // create template
        }

        m_fileName = filePath;
        return true;
    }

    return false;
}

bool Scheme::LoadTemplate(TiXmlElement* templateNode, std::unique_ptr<SchemeTemplate>& schemeTemplate)
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

    switch (static_cast<SchemeTemplateClassID>(schemeTemplateClassId))
    {
        case SingleTemplate:
        {
            break;
        }

        case MultipleTemplate:
        {
            break;
        }

        default:
            return false;
    }

    return true;
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