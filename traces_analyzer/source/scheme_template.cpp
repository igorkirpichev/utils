#include "scheme_template.h"

#include "afxwin.h"

#include <algorithm>

SchemeTemplate::SchemeTemplate(tstring const& name) :
    m_name(name)
{}

SchemeTemplate::~SchemeTemplate()
{
}

tstring SchemeTemplate::GetName() const
{
    return m_name;
}


SingleSchemeTemplate::SingleSchemeTemplate(tstring const& name) :
    SchemeTemplate(name),
    m_tracePoint(TracePoint())
{
}

void SingleSchemeTemplate::GetTracePoint(TracePoint& tracePoint) const
{
    tracePoint = m_tracePoint;
}


MultipleSchemeTemplate::MultipleSchemeTemplate(tstring const& name) :
    SchemeTemplate(name),
    m_beginTracePoint(TracePoint()),
    m_endTracePoint(TracePoint())
{
}

void MultipleSchemeTemplate::GetTracePoint(TracePoint& beginTracePoint, TracePoint& endTracePoint) const
{
    beginTracePoint = m_beginTracePoint;
    endTracePoint   = m_endTracePoint;
}

void MultipleSchemeTemplate::EnumerateTemplates(ISchemeTemplatesReceiver* receiver) const
{
    ASSERT(receiver);

    std::for_each(m_templates.begin(), m_templates.end(),
        [receiver](std::unique_ptr<SchemeTemplate> const& schemeTemplate)
        {
            receiver->EnumTemplate(schemeTemplate.get());
        });
}
