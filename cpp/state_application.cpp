#include "state_application.h"
#include "logger.h"

StateApplication::StateApplication(ApplicationManager* app_manager)
  : Application(app_manager), phase_(kPhaseStartup) {
  dispatcher_.Register("EvPhaseChanged",  reinterpret_cast<EventProcess>(&StateApplication::OnPhaseChanged));
  dispatcher_.Register("EvSystemStartUp", reinterpret_cast<EventProcess>(&StateApplication::OnSystemStartUp));
  dispatcher_.Register("EvTransRcvd",     reinterpret_cast<EventProcess>(&StateApplication::OnTransRcvd));
  dispatcher_.Register("EvTransComplete", reinterpret_cast<EventProcess>(&StateApplication::OnTransComplete));
  dispatcher_.Register("EvSystemError",   reinterpret_cast<EventProcess>(&StateApplication::OnSystemError));
}

void StateApplication::ProcessPhase() {
  switch (phase_) {
    case kPhaseStartup:
      PhaseStartUp();
      break;
    case kPhasePending:
      PhasePending();
      break;
    case kPhaseTransaction:
      PhaseTransaction();
      break;
  }
}

void StateApplication::ChangePhase(PhaseIndex phase) {
  if (phase_ != phase) {
    phase_ = phase;
    SendPhaseChanged();
  }
}

void StateApplication::PhaseStartUp() {
  Logger::WriteInfo("[Startup State]: Waiting system online...");
  // Message 'EvWaitSystemOnline' will be sent to other application
  BroadCastMessage("EvWaitSystemOnline");
  // Then waiting 'EvSystemStartUp' message
}

void StateApplication::PhasePending() {
  Logger::WriteInfo("[Pending State]: Waiting transaction received...");
  // Message 'EvWaitTransRcvd' will be sent to other application
  // that handles transaction input devices
  BroadCastMessage("EvWaitTransRcvd");
  // Then waiting 'EvTransRcvd' message
}

void StateApplication::PhaseTransaction() {
  // Send transaction to server
  Logger::WriteInfo("[Transaction State]: Sending transaction to server...");
  // Message 'EvWaitTransComplete' will be sent to other application
  // that sends transaction request to server and waits for the response
  BroadCastMessage("EvWaitTransComplete");
  // Then waiting 'EvTransComplete' or 'EvSystemError'
}

bool StateApplication::OnSystemStartUp(const std::string& event_name) {
  if (phase_ == kPhaseStartup) {
    Logger::WriteInfo("[Startup State]: System Online.");
    ChangePhase(kPhasePending);
  }
  return true;
}

// Transaction info can be stored in app_manager_->GetContext()
bool StateApplication::OnTransRcvd(const std::string& event_name) {
  if (phase_ == kPhasePending) {
    Logger::WriteInfo("[Pending State]: Transaction Received. Transaction Info: xxxxxx");
    ChangePhase(kPhaseTransaction);
  }
  return true;
}

bool StateApplication::OnTransComplete(const std::string& event_name) {
  if (phase_ == kPhaseTransaction) {
    Logger::WriteInfo("[Transaction State]: Transaction Complete.");
    ChangePhase(kPhasePending);
  }
  return true;
}

bool StateApplication::OnSystemError(const std::string& event_name) {
  if (phase_ == kPhaseTransaction) {
    std::stringstream ss;
    ss << "[Transaction State]: System Error: " << context_->GetErrorMessage();
    Logger::WriteError(ss.str());
    ChangePhase(kPhaseStartup);
  }
  return true;
}