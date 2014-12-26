#include <pebble.h>
#include "string.h"
  
#define DEBUG 0
#define BUFFER_SIZE 44
  
static int tick_set;
static int debug_hour;
static int debug_min;

static Window *s_main_window;
static TextLayer *s_msg1_layer;
static TextLayer *s_msg2_layer;
static TextLayer *s_msg3_layer;
static TextLayer *s_msg4_layer;

static GFont s_custom_font1;
static GFont s_custom_font2;

static char line1Str[2][BUFFER_SIZE];
static char line2Str[2][BUFFER_SIZE];
static char line3Str[2][BUFFER_SIZE];
static char line4Str[2][BUFFER_SIZE];


static const char* const SPECIAL[] = {
  "lEd",
  "lEfjE",
  "nav",
  "arO",
  "nezit",
  "noSuh",
  "crep"
};

static const char* const ONES[] = {
  " ",
  "Ge",
  "Qttek",
  "morAh",
  "GEn",
  "tq",
  "tah",
  "tEh",
  "cloò",
  "cnelik",
  "zIt"
};

static const char* const TENS[] = {
  " ",
  "zIt",
  "SUh",
  "cnimrah",
  "nevGen",
  "nevtq"
};

void time_to_4words(int hours, int minutes, char *line1, char *line2, char *line3, char *line4)
{
    
  memset(line1, '\0', BUFFER_SIZE);
  memset(line2, '\0', BUFFER_SIZE);
  memset(line3, '\0', BUFFER_SIZE);
  memset(line4, '\0', BUFFER_SIZE);


  if (hours == 0 && minutes == 0) {
    strcpy(line1, "");
    strcpy(line2, SPECIAL[1]);
    strcpy(line3, SPECIAL[2]);
    strcpy(line4, "");
  }  else if (hours == 12 && minutes == 0) {
    strcpy(line1, "");
    strcpy(line2, SPECIAL[0]);
    strcpy(line3, SPECIAL[2]);
    strcpy(line4, "");
  } else {
    
    if(hours > 12) {
      hours = hours - 12;
    } else if(hours == 0) {
      hours = 12;
    }
    int h_tens_val = hours / 10 % 10;
    int h_ones_val = hours % 10;
    int m_tens_val = minutes / 10 % 10;
    int m_ones_val = minutes % 10;

    if (minutes > 0 && hours <= 10) {
      strcpy(line1, ONES[hours]);
      strcpy(line2, SPECIAL[3]);
    } else if (minutes == 0 && hours <= 10) {
      strcpy(line1, "");
      strcpy(line2, ONES[hours]);
    } else if (hours == 10 || hours == 20) {
      strcpy(line1, TENS[h_tens_val]);
      strcpy(line2, SPECIAL[3]);
    } else if (hours > 10 && hours < 20) {
      strcpy(line1, SPECIAL[4]);
      strcpy(line2, ONES[h_ones_val]);
    } else if (hours > 20) {
      strcpy(line1, SPECIAL[5]);
      strcpy(line2, ONES[h_ones_val]);
    } else {
      strcpy(line1, TENS[h_tens_val]);
      strcpy(line2, ONES[h_ones_val]);
    }

    if (minutes == 0 && hours <= 11) {
      strcpy(line3, SPECIAL[3]);
      strcpy(line4, "");
    } else if (minutes <= 10) {
      strcpy(line3, ONES[minutes]);
      strcpy(line4, SPECIAL[6]);
    } else if (m_ones_val == 0) {
      strcpy(line3, TENS[m_tens_val]);
      strcpy(line4, SPECIAL[6]);
    } else if (minutes > 10 && minutes < 20) {
      strcpy(line3, SPECIAL[4]);
      strcpy(line4, ONES[m_ones_val]);
    } else if (minutes > 20 && minutes < 30) {
      strcpy(line3, SPECIAL[5]);
      strcpy(line4, ONES[m_ones_val]);
    } else {
      strcpy(line3, TENS[m_tens_val]);
      strcpy(line4, ONES[m_ones_val]);
    }
  }
  	  
}

// Update screen without animation first time we start the watchface
static void display_initial_time(int hour, int min)
{  
	time_to_4words(hour, min, line1Str[0], line2Str[0], line3Str[0], line4Str[0]);
	
	text_layer_set_text(s_msg1_layer, line1Str[0]);
	text_layer_set_text(s_msg2_layer, line2Str[0]);
	text_layer_set_text(s_msg3_layer, line3Str[0]);
	text_layer_set_text(s_msg4_layer, line4Str[0]);
}



// Debug methods. For quickly debugging enable debug macro on top to transform the watchface into
// a standard app and you will be able to change the time with the up and down buttons
#if DEBUG

static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  debug_hour += 1;
	if (debug_hour >= 24) {
			debug_hour = 0;
	}

  display_initial_time(debug_hour, debug_min);
}


static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  debug_min += 1;
 	if (debug_min >= 60) {
		debug_min = 0;
	}
  
  display_initial_time(debug_hour, debug_min);
}

static void click_config_provider(ClickRecognizerRef recognizer, void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler)up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler)down_single_click_handler);
}

#endif

  
// Configure the bold lines
static void configureBoldLayer(TextLayer *textlayer)
{
  text_layer_set_font(textlayer, s_custom_font1);
	text_layer_set_text_color(textlayer, GColorWhite);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, GTextAlignmentRight);
}

// Configure the light lines
static void configureLightLayer(TextLayer *textlayer)
{
  text_layer_set_font(textlayer, s_custom_font2);
	text_layer_set_text_color(textlayer, GColorWhite);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, GTextAlignmentRight);
}

static void main_window_load(Window *window) {
  s_custom_font1 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROVAS_45));
  s_custom_font2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROVAS_42));


  s_msg1_layer = text_layer_create(GRect(0, 0, 144, 50));
  configureBoldLayer(s_msg1_layer);

  s_msg2_layer = text_layer_create(GRect(0, 44, 144, 50));
  configureBoldLayer(s_msg2_layer);
  
  s_msg3_layer = text_layer_create(GRect(0, 88, 144, 46));
  configureLightLayer(s_msg3_layer);

  s_msg4_layer = text_layer_create(GRect(0, 125, 144, 46));
  configureLightLayer(s_msg4_layer);

  // Add it as a child layers to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_msg1_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_msg2_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_msg3_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_msg4_layer));

 	// Configure time on init
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
	display_initial_time(t->tm_hour, t->tm_min);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if(tick_set == 0) {
    debug_hour = tick_time->tm_hour;
    debug_min = tick_time->tm_min;
    tick_set = 1;
  }
  display_initial_time(tick_time->tm_hour, tick_time->tm_min);
}

static void main_window_unload(Window *window) {
  // destroy fonts
  fonts_unload_custom_font(s_custom_font1);
  fonts_unload_custom_font(s_custom_font2);

  // Destroy TextLayer
  text_layer_destroy(s_msg1_layer);
  text_layer_destroy(s_msg2_layer);
  text_layer_destroy(s_msg3_layer);
  text_layer_destroy(s_msg4_layer);

}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_fullscreen(s_main_window, true);
  window_set_background_color(s_main_window, GColorBlack);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  #if DEBUG
	// Button functionality
	window_set_click_config_provider(s_main_window, (ClickConfigProvider) click_config_provider);
	#endif
    
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
}

static void deinit() {
  tick_timer_service_unsubscribe();
  
  // Destroy Window
  window_destroy(s_main_window);
}



int main(void) {
  init();
  app_event_loop();
  deinit();
}


  