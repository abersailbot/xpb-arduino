#ifndef COMPASS_H
#define COMPASS_H

class HMC6343 {
    public:
        void initialise();
        int get_bearing();
        int get_pitch();
        int get_roll();
        bool poll_data();
    private:
        int bearing;
        int pitch;
        int roll;
};

// Arduino style
extern HMC6343 Compass;

#endif
