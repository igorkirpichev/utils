#include "scheme.h"

#include <afxwin.h>

#include "tinyxml/tinyxml.h"

#define XML_TAG_ROOT			    "scheme_templates_root"
#define XML_TAG_TEMPLATE		    "template"

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
        }
    }

    return false;
}

bool Scheme::LoadTemplate(TiXmlElement* templateNode)
{

}


bool Scheme::Save(tstring const& filePath)
{
    return false;
}

bool Scheme::IsModified() const
{
    return true;
}

tstring Scheme::GetFileName() const
{
    return m_fileName;
}

tstring Scheme::GetDisplayFileName() const
{
    return m_fileName.substr(m_fileName.find_last_of(TEXT("/\\")) + 1);
}