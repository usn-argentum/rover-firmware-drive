#include <Arduino.h>

#include <Janus.h>

Hardware::PWMMotor left_wheel_motor(2, 3, 4);
Driver::ESCON50Driver left_wheel_driver(&left_wheel_motor, 0.1, 0.9, 12);

Hardware::PWMMotor right_wheel_motor(5, 6, 7);
Driver::ESCON50Driver right_wheel_driver(&right_wheel_motor, 0.1, 0.9, 12, );

unsigned int steering_tick_rate = 20; // 20ms -> 50Hz, should be plenty
Hardware::OpenCRSerialDynamixel steering_dxls(&Serial1, 1000000);

void setup() {
  Serial.begin(115200);

  Hardware::init();
  Hardware::set_pwm_depth(12); // 12-bit pwm for extra s m o o t h n e s s :)

  left_wheel_driver.init();
  left_wheel_motor.arm();

  right_wheel_driver.init();
  right_wheel_motor.arm();

  steering_dxls.init();
  steering_dxls.arm();

  steering_dxls.motor_1.radians = 3.14;
  steering_dxls.motor_2.radians = 0;
  steering_dxls.motor_1.velocity = 3000.0f;
  steering_dxls.motor_2.velocity = 3000.0f;
  steering_dxls.motor_1.acceleration = 75000.0f;
  steering_dxls.motor_2.acceleration = 75000.0f;
}

void loop() {
  unsigned long t = millis();
  static unsigned long steering_last_tick;

  right_wheel_driver.set_speed(1.0);

  steering_dxls.update();

  if (t - steering_last_tick >= steering_tick_rate) {
    steering_last_tick = t;

    //steering_dxls.motor_1.radians = (t / 3000) % 6;
    steering_dxls.transmit_motor_state();

    /*dxl.motor_1.radians = min(M_TWOPI, max(0, (dxl.motor_1.radians + (random(100) / 100.0f) - 0.5f)));
    dxl.motor_1.velocity = 3000.0f;
    dxl.motor_1.acceleration = 75000.0f;
    dxl.transmit_motor_state();
    Serial.println(dxl.motor_1.radians);*/
  }
}
