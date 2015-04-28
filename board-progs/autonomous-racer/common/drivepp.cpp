/* -*- mode: c++; c-basic-offset: 4; -*- */
#include "drivepp.hpp"

/* map, map, where art thou map */

drive::drive() {}

drive::drive(motor* left, motor* right, percent_t speed, Direction direction) {

    this->left = left;
    this->right = right;
    set(speed, direction);
}

void drive::stop() {

    left->stop();
    right->stop();
}

void drive::start() {

    left->start();
    right->start();
}

void drive::forward(percent_t speed) {

    left->set(speed, FORWARD);
    right->set(speed, FORWARD);
}

void drive::backward(percent_t speed) {

    left->set(speed, BACKWARD);
    right->set(speed, BACKWARD);
}

void drive::set(percent_t speed, Direction dir) {

    left->set(speed, dir);
    right->set(speed, dir);
}

void drive::steer(uint32_t left_sens, uint32_t left_front_sens,
                  uint32_t right_sens, uint32_t right_front_sens,
                  uint32_t back_sens) {

    /* todo: feed the lf/f, rf/r data here for porportional control of
     * the motors. the side with the larger coefficient slows more */

    percent_t race_speed = 30;
    Direction dir = FORWARD;

    percent_t left_speed = delta(left_sens, left_front_sens);
    percent_t right_speed = delta(right_sens, right_front_sens);

    /* todo: utilize \back_sens */

    /* optional: path-centering algorithm */

    /* left->set(race_speed*left_speed/100, dir); */
    /* right->set(race_speed*right_speed/100, dir); */

    if (left_sens > 1500) {
        left->set(0, dir);
        right->set(race_speed, dir);
    } else {
        left->set(race_speed);
        right->set(race_speed);
    }
}

/* Local Variables: */
/* firestarter: (compile "make -k -j32 -C ~/workspace/ee445m-labs/build/") */
/* End: */
