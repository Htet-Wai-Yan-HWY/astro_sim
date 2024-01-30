/* Copyright (c) 2017, United States Government, as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * 
 * All rights reserved.
 * 
 * The Astrobee platform is licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License. You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#ifndef FF_UTIL_FF_SERVICE_H_
#define FF_UTIL_FF_SERVICE_H_

// ROS includes
#include <ff_common/ff_ros.h>
#include <ff_util/ff_timer.h>

// C++ includes
#include <string>
#include <functional>
#include <memory>

namespace ff_util {

template < class ServiceSpec >
struct FreeFlyerService {
  FreeFlyerService() {
    request = std::make_shared<typename ServiceSpec::Request>();
    response = std::make_shared<typename ServiceSpec::Response>();
  }
  std::shared_ptr<typename ServiceSpec::Request> request;
  std::shared_ptr<typename ServiceSpec::Response> response;
};

///////////////////////////// SERVICE CLIENT CODE /////////////////////////////

// This is a simple wrapper around a ROS2 service client.

template < class ServiceSpec >
class FreeFlyerServiceClient {
 protected:
  enum State {
    WAITING_FOR_CREATE   = 0,   // Connect() has not been called
    WAITING_FOR_CONNECT  = 1,   // Waiting to connect to server
    WAITING_FOR_CALL     = 2    // Connection established, waiting on call
  };
  static constexpr double DEFAULT_TIMEOUT_CONNECTED   = 10.0;
  static constexpr double DEFAULT_POLL_DURATION       = 0.1;

 public:
  // Callback types
  typedef std::function < void (void) > ConnectedCallbackType;
  typedef std::function < void (void) > TimeoutCallbackType;

  // Setters for callbacks
  void SetTimeoutCallback(TimeoutCallbackType cb_timeout)          { cb_timeout_ = cb_timeout;     }
  void SetConnectedCallback(ConnectedCallbackType cb_connected)    { cb_connected_ = cb_connected; }

  // Setters for timeouts
  void SetConnectedTimeout(double to_connected)  { to_connected_ = to_connected; }

  // Constructor
  FreeFlyerServiceClient() : state_(WAITING_FOR_CREATE),
    to_connected_(DEFAULT_TIMEOUT_CONNECTED),
    to_poll_(DEFAULT_POLL_DURATION) {}

  // Destructor
  ~FreeFlyerServiceClient() {}

  bool Create(rclcpp::Node::SharedPtr node, std::string const& topic) {
    // Create a timer to poll to see if the server is ready
    timer_connected_.createTimer(to_connected_,
        std::bind(&FreeFlyerServiceClient::TimeoutCallback, this),
        node,
        true,
        false);
    timer_poll_.createTimer(to_poll_,
        std::bind(&FreeFlyerServiceClient::ConnectPollCallback, this),
        node,
        false,
        false);
    // Save the node handle and topic to support reconnects
    node_ = node;
    topic_ = topic;
    client_group_ = node->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

    return IsConnected();
  }

  // Check that the service is ready
  bool IsConnected() {
    if (exists())
      return true;
    ConnectPollCallback();
    return false;
  }

  bool Call(FreeFlyerService<ServiceSpec> & service) {
    return call(service);
  }

  bool Call(const typename ServiceSpec::Request & request, std::shared_ptr<typename ServiceSpec::Response> & response) {
    return call(request, response);
  }

  // ROS1 functions
  bool call(const typename ServiceSpec::Request & request, std::shared_ptr<typename ServiceSpec::Response> & response) {
    return call(std::make_shared<typename ServiceSpec::Request>(request), response);
  }
  bool call(const std::shared_ptr<typename ServiceSpec::Request> & request,
            std::shared_ptr<typename ServiceSpec::Response> & response) {
    if (IsConnected()) {
      auto result = service_client_->async_send_request(request);
      std::future_status status = result.wait_for((std::chrono::seconds)10);  // timeout to guarantee a graceful finish

      if (status == std::future_status::ready) {
        response = result.get();
        return true;
      }
    }
    return false;
  }

  bool call(FreeFlyerService<ServiceSpec> & service) {
    return call(service.request.get(), service.response);
  }

  bool exists() {
    if (service_client_.get() != NULL) {
      return service_client_->service_is_ready();
    }
    return false;
  }

  bool isValid() {
    return exists();
  }

  bool waitForExistence(std::shared_ptr<rclcpp::Duration> duration) {
    return waitForExistence(duration->seconds());
  }

  bool waitForExistence(double duration_sec) {
    if (service_client_.get() != NULL) {
      return service_client_->wait_for_service((std::chrono::duration<double>) duration_sec);
    }
    return false;
  }

 protected:
  // Simple wrapper around an optional timer
  void StartOptionalTimer(ff_util::FreeFlyerTimer & timer,
                          double duration) {
    if (duration == 0) return;
    timer.stop();
    timer.setPeriod(duration);
    timer.start();
  }

  // Called periodically until the server is ready
  void ConnectPollCallback() {
    // Case: connected
    if (service_client_.get() != NULL &&
        service_client_->service_is_ready()) {
      if (state_ != WAITING_FOR_CALL) {
        state_ = WAITING_FOR_CALL;
        timer_connected_.stop();
        timer_poll_.stop();
        if (cb_connected_)
          cb_connected_();
      }
    // Case: disconnected
    } else {
      service_client_ = node_->create_client<ServiceSpec>(topic_, rmw_qos_profile_services_default,
                                                                client_group_);
      state_ = WAITING_FOR_CONNECT;
      StartOptionalTimer(timer_connected_, to_connected_);
      StartOptionalTimer(timer_poll_, to_poll_);
    }
  }

  // Called when the service doesn't go active
  void TimeoutCallback() {
    timer_connected_.stop();
    timer_poll_.stop();
    if (cb_timeout_)
      cb_timeout_();
  }

 protected :
  State state_;
  double to_connected_;
  double to_poll_;
  TimeoutCallbackType cb_timeout_;
  ConnectedCallbackType cb_connected_;
  ServiceClient<ServiceSpec> service_client_;
  rclcpp::CallbackGroup::SharedPtr client_group_;
  ff_util::FreeFlyerTimer timer_connected_;
  ff_util::FreeFlyerTimer timer_poll_;
  std::string topic_;
  rclcpp::Node::SharedPtr node_;
};

}  // namespace ff_util

#endif  // FF_UTIL_FF_SERVICE_H_
