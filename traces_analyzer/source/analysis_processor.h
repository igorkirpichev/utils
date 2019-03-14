#pragma once

#include "scintilla.h"
#include "traces_parser_provider.h"
#include "scheme.h"
#include "notepad_plus_plus.h"

#include <condition_variable>
#include <atomic>
#include <memory>

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

struct DocumentProperties
{
    DocumentProperties() :
        charCount(0)
    {}

    tstring         fullCurrentPath;
    unsigned int    charCount;
    unsigned int    lineCount;
};

class AnalysisSession :
    public AnalysisProcessContext
{
public:
    AnalysisSession(AnalysisProcessContext const& analysisProcessContext) :
        AnalysisProcessContext(analysisProcessContext)
    {
        documentProperties.fullCurrentPath  = notepad.GetFullCurrentPath();
        documentProperties.charCount        = scintilla.DirectCall(SCI_GETLENGTH);
        documentProperties.lineCount        = scintilla.DirectCall(SCI_GETLINECOUNT);
    }

public:
    DocumentProperties documentProperties;
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
    static void DoWork(AnalysisProcessor* p_this);

private:
    void ProcessSchemeTemplate(SchemeTemplate* schemeTemplate);
    void ProcessSingleSchemeTemplate(SingleSchemeTemplate* schemeTemplate);
    void ProcessMultipleSchemeTemplate(MultipleSchemeTemplate* schemeTemplate);

private:
    std::unique_ptr<AnalysisSession> m_analysisSession;

    std::mutex              m_processDoneGuard;
    std::condition_variable m_processDone;
    std::atomic_bool        m_inProcess;
    std::atomic_bool        m_doCancel;
};

