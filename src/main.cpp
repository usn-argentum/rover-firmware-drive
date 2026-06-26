#include <Arduino.h>
#include <Janus.h>
#include <Hermes.h>
#include <std_msgs/msg/float32.h>

#define SERIAL_TELEMETRY Serial3

bool activity_led;
unsigned long activity_led_timeout;

float left_speed_axis;
float right_speed_axis;
float left_steer_axis;
float right_steer_axis;

void on_left_motor_speed(const std_msgs__msg__Float32* msg) {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  float command = msg->data;
  left_speed_axis = command;
}

void on_right_motor_speed(const std_msgs__msg__Float32* msg) {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  float command = msg->data;
  right_speed_axis = command;
}

void on_left_angle(const std_msgs__msg__Float32* msg) {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  float command = msg->data;
  left_steer_axis = command;
}

void on_right_angle(const std_msgs__msg__Float32* msg) {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  float command = msg->data;
  right_steer_axis = command;
}

void timer_callback() {
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

#include "drive.h"
#include "comm.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  //SERIAL_TELEMETRY.begin(115200);
  Drive::init();
  Comm::init();
}

void loop() {
  //unsigned long time = millis();
  //static unsigned long last_s;

  Comm::update();
  Drive::update();

  /*if (time - last_serial_telemetry >= 1000) {
    //blip_act_led(0);
    last_serial_telemetry = time;
    
    SERIAL_TELEMETRY.print("Left steer axis:          "); SERIAL_TELEMETRY.println(left_steer_axis);
    SERIAL_TELEMETRY.print("Left steer angle target:  "); SERIAL_TELEMETRY.println(steering_left.get_position() * (180.0f / M_PI));
    SERIAL_TELEMETRY.print("Right steer axis:         "); SERIAL_TELEMETRY.println(right_steer_axis);
    SERIAL_TELEMETRY.print("Right steer angle target: "); SERIAL_TELEMETRY.println(steering_right.get_position() * (180.0f / M_PI));      
    SERIAL_TELEMETRY.print("Left speed axis:          "); SERIAL_TELEMETRY.println(left_speed_axis);
    SERIAL_TELEMETRY.print("Left motor RPM target:    "); SERIAL_TELEMETRY.println(left_motor.get_rpm());
    SERIAL_TELEMETRY.print("Right speed axis:         "); SERIAL_TELEMETRY.println(right_speed_axis);
    SERIAL_TELEMETRY.print("Right motor RPM target:   "); SERIAL_TELEMETRY.println(right_motor.get_rpm());
  }*/
}
