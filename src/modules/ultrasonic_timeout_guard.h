#ifndef ULTRASONIC_TIMEOUT_GUARD_H
#define ULTRASONIC_TIMEOUT_GUARD_H

#include <stdint.h>

#define ULTRASONIC_MAX_ECHO_US 30000UL
#define ULTRASONIC_MIN_ECHO_US 150UL

static inline uint8_t is_echo_duration_valid(uint32_t echo_us) {
    return (echo_us >= ULTRASONIC_MIN_ECHO_US && echo_us <= ULTRASONIC_MAX_ECHO_US);
}

static inline uint16_t echo_us_to_distance_cm(uint32_t echo_us) {
    if (!is_echo_duration_valid(echo_us)) return 0;
    return (uint16_t)(echo_us / 58UL);
}

#endif /* ULTRASONIC_TIMEOUT_GUARD_H */
