#ifndef __state_application_h__
#define __state_application_h__

#include "application.h"

class StateApplication : public Application {
  protected:
    enum PhaseIndex {
      kPhaseStartup = 0,
      kPhasePending,
      kPhaseTransaction,
    };
    PhaseIndex phase_;

  public:
    explicit StateApplication(ApplicationManager* app_manager);
    virtual ~StateApplication() {}

  protected:
    virtual void ChangePhase(PhaseIndex phase);
    virtual void ProcessPhase();
    virtual void SendPhaseChanged() { IssueMessage("EvPhaseChanged"); }

    virtual void PhaseStartUp();
    virtual void PhasePending();
    virtual void PhaseTransaction();

    // Callback functions
    virtual bool OnStart(const std::string& event_name)        { ProcessPhase(); return true; }
    virtual bool OnPhaseChanged(const std::string& event_name) { ProcessPhase(); return true; }
    virtual bool OnSystemStartUp(const std::string& event_name);
    virtual bool OnTransRcvd(const std::string& event_name);
    virtual bool OnTransComplete(const std::string& event_name);
    virtual bool OnSystemError(const std::string& event_name);
};

#endif // __state_application_h__