#ifndef TEMPMODE_H
#define TEMPMODE_H

class TempMode {
public:
    enum TEMP_MODE {
        COLD,
        HOT,
    };

    TempMode(): _mode(COLD), _target_temp_cold(25.0f),
                _target_temp_hot(37.0f),
                _current_target_temp(_target_temp_cold) {}

    void toggle() {
        _mode = isColdMode() ? HOT : COLD;
        _current_target_temp = (_mode == COLD) ? _target_temp_cold : _target_temp_hot;
    }

    bool isHotMode() {
        return (_mode == HOT);
    }

    bool isColdMode() {
        return (_mode == COLD);
    }

    float getTargetTemp() {
        return _current_target_temp;
    }

    void setTargetTemp(enum TEMP_MODE mode, float target) {
        if (mode == COLD)
            _target_temp_cold = target;
        else
            _target_temp_hot = target;
    }

private:
    enum TEMP_MODE _mode;
    float _target_temp_cold;
    float _target_temp_hot;
    float &_current_target_temp;
};

#endif /* TEMPMODE_H */
