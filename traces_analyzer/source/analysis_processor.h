#pragma once

#include "scintilla.h"
#include "traces_parser_provider.h"
#include "scheme.h"

#include <thread>
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
    Scintilla                           scintillaView;
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

private:
    static void DoWork(AnalysisProcessor* parent, AnalysisProcessContext analysisProcessContext);

private:
    std::thread m_processThread;
    std::atomic<bool> m_inProcess;
};

