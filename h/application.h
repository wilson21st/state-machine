#ifndef __application_h__
#define __application_h__

#include "task.h"
#include "application_manager.h"
#include "message_queue.h"
#include "event_dispatcher.h"

class Application : public Task {
  protected:
    bool quit_;
    MessageQueue message_queue_;
    Context* context_;

    EventDispatcher<Application> dispatcher_;
    typedef EventDispatcher<Application>::EventProcess EventProcess;

    ApplicationManager* app_manager_;

  public:
    explicit Application(ApplicationManager* app_manager);
    virtual ~Application() {}

    virtual Context* GetContext() const { return context_; }

    virtual void Run();
    virtual void IssueMessage(const std::string& message) { message_queue_.PutMessage(message); }
    virtual void BroadCastMessage(const std::string& message) { app_manager_->BroadCastMessage(message); }

  protected:
    virtual bool Dispatch(const std::string& event_name) { return dispatcher_.Dispatch(event_name, this); }
    void MessageLoop();

    // Callback functions
    virtual bool OnStart(const std::string& event_name) { return true; }
    virtual bool OnQuit(const std::string& event_name) { quit_ = true; return true; }
};

#endif // __application_h__