#ifndef __context_h__
#define __context_h__

#include <boost/system/system_error.hpp>

// Warning: this class is not thread-safe
// Class shared across applications
class Context {
  protected:
    std::string host_;
    std::string service_;
    std::string err_msg_;

  public:
    Context() {}
    void SetHost(const std::string& host) { host_ = host; }
    void SetService(const std::string& service) { service_ = service; }
    void SetErrorMessage(const std::string& err_msg) { err_msg_ = err_msg; }

    std::string GetHost() const { return host_; }
    std::string GetService() const { return service_; }
    std::string GetErrorMessage() const { return err_msg_; }
};

#endif // __context_h__