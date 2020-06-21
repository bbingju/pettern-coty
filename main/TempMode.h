#ifndef TEMPMODE_H
#define TEMPMODE_H

#include <Preferences.h>

class TempMode {
public:
    enum TEMP_MODE {
        COLD,
        HOT,
    };

    TempMode(): _target_cold(25.0f), _target_hot(37.0f)
                {
                    _mode = readMode();
                    _pref.begin("tempmode", true);
                    _target_hot = _pref.getFloat("hot_mode_temp", _target_hot);
                    _target_cold = _pref.getFloat("cold_mode_temp", _target_cold);
                    printf("mode = %s, target %.1f\n", _mode ? "HOT" : "COLD", _mode ? _target_hot : _target_cold);
                    _pref.end();
                }

    void toggle() {
        _mode = isColdMode() ? HOT : COLD;
        saveMode(_mode);
    }

    bool isHotMode() {
        return (_mode == HOT);
    }

    bool isColdMode() {
        return (_mode == COLD);
    }

    float getTarget() {
        return _mode == COLD ? _target_cold : _target_hot;
    }

    void setTarget(TEMP_MODE mode, float target) {
        _pref.begin("tempmode", false);
        if (mode == COLD) {
            _pref.putFloat("cold_mode_temp", target);
            _target_cold = target;
        }
        else {
            _pref.putFloat("hot_mode_temp", target);
            _target_hot = target;    
        }
        _pref.end();            
    }

private:
    enum TEMP_MODE _mode;
    float _target_cold;
    float _target_hot;
    Preferences _pref;

    enum TEMP_MODE readMode() {
        _pref.begin("tempmode", true);
        char mode = _pref.getChar("mode", 'C');
        _pref.end();
        return mode == 'C' ? COLD : HOT;
    }

    void saveMode(enum TEMP_MODE mode) {
        _pref.begin("tempmode", false);
        _pref.putChar("mode", mode == COLD ? 'C' : 'H');
        _pref.end();         
    }
};

#endif /* TEMPMODE_H */
