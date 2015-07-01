#include <pebble.h>
#include "gpathcentrerotate.h"

/*****************************************************/
/***************** DEFINE CONSTANTS ******************/	
/*****************************************************/

/* screen sizes */
#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168

/* centre of rotation */
#define ROT_CENTRE_X 72
#define ROT_CENTRE_Y 84

/* top "circle" */
#define CENTRE_DIA 5
#define CIRCLE_STROKE_COLOUR GColorBlack
#define CIRCLE_FILL_COLOUR GColorWhite

/* seconds hand */
#define SECONDS_STROKE_COLOUR GColorBlack
#define SECONDS_FILL_COLOUR GColorWhite
	
/* hand colours */
#define HANDS_STROKE_COLOUR GColorBlack
#define HANDS_FILL_COLOUR GColorWhite
//#define HANDS_LUME_COLOUR GColorMalachite
	
/* seconds paddle */
#define SECS_IC_X 4
#define SECS_IC_Y 56
	
/* size of dial text bitmap */
#define DIAL_X 46
#define DIAL_Y 67
	
/*****************************************************/
/*********** DEFINE VARIABLES & 'OBJECTS' ************/	
/*****************************************************/

/* windows & layers */
static Window *main_window;
Layer *hours_layer, *minutes_layer, *seconds_layer, *circle_layer;
RotBitmapLayer *paddle_layer;
BitmapLayer *dial_layer, *dial_text_layer;
TextLayer *temp_text_layer;

/* paths & bitmaps */
GPath *s_hand_path, *m_hand_path, *h_hand_path;
//GPath *m_lume_path;
GBitmap *seconds_paddle, *watch_dial, *dial_text;

/* variables */
char temp_buff[] = "00";
bool drawing_hands = false;
int hours, minutes, seconds, hours_display;
int hours_angle = -1;			// set to -1 as default so we draw on load
int minutes_angle = -1;		// set to -1 as default so we draw on load


/*****************************************************/
/*********** DEFINE PATH INFOS & POINTS **************/	
/*****************************************************/

/* 3-pixel wide rectangle for the seconds hand, stroked at 1px so 1px visible */
GPathInfo s_hand_info = {
	.num_points = 4,
	.points = (GPoint[]) { {71,28} , {73,28} , {73,107} , {71,107} }
};


/* 7=pixel side rectangle for the minutes hand, stroked at 1px so 5px visible */
GPathInfo m_hand_info = {
	.num_points = 4,
	.points = (GPoint[]) { {69,21} , {75,21} , {75,94} , {69,94} }
};

/* 3-pixel wide rectangle for the lume within the minutes hand, stroked at 1px so 1px visible */
/*GPathInfo m_lume_info = {
	.num_points = 4,
	.points = (GPoint[]) { {71,28} , {73,28} , {73,68} , {71,68} }
};*/

GPathInfo h_hand_info = {
	.num_points = 4,
	.points = (GPoint[]) { {69,40} , {75,40} , {75,94} , {69,94} }
};


/*****************************************************/
/**************** LAYER UPDATE PROCS *****************/	
/*****************************************************/

void hours_update_proc(Layer *l, GContext *ctx) {
	if(drawing_hands) {
		
		// set paths to new angle
		GPoint rot_point = (GPoint) {ROT_CENTRE_X, ROT_CENTRE_Y};
		gpath_centre_rotate(h_hand_path, hours_angle, rot_point);
		// draw and fill the hour hand
		graphics_context_set_stroke_color(ctx, HANDS_STROKE_COLOUR);
		graphics_context_set_fill_color(ctx, HANDS_FILL_COLOUR);
		gpath_draw_filled(ctx, h_hand_path);
		gpath_draw_outline(ctx, h_hand_path);
		
	}
}

void minutes_update_proc(Layer *l, GContext *ctx) {
	if(drawing_hands) {
		
		// set paths to new angle
		GPoint rot_point = (GPoint) {ROT_CENTRE_X, ROT_CENTRE_Y};
		gpath_centre_rotate(m_hand_path, minutes_angle, rot_point);
		//gpath_centre_rotate(m_lume_path, minutes_angle, rot_point);
		// draw and fill the minute hand
		graphics_context_set_stroke_color(ctx, HANDS_STROKE_COLOUR);
		graphics_context_set_fill_color(ctx, HANDS_FILL_COLOUR);
		gpath_draw_filled(ctx, m_hand_path);
		gpath_draw_outline(ctx, m_hand_path);
		// draw and fill the minute lume
		//graphics_context_set_stroke_color(ctx, HANDS_FILL_COLOUR);
		//graphics_context_set_fill_color(ctx, HANDS_LUME_COLOUR);
		//gpath_draw_filled(ctx, m_lume_path);
		//gpath_draw_outline(ctx, m_lume_path);
	}
}

void seconds_update_proc(Layer *l, GContext *ctx) {
	if(drawing_hands) {
		int rot_deg = seconds * (360 / 60);
		GPoint rot_point = (GPoint) {ROT_CENTRE_X, ROT_CENTRE_Y};
		gpath_centre_rotate(s_hand_path, rot_deg, rot_point);
		graphics_context_set_stroke_color(ctx, SECONDS_STROKE_COLOUR);
		graphics_context_set_fill_color(ctx, SECONDS_FILL_COLOUR);
		gpath_draw_filled(ctx, s_hand_path);
		gpath_draw_outline(ctx, s_hand_path);
	}
}

void circle_update_proc(Layer *l, GContext *ctx) {
	
	/*graphics_context_set_stroke_color(ctx, GColorRed);
	graphics_draw_line(ctx, GPoint(70,84), GPoint(74,84));
	graphics_draw_line(ctx, GPoint(72,82), GPoint(72, 86));
	graphics_draw_circle(ctx, GPoint(72,84), 23);*/
	
	graphics_context_set_fill_color(ctx, CIRCLE_FILL_COLOUR);
	graphics_context_set_stroke_color(ctx, CIRCLE_STROKE_COLOUR);
	graphics_fill_circle(ctx, GPoint(ROT_CENTRE_X, ROT_CENTRE_Y), CENTRE_DIA);
	graphics_draw_circle(ctx, GPoint(ROT_CENTRE_X, ROT_CENTRE_Y), CENTRE_DIA);
	
}


void paddle_layer_update() {
	GRect r;
	int32_t seconds_angle = seconds * TRIG_MAX_ANGLE / 60;
	r = layer_get_frame((Layer *)paddle_layer);
	r.origin.x = (SCREEN_WIDTH - r.size.w)/2;
	r.origin.y = (SCREEN_HEIGHT - r.size.h)/2;
	layer_set_frame((Layer *)paddle_layer, r);
	rot_bitmap_layer_set_angle(paddle_layer, seconds_angle);
}

/*****************************************************/
/********** TICK HANDLER HELPER FUNCTIONS ************/	
/*****************************************************/

/* get angle in degrees of the minute hand */
int get_minutes_angle() {
	int theta = (int) ( (float)minutes * (360/60.0) + (float)seconds * (360/(60*60.0)) );
	return theta;
}

/* get angle in degrees of the hour hand */
int get_hours_angle() {
	int theta = (int) ( (float)hours_display * (360 / 12.0) + (float)minutes * (360/(12*60.0)) );
	return theta;
}

/*****************************************************/
/******************* TICK HANDLER ********************/	
/*****************************************************/

/* tick handler - update the hands */
void tick_handler(struct tm * tick_time, TimeUnits units_changed) {

	// as soon as tick_handler starts, set the hands to be visible
	drawing_hands = true;
	
	// get time into storage
	hours = (int)tick_time->tm_hour;
	minutes = (int)tick_time->tm_min;
	seconds = (int)tick_time->tm_sec;
	
	// deal with 24-hour clock for calculation purposes
	if( hours >= 12) {
		hours_display = hours - 12;
	} else {
		hours_display = hours;
	}
	
	// deal with leap seconds!
	if(seconds > 59) seconds = 59;
	
	// update time if either it's not displaying OR this is a new day
	if(strcmp(temp_buff, "00") == 0 || (hours == 0 && minutes == 0 && seconds == 0)) {
		strftime(temp_buff, sizeof("00"), "%e", tick_time);
		text_layer_set_text(temp_text_layer, temp_buff);
	}
	
	// check whether we need to draw hours hand
	if( hours_angle == -1 || hours_angle != get_hours_angle() ) {
		hours_angle = get_hours_angle();
		layer_mark_dirty(hours_layer);	
	}
	
	
	// check whether we need to re-draw minutes hand
	if( minutes_angle == -1 || minutes_angle != get_minutes_angle() ) {
		minutes_angle = get_minutes_angle();
		layer_mark_dirty(minutes_layer);	
	}
	
	// draw seconds hand
	layer_mark_dirty(seconds_layer);
	
	// un-hide the paddle first time we draw - stops paddle showing in random position/angle on load
	bool paddle_hidden = layer_get_hidden((Layer *)paddle_layer);
	if(paddle_hidden) layer_set_hidden((Layer *)paddle_layer, false);
	paddle_layer_update();
}


/*****************************************************/
/*************** MAIN WINDOW HANDLERS ****************/	
/*****************************************************/

static void main_window_load(Window *w) {
	
	window_set_background_color(w, GColorBlack);
	
	// get a layer for the window_root for easy reference
	Layer *w_layer = window_get_root_layer(w);
	
	// init our general layers
	GRect r = GRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	hours_layer = layer_create(r);
	minutes_layer = layer_create(r);
	seconds_layer = layer_create(r);
	circle_layer = layer_create(r);
	
	// set update procs
	layer_set_update_proc(hours_layer, hours_update_proc);
	layer_set_update_proc(minutes_layer, minutes_update_proc);
	layer_set_update_proc(seconds_layer, seconds_update_proc);
	layer_set_update_proc(circle_layer, circle_update_proc);
		
	// init our gpaths
	s_hand_path = gpath_create(&s_hand_info);
	m_hand_path = gpath_create(&m_hand_info);
	h_hand_path = gpath_create(&h_hand_info);
	//m_lume_path = gpath_create(&m_lume_info);
	
	// init our bitmaps
	seconds_paddle = gbitmap_create_with_resource(RESOURCE_ID_SECOND_HAND_PADDLE);
	watch_dial = gbitmap_create_with_resource(RESOURCE_ID_WATCH_DIAL);
	dial_text = gbitmap_create_with_resource(RESOURCE_ID_DIAL_TEXT);
	
	// create rotbitmaplayer
	paddle_layer = rot_bitmap_layer_create(seconds_paddle);
	rot_bitmap_set_src_ic(paddle_layer, GPoint(SECS_IC_X,SECS_IC_Y));
	rot_bitmap_set_compositing_mode(paddle_layer, GCompOpSet);
	
	// create background bitmaplayer
	dial_layer = bitmap_layer_create(r);
	bitmap_layer_set_bitmap(dial_layer, watch_dial);
	
	// create dial text bitmaplayer
	dial_text_layer = bitmap_layer_create(GRect( (SCREEN_WIDTH - DIAL_X)/2, (SCREEN_HEIGHT - DIAL_Y)/2 + 2, DIAL_X, DIAL_Y ));
	bitmap_layer_set_bitmap(dial_text_layer, dial_text);
	
	// create the text layer
	temp_text_layer = text_layer_create(GRect(114,75,15,15));
	text_layer_set_background_color(temp_text_layer, GColorClear);
	text_layer_set_text_color(temp_text_layer, GColorBlack);
	text_layer_set_text(temp_text_layer, "00");
	text_layer_set_text_alignment(temp_text_layer, GTextAlignmentRight);
	text_layer_set_font(temp_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	
	// add layers to window
	layer_add_child(w_layer, (Layer *)dial_layer);
	layer_add_child(w_layer, (Layer *)dial_text_layer);
	layer_add_child(w_layer, text_layer_get_layer(temp_text_layer));
	layer_add_child(w_layer, hours_layer);
	layer_add_child(w_layer, minutes_layer);
	layer_add_child(w_layer, seconds_layer);
	layer_add_child(w_layer, (Layer *)paddle_layer);
	layer_add_child(w_layer, circle_layer);
	
	// hide the paddle layer until we've moved it!
	layer_set_hidden((Layer *)paddle_layer, true);
	
}

static void main_window_unload(Window *w) {
	// destroy layers
	layer_destroy(hours_layer);
	layer_destroy(minutes_layer);
	layer_destroy(seconds_layer);
	layer_destroy(circle_layer);
	rot_bitmap_layer_destroy(paddle_layer);
	bitmap_layer_destroy(dial_layer);
	bitmap_layer_destroy(dial_text_layer);
	
	// destroy gpaths & gbitmaps
	gpath_destroy(s_hand_path);
	gpath_destroy(m_hand_path);
	gpath_destroy(h_hand_path);
	//gpath_destroy(m_lume_path);
	gbitmap_destroy(seconds_paddle);
	gbitmap_destroy(watch_dial);
	gbitmap_destroy(dial_text);
	
	// temp text layer
	text_layer_destroy(temp_text_layer);
}


/*****************************************************/
/*************** INIT, DEINIT & MAIN *****************/	
/*****************************************************/

static void init() {
	
	// create main Window element and assign to pointer
	main_window = window_create();
	
	// subscribe to tick timer
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
	
	// set handlers to manage the elements inside the Window
	window_set_window_handlers(main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});	
	
	// show the Window on the watch, with animated = true
	window_stack_push(main_window, true);
}


static void deinit() {
	window_destroy(main_window);
	tick_timer_service_unsubscribe();
}


int main(void) {
	init();
	app_event_loop();
	deinit();
}