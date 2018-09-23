#ifndef __message_queue_h__
#define __message_queue_h__

#include <list>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

class MessageQueue {
  protected:
    struct VariableContainer {
      std::list<std::string> message_container;
      boost::recursive_mutex mutex;
      boost::condition condition_signal;
    };
    VariableContainer* var_;

  public:
    MessageQueue();
    explicit MessageQueue(const MessageQueue& source);
    virtual ~MessageQueue();

    void Clear();
    void PutMessage(const std::string& message);
    bool GetNextMessage(std::string& message);
    bool WaitMessage(std::string& message);

    virtual void PopMessage();

    MessageQueue& operator=(const MessageQueue&) { return *this; } // do nothing
};

#endif //__message_queue_h__