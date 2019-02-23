#include "analysis_processor.h"
#include "scintilla.h"

#include "npp/notepad_plus_msgs.h"

#include <thread>

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
{
    if (m_inProcess.load())
        return false;

    std::thread(DoWork, this, analysisProcessContext).detach();

    return true;
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

void AnalysisProcessor::DoWork(AnalysisProcessor* p_this, AnalysisProcessContext analysisProcessContext)
{
    ASSERT(p_this);
    
    ScopedProcessStateSwitch scopedProcessStateSwitch(p_this->m_inProcess, p_this->m_processDone);

    Scintilla const& scintilla      = analysisProcessContext.scintilla;
    NotepadPlusPlus const& notepad  = analysisProcessContext.notepad;

    analysisProcessContext.frameCallback->OnStartProcess();
    
    tstring const fullCurrentPath = notepad.GetFullCurrentPath();

    notepad.SendMsg(NPPM_SAVECURRENTFILE);
    notepad.SendMsg(NPPM_HIDETABBAR, 0, 1);
    scintilla.DirectCall(SCI_SETREADONLY, 1);

    while (!p_this->m_doCancel.load())
        Sleep(10);

    scintilla.DirectCall(SCI_SETREADONLY, 0);
    notepad.SendMsg(NPPM_HIDETABBAR, 0, 0);

    analysisProcessContext.frameCallback->OnFinishProcess();
}