#include "EspEasyTimer.h"
#include "src/Servo/Servo.h"
#include "src/Rot_Servo/Rot_Servo.h"
#include "src/Robot/Robot.h"
#include "src/Speaker/Speaker.h"
#include "src/Gyro/Gyro.h"

/*
constexpr int SERVO_PIN = D7;
constexpr int ROT_PIN_1 = D10;
constexpr int ROT_PIN_2 = D2;
constexpr int ROT_PIN_3 = D3;
constexpr int SPEAKER_PIN = D6;
*/

// AI module adapted circuit
// ピン番号の設定
constexpr int SERVO_PIN = D3;
constexpr int ROT_PIN_1 = D1;
constexpr int ROT_PIN_2 = D2;
constexpr int ROT_PIN_3 = D10;
constexpr int SPEAKER_PIN = D0;
constexpr int US_TRIG_PIN = D6;
constexpr int US_ECHO_PIN = D7;

// サーボモーター（キック用モーター）
Servo servo0 = Servo(0, SERVO_PIN, 0);
// 連続回転サーボモーター（ホイールのモーター）
Rot_Servo rot1 = Rot_Servo(1, ROT_PIN_1, 0);
Rot_Servo rot2 = Rot_Servo(2, ROT_PIN_2, 0);
Rot_Servo rot3 = Rot_Servo(3, ROT_PIN_3, 0);

// ジャイロセンサー
Gyro gyro = Gyro();

// ロボット
Robot robo = Robot(rot1, rot2, rot3, servo0, gyro);

// スピーカー
Speaker speaker = Speaker(4, SPEAKER_PIN);

//Udp_Receiver udp_receiver = Udp_Receiver("yota-HP-OmniBook", "yotakunhappy");
//Udp_Receiver udp_receiver = Udp_Receiver("KIKS2f-g", "516a6a9041c41");


// ボールセンサ
bool is_ball_on = true;

// PICスイッチ
bool is_switch_on = false;

// debug mode
bool is_debug_mode = false;

// dribble power
int dribble_pow = 0;

// 割り込みの周期
float cycle = 1;

// 割り込み処理
void timer1Task()
{
  speaker.update();
  gyro.update();
  robo.execute(cycle);
}

// 割り込み用タイマー
EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup()
{
  delay(1000);
  gyro.setup();                    // IMUのセットアップ
  robo.setup();                    // ロボットのセットアップ
  robo.set_use_gyro(true);         // ロボットの制御にIMUを使う設定にする
  timer1.begin(timer1Task, cycle); // 割り込み開始

  // 音を鳴らす
  static Speaker::tone_type start_melody[]{{5, 50}, {5, 50}, {6, 50}, {Speaker::STOP, 0}};
  speaker.set_melody(start_melody);
}

void loop()
{
  robo.set_target_vel(0, 0, 0);
  robo.kick();
  delay(1000);
}

