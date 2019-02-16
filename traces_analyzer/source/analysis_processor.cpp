#include "analysis_processor.h"


AnalysisProcessor::AnalysisProcessor() :
    m_inProcess(false)
{
    
}


AnalysisProcessor::~AnalysisProcessor()
{
}

bool AnalysisProcessor::StartProcess(AnalysisProcessContext const& analysisProcessContext)
{
    if (m_inProcess.load())
        return false;

    std::thread(DoWork, this, analysisProcessContext).detach();

    return true;
}

void AnalysisProcessor::DoWork(AnalysisProcessor* parent, AnalysisProcessContext analysisProcessContext)
{
    parent->m_inProcess.store(true);

    Sleep(5000);

    parent->m_inProcess.store(false);
}