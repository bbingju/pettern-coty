#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

class TemperatureSensor {

    const float Ra{0.000936741098911303};
    const float Rb{0.000269694251459242};
    const float Rc{0.000000191091994778876};

public:
    TemperatureSensor();

    float read();
    float readAV();

};

#endif /* TEMPERATURESENSOR_H */
