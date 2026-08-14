// XboxControllerReceiver.cpp

#include "XboxControllerReceiver.h"
#include <Arduino.h>

// Bluepad32 のサンプルと同等のコールバック実装をこのファイル内に閉じる
static ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// コールバック（接続）
static void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n",
                          ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

// コールバック（切断）
static void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }
    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

// コンストラクタ
XboxControllerReceiver::XboxControllerReceiver() {
    vel_ = {0, 0, 0};
    kick_flag_ = false;
    dribble_pow_ = 0;
    last_updated_time_ = 0;
    activeController_ = nullptr;

    // myControllers を初期化
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) myControllers[i] = nullptr;
}

// setup: Bluepad32 の初期化とコールバック登録を行う（ここで forgetBluetoothKeys も呼ぶ）
void XboxControllerReceiver::setup() {
    Serial.begin(115200);
    Serial.printf("XboxControllerReceiver: firmware=%s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %02X:%02X:%02X:%02X:%02X:%02X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Bluepad32 のセットアップ（コールバックを渡す）
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // 必要ならペアリング情報を消す（コメントアウト可）
    // BP32.forgetBluetoothKeys();

    // 仮想マウスを無効（必要なら true に）
    BP32.enableVirtualDevice(false);

    Serial.println("XboxControllerReceiver: setup complete");
}

// 内部: 単一コントローラの入力をロボット用データに変換して更新
void XboxControllerReceiver::processController(ControllerPtr ctl) {
    if (!ctl) return;

    // デッドゾーン適用ラムダ
    auto applyDeadzone = [&](int v)->int {
        if (abs(v) < deadzone_) return 0;
        return v;
    };

    // 左スティック = 移動
    int lx = applyDeadzone(ctl->axisX());   // -511..512
    int ly = applyDeadzone(ctl->axisY());   // -511..512

    // 右スティック X = 回転
    int rx = applyDeadzone(ctl->axisRX());

    // スケーリング（-511..512 -> -100..100）
    vel_.x = -ly * scale_ * 3.0;
    vel_.y = -lx * scale_ * 3.0; // Y 軸反転（必要に応じて変更）
    vel_.z = -rx * scale_ / 30.0;

    // RB RT LB LT ボタンでキック
    kick_flag_ = ctl->r1() || ctl->r2() || ctl->l1() || ctl->l2();

    if (kick_flag_)
    {
        // コントローラーのバイブレーション
        ctl->playDualRumble(0 /* delayedStartMs */, 250 /* durationMs */, 0x80 /* weakMagnitude */, 0x40 /* strongMagnitude */);
    }

    last_updated_time_ = millis();

    // デバッグ出力（必要なら有効）
    // Serial.printf("vel: %.1f, %.1f, %.1f, kick=%d, dribble=%d\n", vel_.x, vel_.y, vel_.z, kick_flag_, dribble_pow_);
}

// 内部: myControllers 配列を走査して最初に見つかったゲームパッドを処理
void XboxControllerReceiver::processControllers() {
    activeController_ = nullptr;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        ControllerPtr c = myControllers[i];
        if (c && c->isConnected() && c->hasData() && c->isGamepad()) {
            activeController_ = c;
            break;
        }
    }

    if (activeController_) {
        processController(activeController_);
    } else {
        // コントローラが無ければゼロクリア（安全のため）
        vel_ = {0, 0, 0};
        kick_flag_ = false;
        dribble_pow_ = 0;
    }
}

// update: 毎ループで呼ぶ。BP32.update() を呼び、コントローラ処理を行う
void XboxControllerReceiver::update() {
    // BP32.update() はここで一度だけ呼ぶ（他で呼ばないこと）
    bool dataUpdated = BP32.update();
    if (dataUpdated) {
        processControllers();
    } else {
        // 更新が無くても、接続状態の変化を拾いたい場合は processControllers() を常に呼ぶ設計も可能
        // processControllers();
    }

    // 必要なら短い遅延はここでは入れない（呼び出し側のループで制御）
}

