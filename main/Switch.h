#ifndef SWITCH_H
#define SWITCH_H

class Switch {

public:
    Switch(int pin);
    void on();
    void off();
    int state();

private:
    int _pin;
    int _state;

    void update();
};

#endif /* SWITCH_H */
