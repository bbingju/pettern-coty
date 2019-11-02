#ifndef TEMPMODE_H
#define TEMPMODE_H

class TempMode {
public:
    enum TEMP_MODE {
        COLD,
        HOT,
    };

    TempMode(): _mode(COLD), _target_cold(25.0f),
                _target_hot(37.0f),
                _current_target(_target_cold) {}

    void toggle() {
        _mode = isColdMode() ? HOT : COLD;
        _current_target = (_mode == COLD) ? _target_cold : _target_hot;
    }

    bool isHotMode() {
        return (_mode == HOT);
    }

    bool isColdMode() {
        return (_mode == COLD);
    }

    float getTarget() {
        return _current_target;
    }

    void setTarget(enum TEMP_MODE mode, float target) {
        if (mode == COLD)
            _target_cold = target;
        else
            _target_hot = target;
    }

private:
    enum TEMP_MODE _mode;
    float _target_cold;
    float _target_hot;
    float _current_target;
};

#endif /* TEMPMODE_H */
