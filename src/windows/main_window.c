#include "main_window.h"

#define MINUTES_RADIUS       PBL_IF_ROUND_ELSE(60, 60)
#define HOURS_RADIUS         3
#define INSET                PBL_IF_ROUND_ELSE(5, 3)
;

static Window *s_window;
static Layer *s_canvas;

static int s_hours, s_minutes;

static int32_t get_angle_for_hour(int hour) { 
  return (hour * 360) / 12; 
}

static int32_t get_angle_for_minute(int minute) { 
  return (minute * 360) / 60; 
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // 12 hours only, with a minimum size
  s_hours -= (s_hours > 12) ? 12 : 0;

  // Minutes are expanding circle arc
  int minute_angle = get_angle_for_minute(s_minutes);
  GRect frame = grect_inset(bounds, GEdgeInsets(4 * INSET));
  graphics_context_set_fill_color(ctx, data_get_color(ColorTypeRing));
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 20, 0, DEG_TO_TRIGANGLE(minute_angle));

  // Hours are dots
  frame = grect_inset(frame, GEdgeInsets(3 * HOURS_RADIUS));
  for(int i = 0; i < 12; i++) {
    int hour_angle = get_angle_for_hour(i);
    GPoint pos = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(hour_angle));

    graphics_context_set_fill_color(ctx, i <= s_hours ? data_get_color(ColorTypeDotActive) : data_get_color(ColorTypeDotInactive));
    graphics_fill_circle(ctx, pos, HOURS_RADIUS);
  }
}

static void tick_handler(struct tm *time_now, TimeUnits changed) {
  s_hours = time_now->tm_hour;
  s_minutes = time_now->tm_min;
  layer_mark_dirty(s_canvas);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, layer_update_proc);
  layer_add_child(window_layer, s_canvas);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);

  window_destroy(s_window);
}

void main_window_push() {
  s_window = window_create();
  window_set_background_color(s_window, data_get_color(ColorTypeBackground));
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  time_t now = time(NULL);
  struct tm *time_now = localtime(&now);
  tick_handler(time_now, MINUTE_UNIT);
}

void main_window_reload_config() {
  window_set_background_color(s_window, data_get_color(ColorTypeBackground));
  layer_mark_dirty(s_canvas);
}
