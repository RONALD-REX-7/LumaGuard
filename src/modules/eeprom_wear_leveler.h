#ifndef EEPROM_WEAR_LEVELER_H
#define EEPROM_WEAR_LEVELER_H

#include <stdint.h>

#define EEPROM_SLOT_COUNT 16
#define EEPROM_START_OFFSET 32

static inline uint16_t get_next_eeprom_slot_address(uint8_t current_index) {
    return EEPROM_START_OFFSET + ((current_index % EEPROM_SLOT_COUNT) * 4);
}

static inline uint8_t advance_eeprom_slot(uint8_t current_index) {
    return (current_index + 1) % EEPROM_SLOT_COUNT;
}

#endif /* EEPROM_WEAR_LEVELER_H */
