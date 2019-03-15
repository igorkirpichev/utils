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
	AnalysisResult&						analysisResult;
};

struct DocumentProperties
{
    DocumentProperties() :
        charCount(0),
		lineCount(0)
    {}

    tstring   fullCurrentPath;
    size_t    charCount;
    size_t    lineCount;
};

class AnalysisSession :
    public AnalysisProcessContext
{
public:
    AnalysisSession(AnalysisProcessContext const& analysisProcessContext) :
        AnalysisProcessContext(analysisProcessContext)
    {
        documentProperties.fullCurrentPath  = notepad.GetFullCurrentPath();
        documentProperties.charCount        = scintilla.GetLength();
        documentProperties.lineCount        = scintilla.GetLineCount();
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

