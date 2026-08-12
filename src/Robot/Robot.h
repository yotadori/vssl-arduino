// Robot.h
#ifndef ROBOT_H
#define ROBOT_H 

#include "../Context/Context.h"
#include "../Rot_Servo/Rot_Servo.h"
#include "../Servo/Servo.h"
#include "../Speaker/Speaker.h"
#include "../Gyro/Gyro.h"

// ロボットの状態を管理するクラス
class Robot {
    public:
        // ロボットの半径（タイヤ部分）(mm)
        constexpr static float RADIUS = 30;
        // 最大速度(mm/s)
        constexpr static float MAX_SPEED = 420;
        // 最大角速度(rad/s)
        constexpr static float MAX_OMEGA = 5;

        /**
         * @brief コンストラクタ
         * @param rot1 モーター1
         * @param rot2 モーター2
         * @param rot3 モーター3
         * @param servo キック用サーボモーター
         * @param gyro ジャイロ
        */
        Robot(Rot_Servo& rot1, Rot_Servo& rot2, Rot_Servo& rot3, Servo& servo, Gyro& gyro);

        /**
         * @brief setup （setupで呼ぶ）
         */
        void setup();

        /**
         * @brief 制御値を更新（一定周期で呼ぶこと）
         * @param cycle 周期
        */
        void execute(float cycle);

        /**
         * @brief キック
         */
        void kick();

        /**
         * @brief 足回りのモーターをすべて停止
         */
        void stop();

        /**
         * @brief 指定した距離だけ移動
         * @param x ロボット正面方向(mm)
         * @param y ロボット左手方向(mm)
         */
        void move_mm(float x, float y);

        /**
         * @brief 指定した角度だけ回転
         * @param deg 角度（度）
         */
        void turn_degree(float deg);

        //// 以下 setter ////
        
        /**
         * @brief 目標速度を設定
         */
        void set_target_vel(xyz_t target_vel);

        /**
         * @brief 目標速度を設定
         * @param vx ロボット前方向の速度(mm/s)
         * @param vy ロボット左方向の速度(mm/s)
         * @param vz 角速度(rad/s)
         */
        void set_target_vel(float vx, float vy, float omega);

        /**
         * @brief move_mm用の速さを設定
         * @param speed 速さ(mm)
         */
        void set_default_speed(float speed);

        /**
         * @brief turn_deg用の速さを設定
         * @param omega 角速度(rad/s)
         */
        void set_default_omega(float omega);

        /**
         * @brief ジャイロを使うかを設定
         * @param flag 使うならtrue
         */
        void set_use_gyro(boolean flag);

        /**
         * @brief キック中か
         */
        bool kicking();

    private:
        xyz_t target_vel_; // 目標速度
        float last_omega_error_; // 角度の誤差
        float omega_error_integral_; // 角度の誤差の積分

        xyz_t vel_; // 推定速度 (x, y, angular)

        // モーター
        Rot_Servo& rot1_;
        Rot_Servo& rot2_;
        Rot_Servo& rot3_;

        // キック用モーター
        Servo& servo_;

        // ジャイロ
        Gyro& gyro_;
        boolean use_gyro_;

        bool kicking_; // キック動作中か
        unsigned int kick_count_;

        // move_mm用スピード
        float default_speed_;
        float default_omega_;
};

#endif // ROBO_OLD_H