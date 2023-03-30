#include "pch.h"

#include <filesystem>

#include "FileProcessor.h"
#include "NotifyCallback.h"

void SortTask::RunInternal()
{
    HRESULT hres = CoCreateInstance(__uuidof(SortServerLib::Sorter), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(m_pSorter.GetAddressOf()));
    if (FAILED(hres))
    {
        return;
    }
    hres = m_pSorter->raw_SortFile(bstr_t(m_inputFileName.c_str()), bstr_t(m_outputFileName.c_str()), m_mode);
    m_pSorter.ReleaseAndGetAddressOf();
}

void SortTask::Run()
{
    if (m_bCanceled)
        return;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    RunInternal();
    m_callback->Completed();
    CoUninitialize();
}

void SortTask::Cancel()
{
    if (m_pSorter)
        m_pSorter->raw_Cancel();
    m_bCanceled = true;
}

void runSortTask(std::shared_ptr<SortTask> pst)
{
    pst->Run();
}

void SortJob::AddTask(std::string description, std::shared_ptr<SortTask> pst)
{
    m_tasks.insert(std::pair{ description, pst });
    auto r = m_tasks[description];
    m_pool.add_task(runSortTask, r);
}

void SortJob::Wait()
{
    m_pool.wait_all();
}

void SortJob::Cancel()
{
    for (auto& t : m_tasks)
    {
        t.second->Cancel();
    }
}

void SortJob::Completed()
{
    m_numCompletedTasks++;
    m_callback->CompletedTaskCount(m_numCompletedTasks, m_tasks.size());
}

CFileProcessor::CFileProcessor(CString inputPath, CString outputPath, int mode, int numThreads, INotifyCallback* callback)
	:m_inputPath(inputPath),
	m_outputPath(outputPath),
	m_mode(mode),
	m_callback(callback),
    m_sd(numThreads, callback)
{
}

void CFileProcessor::run()
{
    auto iPath = std::filesystem::path((LPCTSTR)m_inputPath);
    auto oPath = std::filesystem::path((LPCTSTR)m_outputPath);

    for (auto const& dir_entry : std::filesystem::directory_iterator{ iPath })
    {
        auto inputFileName = dir_entry.path().string();
        auto outputFileName = (oPath / dir_entry.path().filename()).string();
        std::shared_ptr<SortTask> pst = std::make_shared<SortTask>(inputFileName, outputFileName, m_mode, &m_sd);
        m_sd.AddTask(inputFileName, pst);
    }
}

void CFileProcessor::stop()
{
    m_sd.Cancel();
    m_sd.Wait();
}
