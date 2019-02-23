#pragma once

#include "scintilla.h"
#include "traces_parser_provider.h"
#include "scheme.h"
#include "notepad_plus_plus.h"

#include <condition_variable>
#include <atomic>

class IAnalysisProcessorFrameCallback
{
public:
    virtual void OnStartProcess() = 0;
    virtual void OnFinishProcess() = 0;
    // insert new cb 
};

struct AnalysisProcessContext
{
    NotepadPlusPlus                     notepad;
    Scintilla                           scintilla;
    TracesParser                        tracesParser;
    IAnalysisProcessorFrameCallback*    frameCallback;
    Scheme&                             scheme;
};

class AnalysisProcessor
{
public:
    AnalysisProcessor();
    ~AnalysisProcessor();

public:
    bool StartProcess(AnalysisProcessContext const& analysisProcessContext);
    void CancelProcess();

private:
    static void DoWork(AnalysisProcessor* p_this, AnalysisProcessContext analysisProcessContext);

private:

    std::mutex              m_processDoneGuard;
    std::condition_variable m_processDone;
    std::atomic_bool        m_inProcess;
    std::atomic_bool        m_doCancel;
};

