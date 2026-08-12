// Rot_Servo.h
#ifndef ROT_SERVO_H
#define ROT_SERVO_H

// 連続回転サーボモータを制御するためのクラス
class Rot_Servo {
    public:
        /**
         * @brief コンストラクタ
         * @param channel pwmのチャンネル
         * @param pin ピン
        */
        Rot_Servo(int channel, int pin, float offset);

        /**
         * @brief 速度をセット
         * @param speed -1.0~1.0 
        */
        void set_speed(float speed);

        /**
         * @brief duty比をセット
         * @param duty duty(%) 
        */
        void set_duty(float duty);

        /**
         * @brief 力を抜いて止める
        */
        void stop();

    private:
        // pwmのチャンネル
        int channel_;
        float offset_;

};

#endif // ROT_SERVO_H