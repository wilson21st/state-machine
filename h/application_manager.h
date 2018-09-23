#ifndef __application_manager_h__
#define __application_manager_h__

#include <vector>
#include <boost/thread/thread.hpp>
#include "task.h"
#include "context.h"

class ApplicationManager {
  protected:
    // Single instance
    static ApplicationManager* instance_;
    std::vector<Task*> apps_;
    boost::thread_group threads_;
    Context context_;

  public:
    virtual ~ApplicationManager();
    static ApplicationManager* GetInstance();

    virtual Context* GetContext() { return &context_; }
    virtual void SetTransactionServer(const std::string& host, const std::string& port);

    virtual void Boot();
    virtual void RegisterApplication(Task* app) { apps_.push_back(app); }
    virtual void BroadCastMessage(const std::string& message);

    // Must be explicitly called before class destruction
    virtual void Join() { threads_.join_all(); }

  protected:
    ApplicationManager();
};

#endif // __application_manager_h__