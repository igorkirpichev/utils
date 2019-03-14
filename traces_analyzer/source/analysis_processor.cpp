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
    
    analysisSession->notepad.SendMsg(NPPM_SAVECURRENTFILE);
    analysisSession->notepad.SendMsg(NPPM_HIDETABBAR, 0, 1);
    analysisSession->scintilla.DirectCall(SCI_SETREADONLY, 1);

    for (size_t i = 0; (i < schemeTemplateCount) && !p_this->m_doCancel.load(); ++i)
    {
        SchemeTemplate* schemeTemplate = analysisSession->scheme.GetSchemeTemplate(i);

        p_this->ProcessSchemeTemplate(schemeTemplate);
    }

    analysisSession->scintilla.DirectCall(SCI_SETREADONLY, 0);
    analysisSession->notepad.SendMsg(NPPM_HIDETABBAR, 0, 0);
    
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
    textToFind.chrg.cpMax = m_analysisSession->documentProperties.charCount;

    int searchFlags = 0;
    if (tracePoint.regex)
        searchFlags |= SCFIND_REGEXP;

    while (!m_doCancel.load())
    {
        int const findedPosition = static_cast<int>(m_analysisSession->scintilla.DirectCall(
            SCI_FINDTEXT, static_cast<WPARAM>(searchFlags), reinterpret_cast<LPARAM>(&textToFind)));

        if (findedPosition == -1)
            break;

        int const line = static_cast<int>(m_analysisSession->scintilla.DirectCall(
            SCI_LINEFROMPOSITION, static_cast<WPARAM>(findedPosition)));

        int const lineLength = static_cast<int>(m_analysisSession->scintilla.DirectCall(
            SCI_LINELENGTH, static_cast<WPARAM>(line)));

        std::string lineString(static_cast<size_t>(lineLength), 0);

        if (m_analysisSession->scintilla.DirectCall(
            SCI_GETLINE, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(lineString.data())))
        {
            RemoveCarriageReturns(lineString);

            TraceDescription traceDescription;
            tstring::const_iterator traceBegin;
            if (m_analysisSession->tracesParser.Parse(ToTString(lineString), traceDescription, traceBegin, false))
            {

            }
        }

        int const lineEndPosition = static_cast<int>(m_analysisSession->scintilla.DirectCall(
            SCI_GETLINEENDPOSITION, static_cast<WPARAM>(line)));
        
        textToFind.chrg.cpMin = lineEndPosition;
    }
}

void AnalysisProcessor::ProcessMultipleSchemeTemplate(MultipleSchemeTemplate* schemeTemplate)
{
    ASSERT(schemeTemplate);
}