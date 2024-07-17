#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdlib.h>
#include <usbd_customhid.h>

#include "stm32h7xx_hal.h"
#include "keymap.h"


// Maximum number of simultaneously pressed keys
#define ROLLOVER 6

// PS/2 special scancodes
#define PREFIX_CODE 0xe0
#define BREAK_CODE 0xf0

// Size of the HID report in bytes
#define KHIDSZ 8

struct keyboard_hid_t
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[ROLLOVER];
};

#endif
