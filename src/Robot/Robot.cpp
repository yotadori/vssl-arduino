#include "Robot.h"

Robot::Robot(Rot_Servo& rot1, Rot_Servo& rot2, Rot_Servo& rot3, Servo& servo, Gyro& gyro)
   :
    target_vel_{0},
    last_omega_error_(0),
    omega_error_integral_(0),
    vel_{0},
    rot1_{rot1},
    rot2_{rot2},
    rot3_{rot3},
    servo_{servo},
    gyro_{gyro},
    kicking_(false),
    kick_count_(0),
    default_speed_(100),
    default_omega_(3),
    use_gyro_(false)
{}

void Robot::setup() {
  servo_.set_angle(-90);
  delay(500);
  servo_.stop();
  rot1_.set_speed(0);
  rot2_.set_speed(0);
  rot3_.set_speed(0);
  gyro_.setup();
}

void Robot::execute(float cycle) {

  auto clamp = [](float a, float limit) {
    if (a > limit) {
      a = limit;
    } else if (a < -limit) {
      a = -limit;
    }
    return a;
  };
   
  // 出力速度
  xyz_t out_vel = target_vel_;

  if (use_gyro_)
  {
    // 目標角速度エラー
    float omega_error = target_vel_.z - gyro_.gyro().x;

    // 積分
    omega_error_integral_ += omega_error * cycle;
    // 微分
    float omega_error_diff = (omega_error - last_omega_error_) / cycle;

    //Serial.printf(">omega_error:%f\n", (float)omega_error);

    // 角速度をフィードバック（PID）
    const float k_p = 0.1;
    const float k_i = 0.02;
    const float k_d = 0.01;
    out_vel.z += k_p * omega_error + k_i * omega_error_integral_ + k_d * omega_error_diff;

    last_omega_error_ = omega_error;
  }

  // 速度調整用係数
  constexpr float c = 1.0;
 
  // 速度調整
  out_vel.x = clamp(c * out_vel.x, Robot::MAX_SPEED);
  out_vel.y = clamp(c * out_vel.y, Robot::MAX_SPEED);
  out_vel.z = clamp(c * out_vel.z, Robot::MAX_OMEGA);

  // サーボへの出力に変換
  rot1_.set_speed((0.866 * out_vel.x + 0.500 * out_vel.y + Robot::RADIUS * out_vel.z) / Robot::MAX_SPEED);
  rot2_.set_speed((0.000 * out_vel.x - 1.000 * out_vel.y + Robot::RADIUS * out_vel.z) / Robot::MAX_SPEED);
  rot3_.set_speed((-0.866 * out_vel.x + 0.500 * out_vel.y + Robot::RADIUS * out_vel.z) / Robot::MAX_SPEED);

  // キック動作
  if (kicking_)
  {
    if (kick_count_ < 170)
    {
      servo_.set_angle(-45);
    }
    else if (kick_count_ < 220)
    {
      servo_.set_angle(-90);
    }
    else
    {
      servo_.stop();
      kicking_ = false;
    }
    kick_count_++;
  }
}

void Robot::kick()
{
  if (!kicking_) {
    kick_count_ = 0;
    kicking_ = true;
  }
}

void Robot::stop() {
  target_vel_ = {0, 0, 0};
  servo_.stop();
  rot1_.stop();
  rot2_.stop();
  rot3_.stop();
}

void Robot::move_mm(float x, float y) {
  target_vel_.z = 0;
  float distance = sqrt(x * x + y * y);
  target_vel_.x = default_speed_ * x / distance;
  target_vel_.y = default_speed_ * y / distance;
  delay(distance / default_speed_ * 1000);
  target_vel_.x = 0;
  target_vel_.y = 0;
}

void Robot::turn_degree(float deg) {
  float rad = deg * PI / 180.0;
  target_vel_.z = rad > 0 ? default_omega_: -default_omega_;
  delay(abs(rad) / default_omega_ * 1000);
  target_vel_.z = 0;
}

void Robot::set_target_vel(xyz_t target_vel) {
  target_vel_ = target_vel;
}

void Robot::set_target_vel(float vx, float vy, float omega) {
  target_vel_ = xyz_t{vx, vy, omega};
}

void Robot::set_default_speed(float speed) {
  default_speed_ = min(speed, Robot::MAX_SPEED);
}

void Robot::set_default_omega(float omega) {
  default_omega_ = omega;
}

void Robot::set_use_gyro(boolean flag) {
  use_gyro_ = flag;
}

bool Robot::kicking() {
  return kicking_;
}