#pragma once

#include <pebble.h>

typedef enum {
  ColorTypeBackground = 0,
  ColorTypeRing,
  ColorTypeDotInactive,
  ColorTypeDotActive,

  ColorTypeCount
} ColorType;

void data_init();

void data_deinit();

void data_set_color(ColorType type, GColor color);

GColor data_get_color(ColorType type);
