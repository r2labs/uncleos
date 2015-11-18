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

    uint32_t current_duty, start_duty, end_duty;
    uint32_t ms_counter, ms_smooth;

    void pwm_init(void);

    bool enabled;

    int32_t clock_div;

    const static uint16_t num_steps = 50;

public:
    uint32_t min_duty, max_duty, rest_duty;

    servo();

    servo(memory_address_t pwm_base, memory_address_t pwm_gen,
          memory_address_t pwm_out, uint32_t min,
          uint32_t max, uint32_t rest);

    /*! Cut all power to the motor. */
    void stop(void);

    /*! Enable power to the motor. */
    void start(void);

    /*! Step the motor a smoothed amount. */
    void step(void);

    /*! Force the motor speed. */
    uint32_t force(uint32_t pw);

    /*! Set the motor speed. */
    uint32_t set(uint32_t pw, bool force = false);

    uint32_t get(void);

    const static uint16_t pwm_max_period = SERVO_DEFAULT_PWM_PERIOD;
};

#endif  /* __servo__ */
