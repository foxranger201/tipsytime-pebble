
#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_comment_layer;
static TextLayer *s_day_layer;
static TextLayer *s_battery_layer;

static const char *INTRO[] = {
"Pretty sure it's","I think it's","Probably","Damn near","Hell if I know but it's",
"My watch says it's","My brain thinks it's","Looks like","Roughly","I'm guessing it's",
"Don't quote me but it's","I swear it's","My blurry eyes say","Could be wrong but it's"
};

static const char *MINUTES[] = {
"five past","ten past","quarter past","twenty past","half past",
"quarter to","ten to","five to","almost","nearly"
};

static const char *HOURS[] = {
"twelve","one","two","three","four","five","six",
"seven","eight","nine","ten","eleven"
};

static const char *COMMENTS[] = {
"I need water","Bad decision earlier","Tomorrow me is angry","Who moved the clock",
"Time feels fake","Sleep would help","Still awake huh","Brain buffering",
"I blame the bartender","Need food now","Hydration required","Regret detected"
};

static const char *DAY_NAMES[] = {
"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"
};

static const char *BATTERY[] = {
"Battery strong","Still plenty of juice","Battery halfway tired",
"Battery looking nervous","Plug me in soon","Battery panic mode"
};

static int r(int max){return rand()%max;}

static void update_battery() {
  BatteryChargeState state = battery_state_service_peek();
  const char *msg;
  if(state.charge_percent>75) msg=BATTERY[0];
  else if(state.charge_percent>50) msg=BATTERY[1];
  else if(state.charge_percent>35) msg=BATTERY[2];
  else if(state.charge_percent>20) msg=BATTERY[3];
  else if(state.charge_percent>10) msg=BATTERY[4];
  else msg=BATTERY[5];
  text_layer_set_text(s_battery_layer,msg);
}

static void update_time() {
  time_t temp=time(NULL);
  struct tm *tick_time=localtime(&temp);

  int intro_i=r(sizeof(INTRO)/sizeof(INTRO[0]));
  int min_i=r(sizeof(MINUTES)/sizeof(MINUTES[0]));
  int hour=tick_time->tm_hour%12;
  int comment_i=r(sizeof(COMMENTS)/sizeof(COMMENTS[0]));
  int day=tick_time->tm_wday;

  if(r(100)<10) min_i=r(sizeof(MINUTES)/sizeof(MINUTES[0]));
  if(r(100)<5) hour=(hour+1)%12;
  if(r(100)<3) day=r(7);

  static char time_buffer[64];
  snprintf(time_buffer,sizeof(time_buffer),"%s %s %s",INTRO[intro_i],MINUTES[min_i],HOURS[hour]);
  text_layer_set_text(s_time_layer,time_buffer);
  text_layer_set_text(s_comment_layer,COMMENTS[comment_i]);
  text_layer_set_text(s_day_layer,DAY_NAMES[day]);

  update_battery();
}

static void tick_handler(struct tm *tick_time,TimeUnits units_changed){
  update_time();
}

static void battery_handler(BatteryChargeState state){
  update_battery();
}

static void main_window_load(Window *window){
  Layer *window_layer=window_get_root_layer(window);
  GRect bounds=layer_get_bounds(window_layer);

  s_time_layer=text_layer_create(GRect(0,10,bounds.size.w,50));
  text_layer_set_font(s_time_layer,fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_time_layer,GTextAlignmentCenter);
  layer_add_child(window_layer,text_layer_get_layer(s_time_layer));

  s_comment_layer=text_layer_create(GRect(0,60,bounds.size.w,30));
  text_layer_set_font(s_comment_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_comment_layer,GTextAlignmentCenter);
  layer_add_child(window_layer,text_layer_get_layer(s_comment_layer));

  s_day_layer=text_layer_create(GRect(0,90,bounds.size.w,30));
  text_layer_set_font(s_day_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_day_layer,GTextAlignmentCenter);
  layer_add_child(window_layer,text_layer_get_layer(s_day_layer));

  s_battery_layer=text_layer_create(GRect(0,120,bounds.size.w,30));
  text_layer_set_font(s_battery_layer,fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_battery_layer,GTextAlignmentCenter);
  layer_add_child(window_layer,text_layer_get_layer(s_battery_layer));

  update_time();
}

static void main_window_unload(Window *window){
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_comment_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_battery_layer);
}

static void init(){
  srand(time(NULL));
  s_main_window=window_create();
  window_set_window_handlers(s_main_window,(WindowHandlers){
    .load=main_window_load,
    .unload=main_window_unload
  });
  window_stack_push(s_main_window,true);

  tick_timer_service_subscribe(MINUTE_UNIT,tick_handler);
  battery_state_service_subscribe(battery_handler);
}

static void deinit(){
  battery_state_service_unsubscribe();
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}
