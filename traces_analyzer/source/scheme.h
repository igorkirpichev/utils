#pragma once
#include "scheme_template.h"
#include "helpers/string.h"

#include "tinyxml/tinyxml.h"

#define SCHEME_FILE_EXTENSTION          TEXT("xml")
#define SCHEME_DEFAULT_FILE_NAME        TEXT("New scheme")
#define SCHEME_DEFAULT_FILE_NAME_FILTER TEXT("Traces analyzer scheme (*.xml) | *.xml|")

class SchemeItem
{
public:
    SchemeItem();

private:
    SchemeItem(SchemeItem const&) = delete;
    SchemeItem& operator=(SchemeItem const&) = delete;

private:
    std::unique_ptr<SchemeTemplate> m_schemeTemplate;
};

class Scheme
{
public:
    Scheme();
    ~Scheme();

public:
    bool                Load(tstring const& filePath);
    bool                Save(tstring const& filePath = tstring());
    
    bool                IsModified() const;
    tstring             GetFileName() const;
    tstring             GetDisplayFileName() const;

    size_t              GetCountSchemeItems() const;
    SchemeItem const&   GetSchemeItem(size_t i) const;

private:
    bool MakeTemplate(TiXmlElement* templateNode, std::unique_ptr<SchemeTemplate>& schemeTemplate);
    bool MakeTracePoint(TiXmlElement* tracePointNode, tstring& regexString);

private:
    tstring                 m_fileName;
    std::vector<SchemeItem> m_items;
};

