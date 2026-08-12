// Receiver.h
#ifndef RECEIVER_H
#define RECEIVER_H 

#include "../Context/Context.h"

class Receiver {
    public:
        Receiver();

        virtual void setup();

        virtual void update();

        xyz_t vel();

        bool kick_flag();

        int dribble_pow();

        float updated_time();

    protected:
        xyz_t vel_;
        bool kick_flag_;        
        int dribble_pow_;
        unsigned long last_updated_time_;
};

#endif // RECEIVER_H