#include "async_application.h"
#include "logger.h"

AsyncApplication::AsyncApplication(ApplicationManager* app_manager)
  : Application(app_manager) {
  trans_.SetCaller(this);
  dispatcher_.Register("EvWaitSystemOnline",  reinterpret_cast<EventProcess>(&AsyncApplication::OnWaitSystemOnline));
  dispatcher_.Register("EvWaitTransRcvd",     reinterpret_cast<EventProcess>(&AsyncApplication::OnWaitTransRcvd));
  dispatcher_.Register("EvWaitTransComplete", reinterpret_cast<EventProcess>(&AsyncApplication::OnWaitTransComplete));
}

AsyncApplication::~AsyncApplication() {
  for (auto& thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

bool AsyncApplication::OnWaitSystemOnline(const std::string& event_name) {
  // Do something...
  BroadCastMessage("EvSystemStartUp");
  return true;
}

bool AsyncApplication::OnWaitTransRcvd(const std::string& event_name) {
  // Do something...
  BroadCastMessage("EvTransRcvd");
  return true;
}

bool AsyncApplication::OnWaitTransComplete(const std::string& event_name) {
  // Clear all existing running tasks
  trans_.Interrupt();
  for (auto& thread : threads_) {
    thread.detach();
  }
  threads_.clear();
  // Create a new thread verifying transaction
  threads_.push_back(boost::thread(boost::bind(&Transaction::Veriy, &trans_)));
  return true;
}

bool AsyncApplication::OnQuit(const std::string& event_name) {
  trans_.Interrupt();
  for (auto& thread : threads_) {
    thread.detach();
  }
  return __super::OnQuit(event_name);
}