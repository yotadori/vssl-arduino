// Rot_Servo.cpp

#include <Arduino.h>

#include "Rot_Servo.h"

Rot_Servo::Rot_Servo(int channel, int pin, float offset = 0) :
 channel_(channel), offset_(offset)
{
    // pwmのセットアップ
    // 50Hz, 8bit
    pinMode(pin, OUTPUT);
    ledcSetup(channel_, 50, 12);
    ledcAttachPin(pin, channel_);
}

void Rot_Servo::set_speed(float speed) {
    if (speed > 1) {
        speed = 1;
    } else if (speed < -1) {
        speed = -1;
    }

    // -1.0~1.0を6%~8.5%に変換
    constexpr float DUTY_MAX = 8.5;
    constexpr float DUTY_MIN = 6;
    int duty = ((speed + offset_) * (DUTY_MAX - DUTY_MIN) / 2.0 + (DUTY_MAX + DUTY_MIN) / 2.0) * 4095 / 100.0;
    ledcWrite(channel_, duty);
}

void Rot_Servo::set_duty(float duty) {
    ledcWrite(channel_, (int)(duty * 4095 / 100));
}

void Rot_Servo::stop() {
    ledcWrite(channel_, 0);
}