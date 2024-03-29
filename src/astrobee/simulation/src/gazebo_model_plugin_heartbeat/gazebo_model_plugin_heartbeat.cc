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
/**
 * A plugin that does nothing but publish a standard heartbeat.
 * 
 * Can create multiple with plugin_name tag.
 */

// Gazebo includes
#include <astrobee_gazebo/astrobee_gazebo.h>

// STL includes
#include <string>

namespace gazebo {

FF_DEFINE_LOGGER("gazebo_model_plugin_empty");

class GazeboModelPluginHeartbeat : public FreeFlyerModelPlugin {
 public:
  GazeboModelPluginHeartbeat() : FreeFlyerModelPlugin(
    "", "", true) {}

  ~GazeboModelPluginHeartbeat() {}

 protected:
  // Called when the plugin is loaded into the simulator
  void LoadCallback(NodeHandle& nh,
    physics::ModelPtr model, sdf::ElementPtr sdf) {
    }

  // Manage the extrinsics based on the sensor type
  bool ExtrinsicsCallback(geometry_msgs::TransformStamped const* tf) {
    return true;
  }
};

// Register this plugin with the simulator
GZ_REGISTER_MODEL_PLUGIN(GazeboModelPluginHeartbeat)

}   // namespace gazebo
