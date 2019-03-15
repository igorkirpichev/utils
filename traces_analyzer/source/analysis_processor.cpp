#include "analysis_processor.h"
#include "scintilla.h"

#include "npp/notepad_plus_msgs.h"

#include <thread>
#include <vector>

class ScopedProcessStateSwitch
{
public:
    ScopedProcessStateSwitch(std::atomic_bool& stateProcess, std::condition_variable& notificator) :
        m_stateProcess(stateProcess),
        m_notificator(notificator)
    {
        ASSERT(!m_stateProcess.exchange(true));
    };

    ~ScopedProcessStateSwitch()
    {
        ASSERT(m_stateProcess.exchange(false));
        m_notificator.notify_all();
    }


private:
    std::atomic_bool&           m_stateProcess;
    std::condition_variable&    m_notificator;
};


AnalysisProcessor::AnalysisProcessor() :
    m_inProcess(false),
    m_doCancel(false)
{
    
}


AnalysisProcessor::~AnalysisProcessor()
{
    CancelProcess();
}

bool AnalysisProcessor::StartProcess(AnalysisProcessContext const& analysisProcessContext)
try
{
    if (m_inProcess.load())
        return false;

    m_analysisSession.reset(new AnalysisSession(analysisProcessContext));
    
    std::thread(DoWork, this).detach();

    return true;
}
catch (...)
{
    m_analysisSession.reset(nullptr);
    return false;
}

void AnalysisProcessor::CancelProcess()
{
    if (m_inProcess.load())
    {
        m_doCancel.exchange(true);
        
        // wait complete thread
        std::unique_lock<std::mutex> lock(m_processDoneGuard);
        m_processDone.wait(lock, 
            [this]()
            {
                return !m_inProcess.load();
            });

        m_doCancel.exchange(false);
    }
}

void AnalysisProcessor::DoWork(AnalysisProcessor* p_this)
{
    ASSERT(p_this);
    
    ScopedProcessStateSwitch scopedProcessStateSwitch(p_this->m_inProcess, p_this->m_processDone);

    AnalysisSession* analysisSession = p_this->m_analysisSession.get();
    
    size_t const schemeTemplateCount = analysisSession->scheme.GetCountSchemeTemplates();
    
    if (!schemeTemplateCount)
        return;
    
    if (analysisSession->frameCallback)
        analysisSession->frameCallback->OnStartProcess();
    
	analysisSession->notepad.SaveCurrentFile();
	analysisSession->notepad.HideTabBar(true);
	analysisSession->scintilla.SetReadOnly(true);

    for (size_t i = 0; (i < schemeTemplateCount) && !p_this->m_doCancel.load(); ++i)
    {
        SchemeTemplate* schemeTemplate = analysisSession->scheme.GetSchemeTemplate(i);

        p_this->ProcessSchemeTemplate(schemeTemplate);
    }

    analysisSession->scintilla.SetReadOnly(false);
	analysisSession->notepad.HideTabBar(false);
    
    if (analysisSession->frameCallback)
        analysisSession->frameCallback->OnFinishProcess();
}

void AnalysisProcessor::ProcessSchemeTemplate(SchemeTemplate* schemeTemplate)
{
    ASSERT(schemeTemplate);

    switch (schemeTemplate->GetClassID())
    {
        case SingleTemplate:
        {
            SingleSchemeTemplate* singleSchemeTemplate = static_cast<SingleSchemeTemplate*>(schemeTemplate);
            ProcessSingleSchemeTemplate(singleSchemeTemplate);
            break;
        }

        case MultipleTemplate:
        {
            MultipleSchemeTemplate* multipleSchemeTemplate = static_cast<MultipleSchemeTemplate*>(schemeTemplate);
            ProcessMultipleSchemeTemplate(multipleSchemeTemplate);
            break;
        }

        default:
            ASSERT(false);
    }
}

void AnalysisProcessor::ProcessSingleSchemeTemplate(SingleSchemeTemplate* schemeTemplate)
{
    ASSERT(schemeTemplate);

    TracePoint tracePoint;
    schemeTemplate->GetTracePoint(tracePoint);

    Sci_TextToFind textToFind = {0};
    textToFind.lpstrText = const_cast<char*>(tracePoint.expression.c_str());
    textToFind.chrg.cpMin = 0;
    textToFind.chrg.cpMax = static_cast<long>(m_analysisSession->documentProperties.charCount);

    int searchFlags = 0;
    if (tracePoint.regex)
        searchFlags |= SCFIND_REGEXP;

    while (!m_doCancel.load())
    {
        size_t const findedPosition = m_analysisSession->scintilla.FindText(searchFlags, textToFind);

        if (findedPosition == -1)
            break;

        size_t const line = m_analysisSession->scintilla.LineFromPosition(findedPosition);

		std::string lineString = m_analysisSession->scintilla.GetLine(line);
		if (!lineString.empty())
        {
            RemoveCarriageReturns(lineString);

            TraceDescription traceDescription;
            std::string::const_iterator traceBegin;
            if (m_analysisSession->tracesParser.Parse(lineString, traceDescription, traceBegin, false))
            {

            }
        }

        textToFind.chrg.cpMin = static_cast<long>(m_analysisSession->scintilla.GetLineEndPosition(line));
    }
}

void AnalysisProcessor::ProcessMultipleSchemeTemplate(MultipleSchemeTemplate* schemeTemplate)
{
    ASSERT(schemeTemplate);
}