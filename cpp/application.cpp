#include "application.h"
#include "logger.h"

Application::Application(ApplicationManager* app_manager)
    : app_manager_(app_manager),
      context_(app_manager->GetContext()),
      quit_(false) {
  dispatcher_.Register("EvStart", reinterpret_cast<EventProcess>(&Application::OnStart));
  dispatcher_.Register("EvQuit",  reinterpret_cast<EventProcess>(&Application::OnQuit));
}

void Application::Run() {
  try {
    MessageLoop();
  } catch (std::exception e) {
    std::stringstream ss;
    ss << "Unexpected Event: " << e.what();
    Logger::WriteError(ss.str());
    return;
  }
}

void Application::MessageLoop() {
  do {
    std::string message;
    message_queue_.WaitMessage(message);
    Dispatch(message);
  } while (!quit_);
}