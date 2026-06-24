#include <Arduino.h>
#include <Janus.h>

PWMConfig pwm_config;

// 0 - 5000 rpm <=> 10% - 90% pwm duty cycle
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
OpenCRDynamixelMotor steering_left(0, 5500.0f, 70000.0f, &opencr_brigde);
OpenCRDynamixelMotor steering_right(1, 5500.0f, 70000.0f, &opencr_brigde);

bool armed = true;
float control_axis_1;
float control_axis_2;
byte crc = 0;

const float dxl_turn_angle = 13.0f * 1.0f;
const float dxl_angle_low = -dxl_turn_angle * (M_PI / 180.0f);
const float dxl_angle_high = dxl_turn_angle * (M_PI / 180.0f);

void setup() {
    Serial.begin(115200);
    pwm_config.set_resolution(12);
    
    opencr_brigde.init();
    steering_left.init();
    steering_right.init();

    left_motor.init();
    right_motor.init();
    
    left_motor.set_enable(true);
    right_motor.set_enable(true);
    opencr_brigde.send_arm(true);
}

void loop() {
    unsigned long t = millis();

    static unsigned long last_throttle_control;
    static unsigned long last_dxl_control;
    static unsigned long last_dxl_arm;

    if(Serial.available() > 0) {
      bool arm;
      float axis_1;
      float axis_2;

      //Serial.readBytes((char*)&c, sizeof(c));
      Serial.readBytes((char*)&arm, sizeof(arm));
      Serial.readBytes((char*)&axis_1, sizeof(axis_1));
      Serial.readBytes((char*)&axis_2, sizeof(axis_2));  
      
      control_axis_1 = max(min(axis_1, 1.0f), -1.0f);
      control_axis_2 = max(min(axis_2, 1.0f), -1.0f);
    }

    opencr_brigde.update();
    left_motor.set_enable(true);
    right_motor.set_enable(true);

    // ~100Hz motor control.
    if (t - last_throttle_control >= 10) {
      last_throttle_control = t;
      left_motor.set_rpm(-control_axis_2 * escon_config.max_rpm());
      right_motor.set_rpm(control_axis_2 * escon_config.max_rpm());
    }

    //TODO: only send this if the armed status has actually changed
    if (t - last_dxl_arm >= 500) {
      last_dxl_arm = t;
      Serial.print("CRC: "); Serial.println(crc);
      Serial.print("Stick axis 1: "); Serial.println(control_axis_1);
      Serial.print("Stick axis 2: "); Serial.println(control_axis_2);
      Serial.print("Left motor RPM target: "); Serial.println(left_motor.get_rpm());
      Serial.print("Right motor RPM target: "); Serial.println(right_motor.get_rpm());
      Serial.print("Left steer angle target: "); Serial.println(steering_left.get_position() * (180.0f / M_PI));
      Serial.print("Right steer angle target: "); Serial.println(steering_right.get_position() * (180.0f / M_PI));      
    }

    // ~33Hz steering, should be higher.
    //TODO: increase OpenCR efficiency
    if (t - last_dxl_control >= 30) {
        last_dxl_control = t;
        
        float steering_angle = dxl_angle_low + ((control_axis_1 + 1) / 2) * (dxl_angle_high - dxl_angle_low);
        steering_left.set_position(M_PI + steering_angle);
        steering_right.set_position(M_PI - steering_angle);
        
        steering_left.update_bridge();
        steering_right.update_bridge();
        opencr_brigde.send_motors();
    }

    //unsigned long elapsed = micros() - start_t;
    //Serial.print("Loop time: "); Serial.println(elapsed);
    //Serial.print("Load%: "); Serial.println(100.0f * (elapsed / 1000.0f) / 10.0f);
}
