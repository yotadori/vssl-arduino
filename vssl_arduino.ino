// vssl_arduino.ino

#include "EspEasyTimer.h"
#include "src/Servo/Servo.h"
#include "src/Rot_Servo/Rot_Servo.h"
#include "src/Robot/Robot.h"
#include "src/Speaker/Speaker.h"
#include "src/Gyro/Gyro.h"

// XboxControllerReceiver のヘッダを追加（配置場所に合わせてパスを調整）
#include "src/XboxControllerReceiver/XboxControllerReceiver.h"

/*
constexpr int SERVO_PIN = D7;
constexpr int ROT_PIN_1 = D10;
constexpr int ROT_PIN_2 = D2;
constexpr int ROT_PIN_3 = D3;
constexpr int SPEAKER_PIN = D6;
*/

// AI module adapted circuit
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

// XboxControllerReceiver インスタンス
XboxControllerReceiver xboxReceiver;

// ボールセンサ
bool is_ball_on = true;

// PICスイッチ
bool is_switch_on = false;

// debug mode
bool is_debug_mode = false;

// dribble power（割り込みタスクで参照するためグローバルに保持）
int dribble_pow = 0;

// 割り込みの周期
float cycle = 1;

// 割り込み処理
void timer1Task()
{
  speaker.update();
  gyro.update();

  // dribble_pow は loop() で xboxReceiver から更新される
  // Robot::execute が内部で dribble を参照する実装であればそれを使う
  // ここでは robo.execute(cycle) のまま
  robo.execute(cycle);
}

// 割り込み用タイマー
EspEasyTimer timer1(TIMER_GROUP_0, TIMER_0);

void setup()
{
  delay(1000);

  // ハードウェア初期化
  gyro.setup();                    // IMUのセットアップ
  robo.setup();                    // ロボットのセットアップ
  robo.set_use_gyro(true);         // ロボットの制御にIMUを使う設定にする

  // タイマー開始
  timer1.begin(timer1Task, cycle); // 割り込み開始

  // XboxReceiver のセットアップ（Bluepad32 初期化・コールバック登録等を内部で行う）
  xboxReceiver.setup();

  // 音を鳴らす
  static Speaker::tone_type start_melody[]{{5, 50}, {5, 50}, {6, 50}, {Speaker::STOP, 0}};
  speaker.set_melody(start_melody);
}

void loop()
{
  // コントローラ入力を取得して内部状態を更新
  xboxReceiver.update();

  // 取得した目標速度・アクションをロボットに反映
  xyz_t v = xboxReceiver.vel();           // vel_.x, vel_.y, vel_.z
  bool kick = xboxReceiver.kick_flag();   // キックフラグ
  int new_dribble = xboxReceiver.dribble_pow(); // ドリブルパワー

  // 目標速度をセット（ロボット側の期待レンジに合わせて調整済み）
  robo.set_target_vel(v.x, v.y, v.z);

  // キック処理（ボタンが押されたら一度だけキックを発行する実装にする場合は
  // xboxReceiver 側でフラグの立て方を工夫するか、ここでエッジ検出を行う）
  if (kick) {
    robo.kick();
  }

  // ドリブルパワーをグローバルに反映（割り込みタスクや robo.execute 内で参照する想定）
  dribble_pow = new_dribble;

  // 必要ならロボット API に直接渡す（存在するなら）
  // 例: robo.set_dribble_power(dribble_pow);

  // ループの負荷を下げるため短い遅延
  delay(10);
}

