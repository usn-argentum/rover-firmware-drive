#pragma once

#include <Janus.h>

const float dxl_turn_angle = 13.0f * 2.0f;
const float dxl_angle_low = -dxl_turn_angle * (M_PI / 180.0f);
const float dxl_angle_high = dxl_turn_angle * (M_PI / 180.0f);

PWMConfig pwm_config;

//TimerConfig timer_config;

// 0 -  rpm <=> 10% - 90% pwm duty cycle
Escon50Config escon_config(0.0f, 5000.0f, 0.1f, 0.9f);

constexpr unsigned int left_esc_dir = 3;
constexpr unsigned int left_esc_enable = 2;
constexpr unsigned int left_esc_pwm = 4;

constexpr unsigned int right_esc_dir = 6;
constexpr unsigned int right_esc_enable = 5;
constexpr unsigned int right_esc_pwm = 7;

EsconPWMMotor left_motor(left_esc_dir, left_esc_enable, left_esc_pwm, &escon_config, &pwm_config);
EsconPWMMotor right_motor(right_esc_dir, right_esc_enable, right_esc_pwm, &escon_config, &pwm_config);

OpenCRDynamixelBridge opencr_brigde(&Serial1, 1000000);
OpenCRDynamixelMotor steering_left(0, 2500.0f, 10000.0f, &opencr_brigde);
OpenCRDynamixelMotor steering_right(1, 2500.0f, 10000.0f, &opencr_brigde);

bool do_arm = false;
// should receive a value from ros + timeout / watchdog style disable when data dissapears.
// for now, just guess
// disables motors at 0% throttle
// disables steering 

namespace Drive {
  void init() {
    pwm_config.set_resolution(12);

    opencr_brigde.init();
    steering_left.init();
    steering_right.init();
    
    left_motor.init();
    right_motor.init();
    
    left_motor.set_enable(true);
    right_motor.set_enable(true);
    opencr_brigde.send_arm(true);

    //timer_config.init(25, timer_callback);
  }

  void update() {
    unsigned long time = millis();
    static unsigned long last_steer_update;
    static unsigned long last_speed_update;
    static unsigned long last_bridge_update;

    // ~33Hz OpenCR bridge update rate 
    if (time - last_bridge_update >= 30) {  
      last_bridge_update = time;
      opencr_brigde.update();
    }
    
    // ~50Hz steering update rate, only really updated when opencr_bridge.update() gets called but the larp is real
    if (time - last_steer_update >= 20) {
      last_steer_update = time;
      
      float left_steering_angle = dxl_angle_low + ((left_steer_axis + 1) / 2) * (dxl_angle_high - dxl_angle_low);
      float right_steering_angle = dxl_angle_low + ((right_steer_axis + 1) / 2) * (dxl_angle_high - dxl_angle_low);
      
      left_motor.set_enable(true);
      steering_left.set_position(M_PI + left_steering_angle);
      right_motor.set_enable(true);
      steering_right.set_position(M_PI + right_steering_angle);
          
      steering_left.update_bridge();
      steering_right.update_bridge();
      opencr_brigde.send_motors();
    }
    
    // 100Hz motor update rate
    if (time - last_speed_update >= 10) {
      last_speed_update = time;

      if(do_arm) {
        left_motor.set_enable(left_speed_axis != 0.0f);
        right_motor.set_enable(right_speed_axis != 0.0f);
      }

      left_motor.set_rpm(-left_speed_axis * escon_config.max_rpm());
      right_motor.set_rpm(right_speed_axis * escon_config.max_rpm());
    }
  }
}