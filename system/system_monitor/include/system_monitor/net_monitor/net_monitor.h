#ifndef SYSTEM_MONITOR_NET_MONITOR_NET_MONITOR_H
#define SYSTEM_MONITOR_NET_MONITOR_NET_MONITOR_H
/*
 * Copyright 2020 Autoware Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file net_monitor.h
 * @brief Net monitor class
 */

#include <diagnostic_updater/diagnostic_updater.h>
#include <system_monitor/net_monitor/nl80211.h>
#include <map>
#include <string>
#include <vector>

#define toMbit(X) (static_cast<float>(X) / 1000000 * 8)

/**
 * @brief Bytes information
 */
typedef struct bytes
{
  unsigned int rx_bytes;  //!< @brief total bytes received
  unsigned int tx_bytes;  //!< @brief total bytes transmitted

  bytes() : rx_bytes(0), tx_bytes(0) {}
} bytes;

class NetMonitor
{
public:
  /**
   * @brief constructor
   * @param [in] nh node handle to access global parameters
   * @param [in] pnh node handle to access private parameters
   */
  NetMonitor(const ros::NodeHandle & nh, const ros::NodeHandle & pnh);

  /**
   * @brief main loop
   */
  void run(void);

protected:
  using DiagStatus = diagnostic_msgs::DiagnosticStatus;

  /**
   * @brief check CPU usage
   * @param [out] stat diagnostic message passed directly to diagnostic publish calls
   * @note NOLINT syntax is needed since diagnostic_updater asks for a non-const reference
   * to pass diagnostic message updated in this function to diagnostic publish calls.
   */
  void checkUsage(
    diagnostic_updater::DiagnosticStatusWrapper & stat);  // NOLINT(runtime/references)

  /**
   * @brief get wireless speed
   * @param [in] ifa_name interface name
   * @return wireless speed
   */
  float getWirelessSpeed(const char * ifa_name);

  ros::NodeHandle nh_;                   //!< @brief ros node handle
  ros::NodeHandle pnh_;                  //!< @brief private ros node handle
  diagnostic_updater::Updater updater_;  //!< @brief Updater class which advertises to /diagnostics

  char hostname_[HOST_NAME_MAX + 1];        //!< @brief host name
  std::map<std::string, bytes> bytes_;      //!< @brief list of bytes
  ros::Time last_update_time_;              //!< @brief last update time
  std::vector<std::string> device_params_;  //!< @brief list of devices
  NL80211 nl80211_;                         // !< @brief 802.11 netlink-based interface

  float usage_warn_;  //!< @brief Memory usage(%) to generate warning

  /**
   * @brief Network usage status messages
   */
  const std::map<int, const char *> usage_dict_ = {
    {DiagStatus::OK, "OK"}, {DiagStatus::WARN, "high load"}, {DiagStatus::ERROR, "down"}};
};

#endif  // SYSTEM_MONITOR_NET_MONITOR_NET_MONITOR_H
