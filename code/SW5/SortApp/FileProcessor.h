#pragma once

#include <map>

#include <wrl.h>
#include <wrl\implements.h>
#include <wrl\client.h>

#include "threadpool.h"

#import "..\x64\Debug\SortServer.dll"

class INotifyCallback;

class ITaskCompletionCallback
{
public:
    virtual void Completed() = 0;
};

struct SortTask
{
    std::string m_inputFileName;
    std::string m_outputFileName;
    int m_mode;
    bool m_bCanceled;
    Microsoft::WRL::ComPtr<SortServerLib::ISorter> m_pSorter;
    ITaskCompletionCallback* m_callback;
    SortTask(std::string inputFileName, std::string outputFileName, int mode, ITaskCompletionCallback* callback)
        :m_inputFileName(inputFileName),
        m_outputFileName(outputFileName),
        m_callback(callback),
        m_mode(mode),
        m_bCanceled(false)
    {
    }
    SortTask()
        :m_callback(nullptr),
        m_mode(0),
        m_bCanceled(false)
    {
    }
    void Run();
    void RunInternal();
    void Cancel();
};

struct SortJob : public ITaskCompletionCallback
{
    std::atomic<int> m_numCompletedTasks;
    std::map<std::string, std::shared_ptr<SortTask>> m_tasks;
    INotifyCallback* m_callback;
    thread_pool m_pool;
    SortJob(int numThreads, INotifyCallback* callback)
        :m_numCompletedTasks(0),
        m_callback(callback),
        m_pool(numThreads)
    {
    }
    void AddTask(std::string description, std::shared_ptr<SortTask> pst);
    void Wait();
    void Cancel();
    virtual void Completed();
};

class CFileProcessor
{
	CString m_inputPath;
	CString m_outputPath;
	int m_mode;
	INotifyCallback* m_callback;
	SortJob m_sd;
public:
	CFileProcessor(CString inputPath, CString outputPath, int mode, int numThreads, INotifyCallback *callback);
    void run();
    void stop();
};

