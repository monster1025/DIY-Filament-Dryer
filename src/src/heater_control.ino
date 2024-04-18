#include "GyverPID.h"

GyverPID regulator(PID_KP, PID_KI, PID_KD);

void setup_heater(){
    regulator.setDirection(NORMAL);
    //regulator.setLimits(0, 1023);
    regulator.setpoint = LIMIT_TEMP_HEATER_MIN;
}

void change_target_temperature(int temperature){
    regulator.setpoint = temperature;
}

void current_temperature_updated(float temperature){
    regulator.input = temperature;
}

void task_heater() {
    if (box_status == 0){
        set_heater_duty(HEATER_DUTY_OFF);
        return;
    }

    int pwm_value = 0;
    if (temperature_heater > max_temperature_heater || temperature_in > max_temperature_heater){
        Serial.println("Max temperature reached. Setting off heater!");
        pwm_value = 0;
    }else{
        pwm_value = regulator.getResult();
    };

    #if DEF_DEBUG_PID
        Serial.print("current pid value: ");
        Serial.println(pwm_value);
    #endif

    set_heater_duty(pwm_value);
}

void set_heater_duty(uint8_t duty)
{
    fan_duty = duty;
    analogWrite(LED_PIN, 255-duty);
}


