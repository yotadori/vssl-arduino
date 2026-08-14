#ifndef XBOX_CONTROLLER_RECEIVER_H
#define XBOX_CONTROLLER_RECEIVER_H

#include "../Receiver/Receiver.h"
#include <Bluepad32.h>

class XboxControllerReceiver : public Receiver {
public:
    XboxControllerReceiver();

    // Receiver の仮想関数をオーバーライド
    void setup() override;
    void update() override;

private:
    // 内部処理用ヘルパー
    void processController(ControllerPtr ctl);
    void processControllers();

    // 設定
    int deadzone_ = 30;            // スティックのデッドゾーン
    float scale_ = 100.0f / 512.0f; // -511..512 -> -100..100 のスケール

    // 使うコントローラ（最初に見つかったものを使う）
    ControllerPtr activeController_;
};

#endif // XBOX_CONTROLLER_RECEIVER_H

