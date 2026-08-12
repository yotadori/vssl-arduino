// Udp.cpp

#include "Udp_Receiver.h"

Udp_Receiver::Udp_Receiver(char *ssid, char *password) : Receiver(), ssid_{ssid}, password_{password}, udp_{}, kick_flag_time_(0) {}

void Udp_Receiver::setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_, password_);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
    }

    // address 224.4.34.4
    // port 10004
    if(udp_.listenMulticast(IPAddress(224, 4, 23, 4), 10004)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp_.onPacket([this](AsyncUDPPacket packet) {
            // データを受信したときの処理 
            /*
            Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            */
            //reply to the client
            //packet.printf("Got %u bytes of data", packet.length());

            if (packet.length() < 11) return;
            uint8_t *data = packet.data();
            const unsigned int id = 0;
            if ((data[0] & 0b1111) == id + 1) {
                // 自分のidだったら値を更新
                update_data(data); 
            }
        });
        //Send multicast
        //udp_.print("Hello!");
    }
}

void Udp_Receiver::update() {
    // hold kick_flag for 300ms
    if (millis() > kick_flag_time_ + 300) {
        kick_flag_ = false;
    }
}

bool Udp_Receiver::isConnected() {
    return (WiFi.isConnected());
}

void Udp_Receiver::update_data(uint8_t* data) {
    // 時刻を取得
    this->last_updated_time_ = millis();

    const auto velocity = data[1] << 8 | data[2];
    const auto direction = data[3] << 8 | data[4];

    // 速度指令にかける倍率
    static constexpr float linear_coef = 1.0;
    static constexpr float angular_coef = 1.0;

    // mm/s, mm/s, rad/s
    vel_.y =
        -1 * linear_coef * velocity * std::cos(2.0 * M_PI * direction / 65535);
    vel_.x =
        linear_coef * velocity * std::sin(2.0 * M_PI * direction / 65535);
    vel_.z = angular_coef * ((data[0] & 0x80) == 0 ? 1 : -1) *
                                    (data[5] << 8 | data[6]) / 1000.0;

    dribble_pow_ = (int)data[7] - 3;

    if((data[0] >> 5) & 0x01) {
        kick_flag_ = true;
        this->kick_flag_time_ = millis();
        if((0x01 & (data[0] >> 4)) == 1) {
            // line
            const auto line = true;
        }
        else {
            // chip
            const auto chip = true;
        }
    }

}