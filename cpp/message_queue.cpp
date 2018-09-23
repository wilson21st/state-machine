#include "message_queue.h"

MessageQueue::MessageQueue() : var_(new VariableContainer) {
}

MessageQueue::MessageQueue(const MessageQueue& source) : var_(new VariableContainer) {
  var_->message_container = source.var_->message_container;
}

MessageQueue::~MessageQueue() {
  delete var_;
  var_ = NULL;
}

// Clear message container.
void MessageQueue::Clear() {
  boost::lock_guard<boost::recursive_mutex> guard(var_->mutex);
  var_->message_container.clear();
}

// Put message to message container.
void MessageQueue::PutMessage(const std::string& message) {
  if (var_ != NULL) {
    boost::lock_guard<boost::recursive_mutex> guard(var_->mutex);
    var_->message_container.push_back(message);
    var_->condition_signal.notify_one();
  }
}

// Get next message from the queue.
bool MessageQueue::GetNextMessage(std::string& message) {
  if (var_ != NULL) {
    boost::lock_guard<boost::recursive_mutex> guard(var_->mutex);
    if (var_->message_container.size() > 0) {
      message = *var_->message_container.begin();
      var_->message_container.pop_front();
      return true;
    } else {
      message.clear();
      return false;
    }
  }
  return false;
}

// Wait until next message is issued.
bool MessageQueue::WaitMessage(std::string& message) {
  while (!GetNextMessage(message)) {
    if (var_ == NULL) {
      return false;
    }
    var_->condition_signal.wait(var_->mutex);
  }
  return true;
}

// Remove current message from message queue.
void MessageQueue::PopMessage() {
  if (var_ != NULL) {
    boost::lock_guard<boost::recursive_mutex> guard(var_->mutex);
    if (var_->message_container.size() > 0) {
      var_->message_container.pop_front();
    }
  }
}