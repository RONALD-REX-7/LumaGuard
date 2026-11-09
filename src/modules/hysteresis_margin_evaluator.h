#ifndef HYSTERESIS_MARGIN_EVALUATOR_H
#define HYSTERESIS_MARGIN_EVALUATOR_H

#include <stdint.h>

typedef struct {
    uint16_t lower_threshold_cm;
    uint16_t upper_threshold_cm;
    uint8_t is_alert_active;
} HysteresisState;

static inline void hysteresis_init(HysteresisState *state, uint16_t lower_cm, uint16_t upper_cm) {
    state->lower_threshold_cm = lower_cm;
    state->upper_threshold_cm = upper_cm;
    state->is_alert_active = 0;
}

static inline uint8_t hysteresis_evaluate(HysteresisState *state, uint16_t current_cm) {
    if (current_cm <= state->lower_threshold_cm) {
        state->is_alert_active = 1;
    } else if (current_cm >= state->upper_threshold_cm) {
        state->is_alert_active = 0;
    }
    return state->is_alert_active;
}

#endif /* HYSTERESIS_MARGIN_EVALUATOR_H */
