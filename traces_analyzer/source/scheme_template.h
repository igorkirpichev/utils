#pragma once

#include "helpers/string.h"

#include <vector>

enum SchemeTemplateClassID
{
    SingleTemplate = 0,
    MultipleTemplate
};

class TracePoint
{


};

class SchemeTemplate
{
public:
    SchemeTemplate(tstring const& name);
    virtual ~SchemeTemplate();

public:
    virtual SchemeTemplateClassID GetClassID() const = 0;

public:
    tstring GetName() const;

private:
    tstring const m_name;

};

class SchemeTemplatesReceiver 
{
public:
    virtual void EnumTemplate(SchemeTemplate* schemeTemplate) = 0;
};

class SchemeTemplatesEnumerator
{
public:
    virtual void Enumerate(SchemeTemplatesReceiver const& receiver) const = 0;

private:
    std::vector<std::unique_ptr<SchemeTemplate>> m_templates;
};

class SingleSchemeTemplate :
    public SchemeTemplate
{
public:
    SingleSchemeTemplate(tstring const& name);

private:
    const SchemeTemplateClassID classId = SingleTemplate;

public:
    // SchemeTemplate methods
    SchemeTemplateClassID GetClassID() const { return classId; };

public:
    void GetTracePoint(TracePoint& tracePoint) const;

private:
    TracePoint const m_tracePoint;
};

class MultipleSchemeTemplate :
    public SchemeTemplate,
    public SchemeTemplatesEnumerator
{
public:
    MultipleSchemeTemplate(tstring const& name);

private:
    const SchemeTemplateClassID classId = MultipleTemplate;

public:
    // SchemeTemplate methods
    SchemeTemplateClassID GetClassID() const { return classId; };

public:
    void GetTracePoint(TracePoint& beginTracePoint, TracePoint& endTracePoint) const;
    
private:
    TracePoint const m_beginTracePoint;
    TracePoint const m_endTracePoint;
};
