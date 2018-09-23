#ifndef __logger_h__
#define __logger_h__

#include <iostream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions/filter.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/support/date_time.hpp>

class Logger {
  public:
    Logger(const std::string& file_name) {
      std::stringstream ss;
      ss << file_name << ".log";
      boost::log::add_common_attributes();
      boost::log::add_file_log(ss.str(), boost::log::keywords::format = (boost::log::expressions::stream
        << "["
        << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
        << "] [" << boost::log::trivial::severity
        << "] " << boost::log::expressions::smessage));
    }

    static void WriteInfo(const std::string& message)  {
      BOOST_LOG_TRIVIAL(info) << message;
      std::cout << message << "\n"; // debug
    }

    static void WriteError(const std::string& message) {
      BOOST_LOG_TRIVIAL(error) << message;
      std::cout << message << "\n"; // debug
    }
};

#endif // __logger_h__