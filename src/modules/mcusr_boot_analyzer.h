#ifndef MCUSR_BOOT_ANALYZER_H
#define MCUSR_BOOT_ANALYZER_H

#include <stdint.h>

typedef struct {
    uint8_t power_on_reset;
    uint8_t external_reset;
    uint8_t brown_out_reset;
    uint8_t watchdog_reset;
} BootCauseFlags;

static inline BootCauseFlags decode_mcusr_register(uint8_t mcusr_value) {
    BootCauseFlags flags;
    flags.power_on_reset  = (mcusr_value & (1 << 0)) ? 1 : 0;
    flags.external_reset  = (mcusr_value & (1 << 1)) ? 1 : 0;
    flags.brown_out_reset = (mcusr_value & (1 << 2)) ? 1 : 0;
    flags.watchdog_reset  = (mcusr_value & (1 << 3)) ? 1 : 0;
    return flags;
}

#endif /* MCUSR_BOOT_ANALYZER_H */
