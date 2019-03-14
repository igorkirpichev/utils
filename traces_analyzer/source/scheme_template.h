#pragma once

#include "helpers/string.h"

#include <vector>

enum SchemeTemplateClassID
{
    SingleTemplate      = 0,
    MultipleTemplate    = 1
};

class SchemeTemplate;

class ISchemeTemplatesReceiver
{
public:
    virtual void EnumTemplate(SchemeTemplate* schemeTemplate) = 0;
};


struct TracePoint
{
    TracePoint() :
        regex(0)
    {};
    
    std::string expression;
    bool regex;
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


class SingleSchemeTemplate :
    public SchemeTemplate
{
public:
    SingleSchemeTemplate(tstring const& name, TracePoint const& tracePoint);

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
    public SchemeTemplate
{
public:
    MultipleSchemeTemplate(
        tstring const& name, TracePoint const& beginTracePoint, TracePoint const& endTracePoint);

private:
    const SchemeTemplateClassID classId = MultipleTemplate;

public:
    // SchemeTemplate methods
    SchemeTemplateClassID GetClassID() const { return classId; };

public:
    void GetTracePoint(TracePoint& beginTracePoint, TracePoint& endTracePoint) const;
    void EnumerateTemplates(ISchemeTemplatesReceiver* receiver) const;
    
private:
    TracePoint const m_beginTracePoint;
    TracePoint const m_endTracePoint;

    std::vector<std::unique_ptr<SchemeTemplate>> m_templates;
};
