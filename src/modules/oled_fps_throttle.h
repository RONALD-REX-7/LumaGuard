#ifndef OLED_FPS_THROTTLE_H
#define OLED_FPS_THROTTLE_H

#include <stdint.h>

typedef struct {
    uint32_t last_frame_ms;
    uint16_t frame_interval_ms;
} OledFpsThrottle;

static inline void oled_fps_init(OledFpsThrottle *throttle, uint16_t interval_ms) {
    throttle->last_frame_ms = 0;
    throttle->frame_interval_ms = interval_ms;
}

static inline uint8_t oled_fps_should_render(OledFpsThrottle *throttle, uint32_t current_ms) {
    if ((current_ms - throttle->last_frame_ms) >= throttle->frame_interval_ms) {
        throttle->last_frame_ms = current_ms;
        return 1;
    }
    return 0;
}

#endif /* OLED_FPS_THROTTLE_H */
