#ifndef __async_application_h__
#define __async_application_h__

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "application.h"
#include "transaction.h"

class AsyncApplication : public Application {
  protected:
    std::vector<boost::thread> threads_;
    Transaction trans_;

  public:
    explicit AsyncApplication(ApplicationManager* app_manager);
    virtual ~AsyncApplication();

  protected:
    // Callback functions
    virtual bool OnWaitSystemOnline(const std::string& event_name);
    virtual bool OnWaitTransRcvd(const std::string& event_name);
    virtual bool OnWaitTransComplete(const std::string& event_name);
    virtual bool OnQuit(const std::string& event_name);
};

#endif //__async_application_h__