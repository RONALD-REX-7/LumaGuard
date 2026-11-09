#ifndef FLAME_PERSISTENCE_FILTER_H
#define FLAME_PERSISTENCE_FILTER_H

#include <stdint.h>

typedef struct {
    uint8_t current_count;
    uint8_t required_threshold;
    uint8_t is_confirmed;
} FlamePersistenceState;

static inline void flame_filter_init(FlamePersistenceState *state, uint8_t threshold) {
    state->current_count = 0;
    state->required_threshold = threshold;
    state->is_confirmed = 0;
}

static inline uint8_t flame_filter_update(FlamePersistenceState *state, uint8_t raw_reading_active) {
    if (raw_reading_active) {
        if (state->current_count < state->required_threshold) {
            state->current_count++;
        }
        if (state->current_count >= state->required_threshold) {
            state->is_confirmed = 1;
        }
    } else {
        state->current_count = 0;
        state->is_confirmed = 0;
    }
    return state->is_confirmed;
}

#endif /* FLAME_PERSISTENCE_FILTER_H */
