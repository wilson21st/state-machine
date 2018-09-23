#ifndef __task_h__
#define __task_h__

#include <string>

class Task {
  public:
    virtual void Run() = 0;
    virtual void IssueMessage(const std::string& message) = 0;
    virtual void BroadCastMessage(const std::string& message) = 0;
};

#endif // __task_h__