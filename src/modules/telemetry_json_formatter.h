#ifndef TELEMETRY_JSON_FORMATTER_H
#define TELEMETRY_JSON_FORMATTER_H

#include <stdint.h>
#include <stdio.h>

static inline int format_telemetry_packet(char *buffer, size_t buf_len, uint16_t dist_cm, uint8_t flame_active, uint32_t uptime_s) {
    return snprintf(buffer, buf_len,
        "{\"dist_cm\":%u,\"flame\":%u,\"uptime\":%lu}",
        dist_cm, flame_active ? 1 : 0, uptime_s);
}

#endif /* TELEMETRY_JSON_FORMATTER_H */
