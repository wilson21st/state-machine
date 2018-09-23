#ifndef __event_dispatcher_h__
#define __event_dispatcher_h__

#include <string>
#include <hash_map>

template <typename T>
class EventDispatcher {
  public:
    typedef bool (T::*EventProcess)(const std::string& event_name);

  protected:
    stdext::hash_map<std::string, EventProcess> events_;

  public:
    // Register function pointer and event name.
    bool Register(const std::string& event_name, EventProcess func_ptr) {
      // Find function pointer.
      auto itr = events_.find(event_name);

      if (itr == events_.end()) {
        // If the item of this key is not found ...
        events_.insert(stdext::hash_map<std::string, EventProcess>::value_type(event_name, func_ptr));
        return false;
      } else {
        // If the item of this key is found ...
        (*itr).second = func_ptr;
        return true;
      }
    }

    // Dispatch message and call corresponding function.
    bool Dispatch(const std::string& event_name, typename T* owner) {
    // Find function pointer.
      auto itr = events_.find(event_name);

    if (itr != events_.end()) {
      // If key is not found in hash, the result will be same as end().
      EventProcess funcp = (*itr).second;
      // T::OnXxxxxx(event_name);
      (owner->*funcp)(event_name);
      return true;
    }
    return false;
  };
};

#endif //__event_dispatcher_h__