#pragma once
#include "scheme_template.h"
#include "helpers/string.h"
#include "helpers/non_copyable.h"

#include "tinyxml/tinyxml.h"

#define SCHEME_FILE_EXTENSTION          TEXT("xml")
#define SCHEME_DEFAULT_FILE_NAME        TEXT("New scheme")
#define SCHEME_DEFAULT_FILE_NAME_FILTER TEXT("Traces analyzer scheme (*.xml) | *.xml|")

class PluginFrame;

class Scheme
{
private:
    typedef std::vector<std::unique_ptr<SchemeTemplate>> SchemeTemplates;

public:
    bool                Load(tstring const& filePath);
    bool                Save(tstring const& filePath = tstring());
    
    bool                IsModified() const;
    tstring             GetFileName() const;
    tstring             GetDisplayFileName() const;

    size_t              GetCountSchemeTemplates() const;
    SchemeTemplate*     GetSchemeTemplate(size_t i) const;

private:
    bool MakeTemplate(TiXmlElement* templateNode, std::unique_ptr<SchemeTemplate>& schemeTemplate);
    bool MakeTracePoint(TiXmlElement* tracePointNode, TracePoint& tracePoint);

private:
    tstring         m_fileName;
    SchemeTemplates m_schemeTemplates;
};