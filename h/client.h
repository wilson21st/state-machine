// This class is modified from blocking_tcp_client.cpp
// written by Christopher M. Kohlhoff
// it is not designed for commercial purposes
//
// Reference URL:
// http://www.boost.org/doc/libs/1_46_1/doc/html/boost_asio/example/timeouts/blocking_tcp_client.cpp
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef __client_h__
#define __client_h__

#include <boost/asio/connect.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/write.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

//----------------------------------------------------------------------

//
// This class manages socket timeouts by applying the concept of a deadline.
// Each asynchronous operation is given a deadline by which it must complete.
// Deadlines are enforced by an "actor" that persists for the lifetime of the
// client object:
//
//  +----------------+
//  |                |
//  | check_deadline |<---+
//  |                |    |
//  +----------------+    | async_wait()
//              |         |
//              +---------+
//
// If the actor determines that the deadline has expired, the socket is closed
// and any outstanding operations are consequently canceled. The socket
// operations themselves use boost::lambda function objects as completion
// handlers. For a given socket operation, the client object runs the
// io_service to block thread execution until the actor completes.
//
class Client {
  private:
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::deadline_timer deadline_;

  public:
    Client() : socket_(io_service_), deadline_(io_service_) {
      // No deadline is required until the first socket operation is started. We
      // set the deadline to positive infinity so that the actor takes no action
      // until a specific deadline is set.
      deadline_.expires_at(boost::posix_time::pos_infin);
      // Start the persistent actor that checks for deadline expiry.
      CheckDeadline();
    }

    void SetTimeout(int timeout) {
      // Set a deadline for the asynchronous operation. As a host name may
      // resolve to multiple endpoints, this function uses the composed operation
      // async_connect. The deadline applies to the entire operation, rather than
      // individual connection attempts.
      deadline_.expires_from_now(boost::posix_time::seconds(timeout));
    }

    void Connect(const std::string& host, const std::string& service) {
      // Resolve the host name and service to a list of endpoints.
      boost::asio::ip::tcp::resolver::query query(host, service);
      boost::asio::ip::tcp::resolver::iterator iter =
        boost::asio::ip::tcp::resolver(io_service_).resolve(query);

      // Set up the variable that receives the result of the asynchronous
      // operation. The error code is set to would_block to signal that the
      // operation is incomplete. Asio guarantees that its asynchronous
      // operations will never fail with would_block, so any other value in
      // ec indicates completion.
      boost::system::error_code ec = boost::asio::error::would_block;

      // Start the asynchronous operation itself. The boost::lambda function
      // object is used as a callback and will update the ec variable when the
      // operation completes. The blocking_udp_client.cpp example shows how you
      // can use boost::bind rather than boost::lambda.
      boost::asio::async_connect(socket_, iter, boost::lambda::var(ec) = boost::lambda::_1);

      // Block until the asynchronous operation has completed.
      do io_service_.run_one(); while (ec == boost::asio::error::would_block);

      // Determine whether a connection was successfully established. The
      // deadline actor may have had a chance to run and close our socket, even
      // though the connect operation notionally succeeded. Therefore we must
      // check whether the socket is still open before deciding if we succeeded
      // or failed.
      if (ec)
        throw boost::system::system_error(ec);

      if (!socket_.is_open())
        throw boost::system::system_error(boost::asio::error::timed_out);
    }

    void SendRequest(const std::string& request) {
      std::string data = request + "\n";

      // Set up the variable that receives the result of the asynchronous
      // operation. The error code is set to would_block to signal that the
      // operation is incomplete. Asio guarantees that its asynchronous
      // operations will never fail with would_block, so any other value in
      // ec indicates completion.
      boost::system::error_code ec = boost::asio::error::would_block;

      // Start the asynchronous operation itself. The boost::lambda function
      // object is used as a callback and will update the ec variable when the
      // operation completes. The blocking_udp_client.cpp example shows how you
      // can use boost::bind rather than boost::lambda.
      boost::asio::async_write(socket_,
                               boost::asio::buffer(data),
                               boost::lambda::var(ec) = boost::lambda::_1);

      // Block until the asynchronous operation has completed.
      do io_service_.run_one(); while (ec == boost::asio::error::would_block);

      if (ec)
        throw boost::system::system_error(ec);

      if (!socket_.is_open())
        throw boost::system::system_error(boost::asio::error::timed_out);
    }

    std::string GetResponse() {
      boost::asio::streambuf input_buffer;
      // Set up the variable that receives the result of the asynchronous
      // operation. The error code is set to would_block to signal that the
      // operation is incomplete. Asio guarantees that its asynchronous
      // operations will never fail with would_block, so any other value in
      // ec indicates completion.
      boost::system::error_code ec = boost::asio::error::would_block;

      // Start the asynchronous operation itself. The boost::lambda function
      // object is used as a callback and will update the ec variable when the
      // operation completes. The blocking_udp_client.cpp example shows how you
      // can use boost::bind rather than boost::lambda.
      boost::asio::async_read_until(socket_,
                                    input_buffer,
                                    '\n',
                                    boost::lambda::var(ec) = boost::lambda::_1);

      // Block until the asynchronous operation has completed.
      do io_service_.run_one(); while (ec == boost::asio::error::would_block);

      if (ec)
        throw boost::system::system_error(ec);

      if (!socket_.is_open())
        throw boost::system::system_error(boost::asio::error::timed_out);

      Close();

      std::string response;
      std::istream is(&input_buffer);
      std::getline(is, response);
      return response;
    }

    void Close() {
      // Close socket connection
      boost::system::error_code ignored_ec;
      socket_.close(ignored_ec);
    }

  private:
    void CheckDeadline() {
      // Check whether the deadline has passed. We compare the deadline against
      // the current time since a new asynchronous operation may have moved the
      // deadline before this actor had a chance to run.
      if (deadline_.expires_at() <= boost::asio::deadline_timer::traits_type::now())  {
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are canceled. This allows the blocked
        // connect(), read_line() or write_line() functions to return.
        Close();
        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        deadline_.expires_at(boost::posix_time::pos_infin);
      }
      // Put the actor back to sleep.
      deadline_.async_wait(boost::lambda::bind(&Client::CheckDeadline, this));
    }
};

#endif //__client_h__