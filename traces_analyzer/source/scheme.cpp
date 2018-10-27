#include "scheme.h"

#include <afxwin.h>

#include "tinyxml/tinyxml.h"


Scheme::Scheme()
{
    m_fileName = TEXT("");
}


Scheme::~Scheme()
{
}

//bool Scheme::Load(tstring const& filePath)
//{
//    TiXmlDocument doc(ToString(filePath));
//    return false;
//
//}

bool Scheme::Load(tstring const& filePath)
{
    return false;
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