#ifndef __transaction_h__
#define __transaction_h__

#include "application.h"
#include "client.h"

// Warning: this class is not thread-safe
class Transaction {
  protected:
    Application* app_;
    Client client_;

  public:
    Transaction() : app_(NULL) {}
    virtual ~Transaction() { Interrupt(); }

    virtual void SetCaller(Application* app) { app_ = app; }

    virtual void Veriy() {
      try {
        client_.SetTimeout(30);
        client_.Connect(app_->GetContext()->GetHost(),
                        app_->GetContext()->GetService());
        client_.SendRequest("POST /verify HTTP/1.1\r\n");
        std::string response = client_.GetResponse();
        // Check response content
        // ...
        // If success
        app_->BroadCastMessage("EvTransComplete");
      } catch (std::exception& e) {
        app_->GetContext()->SetErrorMessage(e.what());
        app_->BroadCastMessage("EvSystemError");
      }
    }

    virtual void Interrupt() { client_.Close(); }
};

#endif //__transaction_h__