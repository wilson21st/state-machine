#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include "application_manager.h"
#include "state_application.h"
#include "async_application.h"
#include "logger.h"

void Quit(ApplicationManager* app_manager) {
  std::cout << "Quitting...\n";
  app_manager->BroadCastMessage("EvQuit");
}

// Test file
int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <server ip> <port>\n";
    return 1;
  }

  // Initialize
  Logger logger(argv[0]);
  ApplicationManager* app_manager = ApplicationManager::GetInstance();
  app_manager->SetTransactionServer(argv[1], argv[2]);

  // State application handles phase logic
  StateApplication state_app(app_manager);
  app_manager->RegisterApplication(&state_app);
  // Async application handles all asynchronous tasks called from state application
  AsyncApplication async_app(app_manager);
  app_manager->RegisterApplication(&async_app);

  // Assign threads to each application and boot them up
  app_manager->Boot();

  // Bind the CTRL-C signal to local function 'void Quit(ApplicationManager* app_manager)'
  boost::asio::io_service io;
  boost::asio::signal_set signals(io, SIGINT);
  signals.async_wait(boost::bind(&Quit, app_manager));
  boost::thread(boost::bind(&boost::asio::io_service::run, &io)).detach();

  app_manager->BroadCastMessage("EvStart");

  app_manager->Join();
}