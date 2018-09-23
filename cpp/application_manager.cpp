#include "application_manager.h"

ApplicationManager* ApplicationManager::instance_ = NULL;

ApplicationManager::ApplicationManager() {
  if (instance_ == NULL) {
    instance_ = this;
  }
}

ApplicationManager::~ApplicationManager() {
  if (instance_ == this) {
    instance_ = NULL;
  }
}

void ApplicationManager::SetTransactionServer(const std::string& host, const std::string& port) {
  context_.SetHost(host);
  context_.SetService(port);
}

void ApplicationManager::Boot() {
  for (Task* app : apps_) {
    if (app != NULL) {
      threads_.create_thread(boost::bind(&Task::Run, app));
    }
  }
}

void ApplicationManager::BroadCastMessage(const std::string& message) {
  for (Task* app : apps_) {
    if (app != NULL) {
      app->IssueMessage(message);
    }
  }
}

ApplicationManager* ApplicationManager::GetInstance() {
  if (instance_ == NULL) {
    boost::mutex mutex;
    boost::lock_guard<boost::mutex> guard(mutex);
    static ApplicationManager instance;
  }
  return instance_;
}