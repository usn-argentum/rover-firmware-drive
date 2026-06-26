#pragma once

#include "drive.h"
#include <Hermes.h>

Context* ros_ctx = nullptr;
Node* drive_node = nullptr; 

Publisher<std_msgs__msg__Int32>* heartbeat_pub = nullptr;
std_msgs__msg__Int32 heartbeat_msg;
Timer* heartbeat_timer = nullptr;

Subscriber<std_msgs__msg__Float32>* left_motor_speed_sub = nullptr;
Subscriber<std_msgs__msg__Float32>* right_motor_speed_sub = nullptr;
Subscriber<std_msgs__msg__Float32>* right_angle_sub = nullptr;
Subscriber<std_msgs__msg__Float32>* left_angle_sub = nullptr;

void on_timer_heartbeat(rcl_timer_t* tmr, int64_t last_call) {
  heartbeat_msg.data += 1;
  heartbeat_pub->publish(heartbeat_msg);
}

namespace Comm {
  void init() {
    digitalWrite(LED_BUILTIN, HIGH);
    
    set_microros_serial_transports(Serial);

    delay(2000);

    // try to ping ros
    while (rmw_uros_ping_agent(250, 3) != RMW_RET_OK) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(250);
    }

    ros_ctx = new Context();
    // 8 handles, last parameter isnt implemented but should allow domain id changes soon(tm)
    if (!ros_ctx->init(8, 0)) {
      SERIAL_TELEMETRY.println("Failed to init ROS context");
      while(true) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(100);
      }
    }

    // we use one node for everything
    drive_node = new Node(*ros_ctx, "drive_node", "");

    heartbeat_pub = new Publisher<std_msgs__msg__Int32>(
      *drive_node, 
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), 
      "heartbeat"
    );

    left_motor_speed_sub = new Subscriber<std_msgs__msg__Float32>(
      *drive_node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
      "left_motor_speed",
      on_left_motor_speed
    );

    right_motor_speed_sub = new Subscriber<std_msgs__msg__Float32>(
      *drive_node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
      "right_motor_speed",
      on_right_motor_speed
    );

    left_angle_sub = new Subscriber<std_msgs__msg__Float32>(
      *drive_node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
      "left_steering_angle",
      on_left_angle
    );

    right_angle_sub = new Subscriber<std_msgs__msg__Float32>(
      *drive_node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
      "right_steering_angle",
      on_right_angle
    );

    
    //heartbeat_timer = new Timer(*drive_node, 1000, on_timer_heartbeat);
    //heartbeat_timer->start();
    
    digitalWrite(LED_BUILTIN, LOW);
  }

  void update() {
    ros_ctx->spin(1000000); // 1ms timeout
  }
}
