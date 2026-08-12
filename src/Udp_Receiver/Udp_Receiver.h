// Udp_Receiver.h
#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H 

#include "Arduino.h"

#include "WiFi.h"
#include "AsyncUDP.h"

#include "../Context/Context.h"

#include "../Receiver/Receiver.h"

class Udp_Receiver : public Receiver {
    public:
        Udp_Receiver(char* ssid, char* password);

        void setup() override;

        void update() override;

        bool isConnected();

    private:
        char * ssid_;
        char * password_;

        AsyncUDP udp_;

        // last kick_flag time
        int kick_flag_time_;

        /**
         * @brief 値を更新
        */
        void update_data(uint8_t* data);
};

#endif // UDP_RECEIVER_H