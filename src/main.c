#include <pebble.h>

static Window *window;
static TextLayer *time_layer;
static TextLayer *comment_layer;
static BitmapLayer *battery_layer;
static GBitmap *battery_bitmap;

static char *intro[] = {
  "What the hell, it's",
  "Pretty damn sure it's",
  "I swear it's",
  "Hell if I know but it's",
  "Apparently it's",
  "Against all odds it's",
  "I think it's",
  "Don't blame me, it's"
};

static char *minutes[] = {
  "exactly",
  "five past",
  "ten past",
  "quarter past",
  "half past",
  "quarter to",
  "ten to",
  "five to"
};

static char *hours[] = {
  "twelve","one","two","three","four","five",
  "six","seven","eight","nine","ten","eleven"
};

static char *comments[] = {
  "Drink some damn water",
  "Why the hell are you awake",
  "Bad decisions were made",
  "Time is fake anyway",
  "Your brain needs rebooting",
  "Sleep would be smart",
  "Weekend chaos",
  "Monday can go to hell"
};

static void update_time() {

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  int intro_i = rand() % 8;
  int min_i = rand() % 8;
  int comment_i = rand() % 8;

  int hour = tick_time->tm_hour % 12;

  static char buffer[64];

  snprintf(buffer, sizeof(buffer),
           "%s\n%s %s",
           intro[intro_i],
           minutes[min_i],
           hours[hour]);

  text_layer_set_text(time_layer, buffer);
  text_layer_set_text(comment_layer, comments[comment_i]);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  time_layer = text_layer_create(GRect(0, 20, bounds.size.w, 80));
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  comment_layer = text_layer_create(GRect(0, 110, bounds.size.w, 30));
  text_layer_set_font(comment_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(comment_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(comment_layer));

  battery_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON);
  battery_layer = bitmap_layer_create(GRect(60, 140, 24, 12));
  bitmap_layer_set_bitmap(battery_layer, battery_bitmap);

  layer_add_child(window_layer, bitmap_layer_get_layer(battery_layer));

  update_time();
}

static void window_unload(Window *window) {
  text_layer_destroy(time_layer);
  text_layer_destroy(comment_layer);
  bitmap_layer_destroy(battery_layer);
  gbitmap_destroy(battery_bitmap);
}

static void init() {

  window = window_create();

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });

  window_stack_push(window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
