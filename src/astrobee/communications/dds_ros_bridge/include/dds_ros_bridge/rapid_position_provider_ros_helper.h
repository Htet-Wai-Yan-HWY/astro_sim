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

#ifndef DDS_ROS_BRIDGE_RAPID_POSITION_PROVIDER_ROS_HELPER_H_
#define DDS_ROS_BRIDGE_RAPID_POSITION_PROVIDER_ROS_HELPER_H_

#include <ndds/ndds_cpp.h>

#include <string>

#include "dds_ros_bridge/util.h"

#include "ff_msgs/EkfState.h"

#include "rapidIo/PositionProvider.h"

namespace rapid {

/**
 * Provide a publisher which does not convert Quaternion to Rotation Matrix
 *   instead publish quaternion directly
 *   This publisher only does one copy into the RAPID data types
 */
class PositionProviderRosHelper : public PositionProvider {
 public:
  PositionProviderRosHelper(PositionTopicPairParameters const& params,
                            const std::string& entity_name);
  void Publish(const ff_msgs::EkfState::ConstPtr& pose_vel_cov);
};

}  // end namespace rapid

#endif  // DDS_ROS_BRIDGE_RAPID_POSITION_PROVIDER_ROS_HELPER_H_
