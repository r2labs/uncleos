/* -*- mode: c++; c-basic-offset: 4; -*- */
#ifndef __servo__
#define __servo__

#include <stdint.h>

#include "criticalpp.hpp"

#define SERVO_DEFAULT_PWM_PERIOD 40000
typedef uint32_t memory_address_t;

class servo : public critical {
private:
    memory_address_t pwm_base;
    memory_address_t pwm_pin;
    memory_address_t pwm_gen;
    memory_address_t pwm_out;

    uint32_t pwm_period;
    uint32_t duty_period;

    void pwm_init(void);

    bool enabled;

public:
    servo();

    servo(memory_address_t pwm_base, memory_address_t pwm_gen,
          memory_address_t pwm_out);

    /*! Cut all power to the motor. */
    void stop(void);

    /*! Enable power to the motor. */
    void start(void);

    /*! Set the motor speed. */
    uint32_t set(uint32_t pwm_clocks);

    uint32_t get(void);

    const static uint16_t pwm_max_period = SERVO_DEFAULT_PWM_PERIOD;
};

#endif  /* __servo__ */
