#pragma once

#include "scintilla.h"
#include "traces_parser_provider.h"
#include "scheme.h"

#include <thread>
#include <atomic>

struct AnalysisProcessContext
{
    Scintilla       scintillaView;
    TracesParser    tracesParser;
    Scheme&         scheme;
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

