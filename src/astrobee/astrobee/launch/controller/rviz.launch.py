# Copyright (c) 2017, United States Government, as represented by the
# Administrator of the National Aeronautics and Space Administration.
#
# All rights reserved.
#
# The Astrobee platform is licensed under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with the
# License. You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

import os

from ament_index_python import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node


def generate_launch_description():

    return LaunchDescription([
        DeclareLaunchArgument("rviz_file", default_value=[LaunchConfiguration("world"), "_ros2.rviz"]),
        Node(
            package="rviz2",
            namespace="",
            executable="rviz2",
            name="rviz_node",
            arguments=["-d", PathJoinSubstitution([get_package_share_directory("astrobee"),"resources/rviz",LaunchConfiguration("rviz_file")])],
            respawn=True,
        ),
    ])
