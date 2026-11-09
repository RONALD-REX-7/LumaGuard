#ifndef BUZZER_PITCH_LOOKUP_H
#define BUZZER_PITCH_LOOKUP_H

#include <stdint.h>

typedef enum {
    ALARM_TIER_IDLE = 0,
    ALARM_TIER_PROXIMITY_WARN = 1,
    ALARM_TIER_PROXIMITY_CRITICAL = 2,
    ALARM_TIER_FLAME_HAZARD = 3
} AlarmSeverityTier;

static inline uint16_t get_alarm_tone_frequency_hz(AlarmSeverityTier tier) {
    switch (tier) {
        case ALARM_TIER_PROXIMITY_WARN:     return 880;   /* A5 note */
        case ALARM_TIER_PROXIMITY_CRITICAL: return 1760;  /* A6 note */
        case ALARM_TIER_FLAME_HAZARD:       return 2637;  /* E7 high urgent note */
        case ALARM_TIER_IDLE:
        default:                            return 0;
    }
}

#endif /* BUZZER_PITCH_LOOKUP_H */
