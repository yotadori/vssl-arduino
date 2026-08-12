// Gyro.h
#ifndef GYRO_H
#define GYRO_H

#include "../Context/Context.h"

// 加速度センサの値を管理するクラス
class Gyro {
    public:
        /**
         * @brief コンストラクタ
        */
        Gyro();

        void setup();

        /**
         * @brief 値を更新
        */
        void update();
        
        /**
         * @brief 加速度を取得
        */
        xyz_t acc();

        /**
         * @brief 重力成分除去後の加速度を取得
        */
        xyz_t acc_filtered();

        /**
         * @brief 角速度取得
        */
        xyz_t gyro();
        
    private:
        xyz_t acc_; // 加速度
        xyz_t acc_filtered_; // 加速度（重力成分除去）
        xyz_t gyro_; // 角速度
        xyz_t drift_; // ジャイロドリフト成分
        float tmp_; // 温度

        // 重力成分
        xyz_t gravity_;
};

#endif // SERVO_H