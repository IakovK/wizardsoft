#pragma once
class INotifyCallback
{
public:
    virtual void CompletedTaskCount(int completedCount, int totalCount) = 0;
};

