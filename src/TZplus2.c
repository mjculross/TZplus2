#include <pebble.h>

static Window *window;

// This is a custom defined key for saving the display_is_local flag
#define PKEY_DISPLAY_IS_LOCAL 65432
#define DISPLAY_IS_LOCAL_DEFAULT true 

// This is a custom defined key for saving the local_index
#define PKEY_LOCAL_INDEX 26543
#define LOCAL_INDEX_DEFAULT 15 // local=CST

// This is a custom defined key for saving the tz1_index
#define PKEY_TZ1_INDEX 32654
#define TZ1_INDEX_DEFAULT 24 // New York

// This is a custom defined key for saving the tz2_index
#define PKEY_TZ2_INDEX 43265
#define TZ2_INDEX_DEFAULT 38 // London

// This is a custom defined key for saving the tz3_index
#define PKEY_TZ3_INDEX 54326
#define TZ3_INDEX_DEFAULT 80 // Tokyo

#define TOTAL_IMAGES 7
#define SETMODE_SECONDS 20
#define LIGHT_TIMER_SECONDS 4
#define SWITCH_SECONDS 4
#define NUM_TIMEZONES 92

typedef enum {APP_IDLE_STATE = 0, APP_SET_LOCAL_STATE, APP_SET_TZ1_STATE, APP_SET_TZ2_STATE, APP_SET_TZ3_STATE, STATE_COUNT} APP_STATE;

struct tm current_time_local;
struct tm current_time1;
struct tm current_time2;
struct tm current_time3;

int setmode_timer = SETMODE_SECONDS;
int switch_seconds = 0;
int light_timer = LIGHT_TIMER_SECONDS;

bool toggle_flag = false;
bool display_is_local = DISPLAY_IS_LOCAL_DEFAULT;

bool light_on = false;

int local_index = LOCAL_INDEX_DEFAULT;
int local_offset_hours = 0;
int local_offset_mins = 0;

int tz1_index = TZ1_INDEX_DEFAULT;
int tz1_offset_hours = 0;
int tz1_offset_mins = 0;

int tz2_index = TZ2_INDEX_DEFAULT;
int tz2_offset_hours = 0;
int tz2_offset_mins = 0;

int tz3_index = TZ3_INDEX_DEFAULT;
int tz3_offset_hours = 0;
int tz3_offset_mins = 0;

int app_state = APP_IDLE_STATE;
int splash_timer = 3;

GBitmap *tz1_name_image;
GBitmap *tz2_name_image;
GBitmap *tz3_name_image;
GBitmap *tz1_digits_image[TOTAL_IMAGES];
GBitmap *tz2_digits_image[TOTAL_IMAGES];
GBitmap *tz3_digits_image[TOTAL_IMAGES];
GBitmap *splash_image;

BitmapLayer *tz1_name_layer;
BitmapLayer *tz2_name_layer;
BitmapLayer *tz3_name_layer;
BitmapLayer *tz1_digits_layer[TOTAL_IMAGES];
BitmapLayer *tz2_digits_layer[TOTAL_IMAGES];
BitmapLayer *tz3_digits_layer[TOTAL_IMAGES];
BitmapLayer *splash_layer;

const int DAY_TIME_IMAGE_RESOURCE_IDS[] =
{
   RESOURCE_ID_IMAGE_DAY_TIME_0,
   RESOURCE_ID_IMAGE_DAY_TIME_1,
   RESOURCE_ID_IMAGE_DAY_TIME_2,
   RESOURCE_ID_IMAGE_DAY_TIME_3,
   RESOURCE_ID_IMAGE_DAY_TIME_4,
   RESOURCE_ID_IMAGE_DAY_TIME_5,
   RESOURCE_ID_IMAGE_DAY_TIME_6,
   RESOURCE_ID_IMAGE_DAY_TIME_7,
   RESOURCE_ID_IMAGE_DAY_TIME_8,
   RESOURCE_ID_IMAGE_DAY_TIME_9,
};

const int NIGHT_TIME_IMAGE_RESOURCE_IDS[] =
{
   RESOURCE_ID_IMAGE_NIGHT_TIME_0,
   RESOURCE_ID_IMAGE_NIGHT_TIME_1,
   RESOURCE_ID_IMAGE_NIGHT_TIME_2,
   RESOURCE_ID_IMAGE_NIGHT_TIME_3,
   RESOURCE_ID_IMAGE_NIGHT_TIME_4,
   RESOURCE_ID_IMAGE_NIGHT_TIME_5,
   RESOURCE_ID_IMAGE_NIGHT_TIME_6,
   RESOURCE_ID_IMAGE_NIGHT_TIME_7,
   RESOURCE_ID_IMAGE_NIGHT_TIME_8,
   RESOURCE_ID_IMAGE_NIGHT_TIME_9,
};

typedef struct
{
   int day_image_id;
   int night_image_id;
   int offset_hours;
   int offset_mins;
} timezone_t;


timezone_t timezones[] =
{
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_11,      .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_11,      .offset_hours = -11,  .offset_mins =   0 },  // 000
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_10,      .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_10,      .offset_hours = -10,  .offset_mins =   0 },  // 001
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_HAWAII,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_HAWAII,            .offset_hours = -10,  .offset_mins =   0 },  // 002
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_9,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_9,       .offset_hours =  -9,  .offset_mins =   0 },  // 003
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_ALASKA,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_ALASKA,            .offset_hours =  -9,  .offset_mins =   0 },  // 004
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_8,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_8,       .offset_hours =  -8,  .offset_mins =   0 },  // 005
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_PST,               .night_image_id = RESOURCE_ID_IMAGE_NIGHT_PST,               .offset_hours =  -8,  .offset_mins =   0 },  // 006
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_LOSANGELES,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_LOSANGELES,        .offset_hours =  -8,  .offset_mins =   0 },  // 007
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_7,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_7,       .offset_hours =  -7,  .offset_mins =   0 },  // 008
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_PDT,               .night_image_id = RESOURCE_ID_IMAGE_NIGHT_PDT,               .offset_hours =  -7,  .offset_mins =   0 },  // 009
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_MST,               .night_image_id = RESOURCE_ID_IMAGE_NIGHT_MST,               .offset_hours =  -7,  .offset_mins =   0 },  // 010
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_DENVER,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_DENVER,            .offset_hours =  -7,  .offset_mins =   0 },  // 011
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_PHOENIX,           .night_image_id = RESOURCE_ID_IMAGE_NIGHT_PHOENIX,           .offset_hours =  -7,  .offset_mins =   0 },  // 012
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_6,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_6,       .offset_hours =  -6,  .offset_mins =   0 },  // 013
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_MDT,               .night_image_id = RESOURCE_ID_IMAGE_NIGHT_MDT,               .offset_hours =  -6,  .offset_mins =   0 },  // 014
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_CST,               .night_image_id = RESOURCE_ID_IMAGE_NIGHT_CST,               .offset_hours =  -6,  .offset_mins =   0 },  // 015
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_ATLANTA,           .night_image_id = RESOURCE_ID_IMAGE_NIGHT_ATLANTA,           .offset_hours =  -6,  .offset_mins =   0 },  // 016
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_DALLAS,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_DALLAS,            .offset_hours =  -6,  .offset_mins =   0 },  // 017
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_STLOUIS,           .night_image_id = RESOURCE_ID_IMAGE_NIGHT_STLOUIS,           .offset_hours =  -6,  .offset_mins =   0 },  // 018
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_5,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_5,       .offset_hours =  -5,  .offset_mins =   0 },  // 019
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_CDT,               .night_image_id = RESOURCE_ID_IMAGE_NIGHT_CDT,               .offset_hours =  -5,  .offset_mins =   0 },  // 020
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_EST,               .night_image_id = RESOURCE_ID_IMAGE_NIGHT_EST,               .offset_hours =  -5,  .offset_mins =   0 },  // 021
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_DETROIT,           .night_image_id = RESOURCE_ID_IMAGE_NIGHT_DETROIT,           .offset_hours =  -5,  .offset_mins =   0 },  // 022
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_MIAMI,             .night_image_id = RESOURCE_ID_IMAGE_NIGHT_MIAMI,             .offset_hours =  -5,  .offset_mins =   0 },  // 023
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_NEWYORK,           .night_image_id = RESOURCE_ID_IMAGE_NIGHT_NEWYORK,           .offset_hours =  -5,  .offset_mins =   0 },  // 024
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_4_30,    .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_4_30,    .offset_hours =  -4,  .offset_mins = -30 },  // 025
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_4,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_4,       .offset_hours =  -4,  .offset_mins =   0 },  // 026
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_EDT,               .night_image_id = RESOURCE_ID_IMAGE_NIGHT_EDT,               .offset_hours =  -4,  .offset_mins =   0 },  // 027
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_SANJUAN,           .night_image_id = RESOURCE_ID_IMAGE_NIGHT_SANJUAN,           .offset_hours =  -4,  .offset_mins =   0 },  // 028
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_3_30,    .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_3_30,    .offset_hours =  -3,  .offset_mins = -30 },  // 029
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_3,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_3,       .offset_hours =  -3,  .offset_mins =   0 },  // 030
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_GREENLAND,         .night_image_id = RESOURCE_ID_IMAGE_NIGHT_GREENLAND,         .offset_hours =  -3,  .offset_mins =   0 },  // 031
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_SALVADOR,          .night_image_id = RESOURCE_ID_IMAGE_NIGHT_SALVADOR,          .offset_hours =  -3,  .offset_mins =   0 },  // 032
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_2,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_2,       .offset_hours =  -2,  .offset_mins =   0 },  // 033
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_MINUS_1,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_MINUS_1,       .offset_hours =  -1,  .offset_mins =   0 },  // 034
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_AZORES,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_AZORES,            .offset_hours =  -1,  .offset_mins =   0 },  // 035
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC,               .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC,               .offset_hours =   0,  .offset_mins =   0 },  // 036
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_EDINBURGH,         .night_image_id = RESOURCE_ID_IMAGE_NIGHT_EDINBURGH,         .offset_hours =   0,  .offset_mins =   0 },  // 037
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_LONDON,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_LONDON,            .offset_hours =   0,  .offset_mins =   0 },  // 038
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_REYKJAVIK,         .night_image_id = RESOURCE_ID_IMAGE_NIGHT_REYKJAVIK,         .offset_hours =   0,  .offset_mins =   0 },  // 039
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_1,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_1,        .offset_hours =  +1,  .offset_mins =   0 },  // 040
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_AMSTERDAM,         .night_image_id = RESOURCE_ID_IMAGE_NIGHT_AMSTERDAM,         .offset_hours =  +1,  .offset_mins =   0 },  // 041
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_BERLIN,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_BERLIN,            .offset_hours =  +1,  .offset_mins =   0 },  // 042
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_ROME,              .night_image_id = RESOURCE_ID_IMAGE_NIGHT_ROME,              .offset_hours =  +1,  .offset_mins =   0 },  // 043
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_STOCKHOLM,         .night_image_id = RESOURCE_ID_IMAGE_NIGHT_STOCKHOLM,         .offset_hours =  +1,  .offset_mins =   0 },  // 044
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_BRUSSELS,          .night_image_id = RESOURCE_ID_IMAGE_NIGHT_BRUSSELS,          .offset_hours =  +1,  .offset_mins =   0 },  // 045
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_COPENHAGEN,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_COPENHAGEN,        .offset_hours =  +1,  .offset_mins =   0 },  // 046
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_PARIS,             .night_image_id = RESOURCE_ID_IMAGE_NIGHT_PARIS,             .offset_hours =  +1,  .offset_mins =   0 },  // 047
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_2,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_2,        .offset_hours =  +2,  .offset_mins =   0 },  // 048
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_ATHENS,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_ATHENS,            .offset_hours =  +2,  .offset_mins =   0 },  // 049
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_BEIRUT,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_BEIRUT,            .offset_hours =  +2,  .offset_mins =   0 },  // 050
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_CAIRO,             .night_image_id = RESOURCE_ID_IMAGE_NIGHT_CAIRO,             .offset_hours =  +2,  .offset_mins =   0 },  // 051
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_HELSINKI,          .night_image_id = RESOURCE_ID_IMAGE_NIGHT_HELSINKI,          .offset_hours =  +2,  .offset_mins =   0 },  // 052
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_JERUSALEM,         .night_image_id = RESOURCE_ID_IMAGE_NIGHT_JERUSALEM,         .offset_hours =  +2,  .offset_mins =   0 },  // 053
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_3,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_3,        .offset_hours =  +3,  .offset_mins =   0 },  // 054
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_BAGHDAD,           .night_image_id = RESOURCE_ID_IMAGE_NIGHT_BAGHDAD,           .offset_hours =  +3,  .offset_mins =   0 },  // 055
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_KUWAIT,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_KUWAIT,            .offset_hours =  +3,  .offset_mins =   0 },  // 056
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_RIYADH,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_RIYADH,            .offset_hours =  +3,  .offset_mins =   0 },  // 057
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_3_30,     .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_3_30,     .offset_hours =  +3,  .offset_mins = +30 },  // 058
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_4,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_4,        .offset_hours =  +4,  .offset_mins =   0 },  // 059
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_ABUDHABI,          .night_image_id = RESOURCE_ID_IMAGE_NIGHT_ABUDHABI,          .offset_hours =  +4,  .offset_mins =   0 },  // 060
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_MOSCOW,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_MOSCOW,            .offset_hours =  +4,  .offset_mins =   0 },  // 061
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_4_30,     .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_4_30,     .offset_hours =  +4,  .offset_mins = +30 },  // 062
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_KABUL,             .night_image_id = RESOURCE_ID_IMAGE_NIGHT_KABUL,             .offset_hours =  +4,  .offset_mins = +30 },  // 063
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_5,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_5,        .offset_hours =  +5,  .offset_mins =   0 },  // 064
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_5_30,     .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_5_30,     .offset_hours =  +5,  .offset_mins = +30 },  // 065
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_NEWDELHI,          .night_image_id = RESOURCE_ID_IMAGE_NIGHT_NEWDELHI,          .offset_hours =  +5,  .offset_mins = +30 },  // 066
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_5_45,     .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_5_45,     .offset_hours =  +5,  .offset_mins = +45 },  // 067
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_KATHMANDU,         .night_image_id = RESOURCE_ID_IMAGE_NIGHT_KATHMANDU,         .offset_hours =  +5,  .offset_mins = +45 },  // 068
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_6,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_6,        .offset_hours =  +6,  .offset_mins =   0 },  // 069
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_6_30,     .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_6_30,     .offset_hours =  +6,  .offset_mins = +30 },  // 070
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_7,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_7,        .offset_hours =  +7,  .offset_mins =   0 },  // 071
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_HANOI,             .night_image_id = RESOURCE_ID_IMAGE_NIGHT_HANOI,             .offset_hours =  +7,  .offset_mins =   0 },  // 072
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_JAKARTA,           .night_image_id = RESOURCE_ID_IMAGE_NIGHT_JAKARTA,           .offset_hours =  +7,  .offset_mins =   0 },  // 073
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_8,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_8,        .offset_hours =  +8,  .offset_mins =   0 },  // 074
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_BEIJING,           .night_image_id = RESOURCE_ID_IMAGE_NIGHT_BEIJING,           .offset_hours =  +8,  .offset_mins =   0 },  // 075
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_HONGKONG,          .night_image_id = RESOURCE_ID_IMAGE_NIGHT_HONGKONG,          .offset_hours =  +8,  .offset_mins =   0 },  // 076
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_SINGAPORE,         .night_image_id = RESOURCE_ID_IMAGE_NIGHT_SINGAPORE,         .offset_hours =  +8,  .offset_mins =   0 },  // 077
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_TAIPEI,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_TAIPEI,            .offset_hours =  +8,  .offset_mins =   0 },  // 078
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_9,        .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_9,        .offset_hours =  +9,  .offset_mins =   0 },  // 079
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_TOKYO,             .night_image_id = RESOURCE_ID_IMAGE_NIGHT_TOKYO,             .offset_hours =  +9,  .offset_mins =   0 },  // 080
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_SEOUL,             .night_image_id = RESOURCE_ID_IMAGE_NIGHT_SEOUL,             .offset_hours =  +9,  .offset_mins =   0 },  // 081
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_9_30,     .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_9_30,     .offset_hours =  +9,  .offset_mins = +30 },  // 082
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_10,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_10,       .offset_hours =  +10, .offset_mins =   0 },  // 083
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_SYDNEY,            .night_image_id = RESOURCE_ID_IMAGE_NIGHT_SYDNEY,            .offset_hours =  +10, .offset_mins =   0 },  // 084
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_GUAM,              .night_image_id = RESOURCE_ID_IMAGE_NIGHT_GUAM,              .offset_hours =  +10, .offset_mins =   0 },  // 085
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_11,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_11,       .offset_hours =  +11, .offset_mins =   0 },  // 086
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_12,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_12,       .offset_hours =  +12, .offset_mins =   0 },  // 087
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_FIJI,              .night_image_id = RESOURCE_ID_IMAGE_NIGHT_FIJI,              .offset_hours =  +12, .offset_mins =   0 },  // 088
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_13,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_13,       .offset_hours =  +13, .offset_mins =   0 },  // 089
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_SAMOA,             .night_image_id = RESOURCE_ID_IMAGE_NIGHT_SAMOA,             .offset_hours =  +13, .offset_mins =   0 },  // 090
   { .day_image_id = RESOURCE_ID_IMAGE_DAY_UTC_PLUS_14,       .night_image_id = RESOURCE_ID_IMAGE_NIGHT_UTC_PLUS_14,       .offset_hours =  +14, .offset_mins =   0 },  // 091
};



static void click_config_provider(void *config);
static void deinit(void);
static void display_local(void);
static void display_tz1(void);
static void display_tz2(void);
static void display_tz3(void);
static void down_single_click_handler(ClickRecognizerRef recognizer, void *context);
static void handle_accel_tap(AccelAxisType axis, int32_t direction);
static void handle_second_tick(struct tm *tick, TimeUnits units_changed);
static void init(void);
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context);
static void select_long_release_handler(ClickRecognizerRef recognizer, void *context);
static void select_single_click_handler(ClickRecognizerRef recognizer, void *context);
static void set_bitmap_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint this_origin);
static void up_single_click_handler(ClickRecognizerRef recognizer, void *context);
static void update_display(struct tm *current_time);


static void click_config_provider(void *config)
{
   window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
   window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
   window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
   window_long_click_subscribe(BUTTON_ID_SELECT, 250, select_long_click_handler, select_long_release_handler);
}  // click_config_provider()


static void deinit(void)
{
   // Save all settings into persistent storage on app exit
   persist_write_int(PKEY_DISPLAY_IS_LOCAL, display_is_local);
   persist_write_int(PKEY_LOCAL_INDEX, local_index);
   persist_write_int(PKEY_TZ1_INDEX, tz1_index);
   persist_write_int(PKEY_TZ2_INDEX, tz2_index);
   persist_write_int(PKEY_TZ3_INDEX, tz3_index);

   layer_remove_from_parent(bitmap_layer_get_layer(tz1_name_layer));
   bitmap_layer_destroy(tz1_name_layer);
   gbitmap_destroy(tz1_name_image);

   layer_remove_from_parent(bitmap_layer_get_layer(tz2_name_layer));
   bitmap_layer_destroy(tz2_name_layer);
   gbitmap_destroy(tz2_name_image);

   layer_remove_from_parent(bitmap_layer_get_layer(tz3_name_layer));
   bitmap_layer_destroy(tz3_name_layer);
   gbitmap_destroy(tz3_name_image);

   for (int i = 0; i < TOTAL_IMAGES; i++)
   {
      layer_remove_from_parent(bitmap_layer_get_layer(tz1_digits_layer[i]));
      bitmap_layer_destroy(tz1_digits_layer[i]);
      gbitmap_destroy(tz1_digits_image[i]);

      layer_remove_from_parent(bitmap_layer_get_layer(tz2_digits_layer[i]));
      bitmap_layer_destroy(tz2_digits_layer[i]);
      gbitmap_destroy(tz2_digits_image[i]);

      layer_remove_from_parent(bitmap_layer_get_layer(tz3_digits_layer[i]));
      bitmap_layer_destroy(tz3_digits_layer[i]);
      gbitmap_destroy(tz3_digits_image[i]);
   }

   layer_remove_from_parent(bitmap_layer_get_layer(splash_layer));
   bitmap_layer_destroy(splash_layer);
   gbitmap_destroy(splash_image);

   tick_timer_service_unsubscribe();
   accel_tap_service_unsubscribe();
   window_destroy(window);
}  // deinit()


static void display_local(void)
{
   int yloc = 20;
   int resource_id_1;
   int resource_id_2;
   int resource_id_3;

   current_time_local.tm_min += local_offset_mins;

   if (current_time_local.tm_min >= 60)
   {
      current_time_local.tm_min %= 60;

      current_time_local.tm_hour += 1;
   }
   else
   {
      if (current_time_local.tm_min < 0)
      {
         current_time_local.tm_min += 60;

         current_time_local.tm_hour -= 1;
      }
   }

   current_time_local.tm_hour += local_offset_hours;

   if (current_time_local.tm_hour >= 24)
   {
      current_time_local.tm_hour %= 24;

      if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
      {
         // display + offset
         resource_id_1 = RESOURCE_ID_IMAGE_DAY_TIME_PLUS;
      }
      else
      {
         // display + offset
         resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TIME_PLUS;
      }
   }
   else
   {
      if (current_time_local.tm_hour < 0)
      {
         current_time_local.tm_hour += 24;

         if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
         {
            // display - offset
            resource_id_1 = RESOURCE_ID_IMAGE_DAY_TIME_MINUS;
         }
         else
         {
            // display - offset
            resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TIME_MINUS;
         }
      }
      else
      {
         if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
         {
            // display no offset
            resource_id_1 = RESOURCE_ID_IMAGE_DAY_TIME_NONE;
         }
         else
         {
            // display no offset
            resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TIME_NONE;
         }
      }
   }

   set_bitmap_image(&tz1_digits_image[5], tz1_digits_layer[5], resource_id_1, GPoint(114, yloc));

   // display local name
   if (app_state == APP_SET_LOCAL_STATE)
   {
      if (switch_seconds != 0)
      {
         if (toggle_flag == false)
         {
            resource_id_1 = RESOURCE_ID_IMAGE_DAY_LOCAL;
         }
         else
         {
            resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_LOCAL;
         }
      }
      else
      {
         if (toggle_flag == false)
         {
            resource_id_1 = timezones[local_index].day_image_id;
         }
         else
         {
            resource_id_1 = timezones[local_index].night_image_id;
         }
      }
   }
   else
   {
      if (switch_seconds != 0)
      {
         if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
         {
            resource_id_1 = RESOURCE_ID_IMAGE_DAY_LOCAL;
         }
         else
         {
            resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_LOCAL;
         }
      }
      else
      {
         if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
         {
            resource_id_1 = timezones[local_index].day_image_id;
         }
         else
         {
            resource_id_1 = timezones[local_index].night_image_id;
         }
      }
   }

   set_bitmap_image(&tz1_name_image, tz1_name_layer, resource_id_1, GPoint(0, yloc - 20));

   // display time hour
   if (clock_is_24h_style())
   {
      if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
      {
         resource_id_1 = DAY_TIME_IMAGE_RESOURCE_IDS[current_time_local.tm_hour / 10];
         resource_id_2 = DAY_TIME_IMAGE_RESOURCE_IDS[current_time_local.tm_hour % 10];

         // display no AM/PM
         resource_id_3 = RESOURCE_ID_IMAGE_DAY_TIME_NONE;
      }
      else
      {
         resource_id_1 = NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time_local.tm_hour / 10];
         resource_id_2 = NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time_local.tm_hour % 10];

         // display no AM/PM
         resource_id_3 = RESOURCE_ID_IMAGE_NIGHT_TIME_NONE;
      }
   }
   else
   {
      // display AM/PM
      if (current_time_local.tm_hour >= 12)
      {
         if (current_time_local.tm_hour <= 18)
         {
            resource_id_3 = RESOURCE_ID_IMAGE_DAY_TIME_PM;
         }
         else
         {
            resource_id_3 = RESOURCE_ID_IMAGE_NIGHT_TIME_PM;
         }
      }
      else
      {
         if (current_time_local.tm_hour >= 6)
         {
            resource_id_3 = RESOURCE_ID_IMAGE_DAY_TIME_AM;
         }
         else
         {
            resource_id_3 = RESOURCE_ID_IMAGE_NIGHT_TIME_AM;
         }
      }

      if ((current_time_local.tm_hour % 12) == 0)
      {
         if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
         {
            resource_id_1 = DAY_TIME_IMAGE_RESOURCE_IDS[1];
            resource_id_2 = DAY_TIME_IMAGE_RESOURCE_IDS[2];
         }
         else
         {
            resource_id_1 = NIGHT_TIME_IMAGE_RESOURCE_IDS[1];
            resource_id_2 = NIGHT_TIME_IMAGE_RESOURCE_IDS[2];
         }
      }
      else
      {
         if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
         {
            resource_id_1 = DAY_TIME_IMAGE_RESOURCE_IDS[(current_time_local.tm_hour % 12) / 10];
            resource_id_2 = DAY_TIME_IMAGE_RESOURCE_IDS[(current_time_local.tm_hour % 12) % 10];
         }
         else
         {
            resource_id_1 = NIGHT_TIME_IMAGE_RESOURCE_IDS[(current_time_local.tm_hour % 12) / 10];
            resource_id_2 = NIGHT_TIME_IMAGE_RESOURCE_IDS[(current_time_local.tm_hour % 12) % 10];
         }

         if ((current_time_local.tm_hour % 12) < 10)
         {
            if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
            {
               resource_id_1 = RESOURCE_ID_IMAGE_DAY_TIME_BLANK;
            }
            else
            {
               resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TIME_BLANK;
            }
         }
      }
   }

   set_bitmap_image(&tz1_digits_image[0], tz1_digits_layer[0], resource_id_1, GPoint(0, yloc));
   set_bitmap_image(&tz1_digits_image[1], tz1_digits_layer[1], resource_id_2, GPoint(26, yloc));

   // display no AM/PM
   set_bitmap_image(&tz1_digits_image[6], tz1_digits_layer[6], resource_id_3, GPoint(114, yloc + 18));

   // display local colon & minutes
   if ((current_time_local.tm_hour >= 6) && (current_time_local.tm_hour <= 18))
   {
      resource_id_1 = RESOURCE_ID_IMAGE_DAY_TIME_COLON;
      resource_id_2 = DAY_TIME_IMAGE_RESOURCE_IDS[current_time_local.tm_min / 10];
      resource_id_3 = DAY_TIME_IMAGE_RESOURCE_IDS[current_time_local.tm_min % 10];
   }
   else
   {
      resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TIME_COLON;
      resource_id_2 = NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time_local.tm_min / 10];
      resource_id_3 = NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time_local.tm_min % 10];
   }

   set_bitmap_image(&tz1_digits_image[2], tz1_digits_layer[2], resource_id_1, GPoint(52, yloc));
   set_bitmap_image(&tz1_digits_image[3], tz1_digits_layer[3], resource_id_2, GPoint(62, yloc));
   set_bitmap_image(&tz1_digits_image[4], tz1_digits_layer[4], resource_id_3, GPoint(88, yloc));
}  // display_local()


static void display_tz1(void)
{
   int yloc = 20;
   int resource_id_1;
   int resource_id_2;
   int resource_id_3;

   current_time1.tm_min += tz1_offset_mins;

   if (current_time1.tm_min >= 60)
   {
      current_time1.tm_min %= 60;

      current_time1.tm_hour += 1;
   }
   else
   {
      if (current_time1.tm_min < 0)
      {
         current_time1.tm_min += 60;

         current_time1.tm_hour -= 1;
      }
   }

   current_time1.tm_hour += tz1_offset_hours;

   if (current_time1.tm_hour >= 24)
   {
      current_time1.tm_hour %= 24;

      if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
      {
         // display + offset
         resource_id_1 = RESOURCE_ID_IMAGE_DAY_TIME_PLUS;
      }
      else
      {
         // display + offset
         resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TIME_PLUS;
      }
   }
   else
   {
      if (current_time1.tm_hour < 0)
      {
         current_time1.tm_hour += 24;

         if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
         {
            // display - offset
            resource_id_1 = RESOURCE_ID_IMAGE_DAY_TIME_MINUS;
         }
         else
         {
            // display - offset
            resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TIME_MINUS;
         }
      }
      else
      {
         if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
         {
            // display no offset
            resource_id_1 = RESOURCE_ID_IMAGE_DAY_TIME_NONE;
         }
         else
         {
            // display no offset
            resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TIME_NONE;
         }
      }
   }

   set_bitmap_image(&tz1_digits_image[5], tz1_digits_layer[5], resource_id_1, GPoint(114, yloc));

   // display tz1 name
   if (app_state == APP_SET_TZ1_STATE)
   {
      if (switch_seconds != 0)
      {
         if (toggle_flag == false)
         {
            resource_id_1 = RESOURCE_ID_IMAGE_DAY_TZ1;
         }
         else
         {
            resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TZ1;
         }
      }
      else
      {
         if (toggle_flag == false)
         {
            resource_id_1 = timezones[tz1_index].day_image_id;
         }
         else
         {
            resource_id_1 = timezones[tz1_index].night_image_id;
         }
      }
   }
   else
   {
      if (switch_seconds != 0)
      {
         if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
         {
            resource_id_1 = RESOURCE_ID_IMAGE_DAY_TZ1;
         }
         else
         {
            resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TZ1;
         }
      }
      else
      {
         if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
         {
            resource_id_1 = timezones[tz1_index].day_image_id;
         }
         else
         {
            resource_id_1 = timezones[tz1_index].night_image_id;
         }
      }
   }

   set_bitmap_image(&tz1_name_image, tz1_name_layer, resource_id_1, GPoint(0, yloc - 20));

   // display time hour
   if (clock_is_24h_style())
   {
      if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
      {
         resource_id_1 = DAY_TIME_IMAGE_RESOURCE_IDS[current_time1.tm_hour / 10];
         resource_id_2 = DAY_TIME_IMAGE_RESOURCE_IDS[current_time1.tm_hour % 10];

         // display no AM/PM
         resource_id_3 = RESOURCE_ID_IMAGE_DAY_TIME_NONE;
      }
      else
      {
         resource_id_1 = NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time1.tm_hour / 10];
         resource_id_2 = NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time1.tm_hour % 10];

         // display no AM/PM
         resource_id_3 = RESOURCE_ID_IMAGE_NIGHT_TIME_NONE;
      }
   }
   else
   {
      // display AM/PM
      if (current_time1.tm_hour >= 12)
      {
         if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
         {
            resource_id_3 = RESOURCE_ID_IMAGE_DAY_TIME_PM;
         }
         else
         {
            resource_id_3 = RESOURCE_ID_IMAGE_NIGHT_TIME_PM;
         }
      }
      else
      {
         if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
         {
            resource_id_3 = RESOURCE_ID_IMAGE_DAY_TIME_AM;
         }
         else
         {
            resource_id_3 = RESOURCE_ID_IMAGE_NIGHT_TIME_AM;
         }
      }

      if ((current_time1.tm_hour % 12) == 0)
      {
         if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
         {
            resource_id_1 = DAY_TIME_IMAGE_RESOURCE_IDS[1];
            resource_id_2 = DAY_TIME_IMAGE_RESOURCE_IDS[2];
         }
         else
         {
            resource_id_1 = NIGHT_TIME_IMAGE_RESOURCE_IDS[1];
            resource_id_2 = NIGHT_TIME_IMAGE_RESOURCE_IDS[2];
         }
      }
      else
      {
         if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
         {
            resource_id_1 = DAY_TIME_IMAGE_RESOURCE_IDS[(current_time1.tm_hour % 12) / 10];
            resource_id_2 = DAY_TIME_IMAGE_RESOURCE_IDS[(current_time1.tm_hour % 12) % 10];
         }
         else
         {
            resource_id_1 = NIGHT_TIME_IMAGE_RESOURCE_IDS[(current_time1.tm_hour % 12) / 10];
            resource_id_2 = NIGHT_TIME_IMAGE_RESOURCE_IDS[(current_time1.tm_hour % 12) % 10];
         }

         if ((current_time1.tm_hour % 12) < 10)
         {
            if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
            {
               resource_id_1 = RESOURCE_ID_IMAGE_DAY_TIME_BLANK;
            }
            else
            {
               resource_id_1 = RESOURCE_ID_IMAGE_NIGHT_TIME_BLANK;
            }
         }
      }
   }

   set_bitmap_image(&tz1_digits_image[0], tz1_digits_layer[0], resource_id_1, GPoint(0, yloc));
   set_bitmap_image(&tz1_digits_image[1], tz1_digits_layer[1], resource_id_2, GPoint(26, yloc));

   // display no AM/PM
   set_bitmap_image(&tz1_digits_image[6], tz1_digits_layer[6], resource_id_3, GPoint(114, yloc + 18));

   // display tz1 colon & minutes
   if ((current_time1.tm_hour >= 6) && (current_time1.tm_hour <= 18))
   {
      set_bitmap_image(&tz1_digits_image[2], tz1_digits_layer[2], RESOURCE_ID_IMAGE_DAY_TIME_COLON, GPoint(52, yloc));
      set_bitmap_image(&tz1_digits_image[3], tz1_digits_layer[3], DAY_TIME_IMAGE_RESOURCE_IDS[current_time1.tm_min / 10], GPoint(62, yloc));
      set_bitmap_image(&tz1_digits_image[4], tz1_digits_layer[4], DAY_TIME_IMAGE_RESOURCE_IDS[current_time1.tm_min % 10], GPoint(88, yloc));
   }
   else
   {
      set_bitmap_image(&tz1_digits_image[2], tz1_digits_layer[2], RESOURCE_ID_IMAGE_NIGHT_TIME_COLON, GPoint(52, yloc));
      set_bitmap_image(&tz1_digits_image[3], tz1_digits_layer[3], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time1.tm_min / 10], GPoint(62, yloc));
      set_bitmap_image(&tz1_digits_image[4], tz1_digits_layer[4], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time1.tm_min % 10], GPoint(88, yloc));
   }
}  // display_tz1()


static void display_tz2(void)
{
   int yloc = 76;

   current_time2.tm_min += tz2_offset_mins;

   if (current_time2.tm_min >= 60)
   {
      current_time2.tm_min %= 60;

      current_time2.tm_hour += 1;
   }
   else
   {
      if (current_time2.tm_min < 0)
      {
         current_time2.tm_min += 60;

         current_time2.tm_hour -= 1;
      }
   }

   current_time2.tm_hour += tz2_offset_hours;

   if (current_time2.tm_hour >= 24)
   {
      current_time2.tm_hour %= 24;

      if ((current_time2.tm_hour >= 6) && (current_time2.tm_hour <= 18))
      {
         // display + offset
         set_bitmap_image(&tz2_digits_image[5], tz2_digits_layer[5], RESOURCE_ID_IMAGE_DAY_TIME_PLUS, GPoint(114, yloc));
      }
      else
      {
         // display + offset
         set_bitmap_image(&tz2_digits_image[5], tz2_digits_layer[5], RESOURCE_ID_IMAGE_NIGHT_TIME_PLUS, GPoint(114, yloc));
      }
   }
   else
   {
      if (current_time2.tm_hour < 0)
      {
         current_time2.tm_hour += 24;

         if ((current_time2.tm_hour >= 6) && (current_time2.tm_hour <= 18))
         {
            // display - offset
            set_bitmap_image(&tz2_digits_image[5], tz2_digits_layer[5], RESOURCE_ID_IMAGE_DAY_TIME_MINUS, GPoint(114, yloc));
         }
         else
         {
            // display - offset
            set_bitmap_image(&tz2_digits_image[5], tz2_digits_layer[5], RESOURCE_ID_IMAGE_NIGHT_TIME_MINUS, GPoint(114, yloc));
         }
      }
      else
      {
         if ((current_time2.tm_hour >= 6) && (current_time2.tm_hour <= 18))
         {
            // display no offset
            set_bitmap_image(&tz2_digits_image[5], tz2_digits_layer[5], RESOURCE_ID_IMAGE_DAY_TIME_NONE, GPoint(114, yloc));
         }
         else
         {
            // display no offset
            set_bitmap_image(&tz2_digits_image[5], tz2_digits_layer[5], RESOURCE_ID_IMAGE_NIGHT_TIME_NONE, GPoint(114, yloc));
         }
      }
   }

   // display tz2 name
   if (app_state == APP_SET_TZ2_STATE)
   {
      if (toggle_flag == false)
      {
         set_bitmap_image(&tz2_name_image, tz2_name_layer, timezones[tz2_index].day_image_id, GPoint(0, yloc - 20));
      }
      else
      {
         set_bitmap_image(&tz2_name_image, tz2_name_layer, timezones[tz2_index].night_image_id, GPoint(0, yloc - 20));
      }
   }
   else
   {
      if ((current_time2.tm_hour >= 6) && (current_time2.tm_hour <= 18))
      {
         set_bitmap_image(&tz2_name_image, tz2_name_layer, timezones[tz2_index].day_image_id, GPoint(0, yloc - 20));
      }
      else
      {
         set_bitmap_image(&tz2_name_image, tz2_name_layer, timezones[tz2_index].night_image_id, GPoint(0, yloc - 20));
      }
   }

   // display time hour
   if (clock_is_24h_style())
   {
      if ((current_time2.tm_hour >= 6) && (current_time2.tm_hour <= 18))
      {
         set_bitmap_image(&tz2_digits_image[0], tz2_digits_layer[0], DAY_TIME_IMAGE_RESOURCE_IDS[current_time2.tm_hour / 10], GPoint(0, yloc));
         set_bitmap_image(&tz2_digits_image[1], tz2_digits_layer[1], DAY_TIME_IMAGE_RESOURCE_IDS[current_time2.tm_hour % 10], GPoint(26, yloc));

         // display no AM/PM
         set_bitmap_image(&tz2_digits_image[6], tz2_digits_layer[6], RESOURCE_ID_IMAGE_DAY_TIME_NONE, GPoint(114, yloc + 18));
      }
      else
      {
         set_bitmap_image(&tz2_digits_image[0], tz2_digits_layer[0], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time2.tm_hour / 10], GPoint(0, yloc));
         set_bitmap_image(&tz2_digits_image[1], tz2_digits_layer[1], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time2.tm_hour % 10], GPoint(26, yloc));

         // display no AM/PM
         set_bitmap_image(&tz2_digits_image[6], tz2_digits_layer[6], RESOURCE_ID_IMAGE_NIGHT_TIME_NONE, GPoint(114, yloc + 18));
      }
   }
   else
   {
      // display AM/PM
      if (current_time2.tm_hour >= 12)
      {
         if (current_time2.tm_hour <= 18)
         {
            set_bitmap_image(&tz2_digits_image[6], tz2_digits_layer[6], RESOURCE_ID_IMAGE_DAY_TIME_PM, GPoint(114, yloc + 18));
         }
         else
         {
            set_bitmap_image(&tz2_digits_image[6], tz2_digits_layer[6], RESOURCE_ID_IMAGE_NIGHT_TIME_PM, GPoint(114, yloc + 18));
         }
      }
      else
      {
         if (current_time2.tm_hour >= 6)
         {
            set_bitmap_image(&tz2_digits_image[6], tz2_digits_layer[6], RESOURCE_ID_IMAGE_DAY_TIME_AM, GPoint(114, yloc + 18));
         }
         else
         {
            set_bitmap_image(&tz2_digits_image[6], tz2_digits_layer[6], RESOURCE_ID_IMAGE_NIGHT_TIME_AM, GPoint(114, yloc + 18));
         }
      }

      if ((current_time2.tm_hour % 12) == 0)
      {
         if ((current_time2.tm_hour >= 6) && (current_time2.tm_hour <= 18))
         {
            set_bitmap_image(&tz2_digits_image[0], tz2_digits_layer[0], DAY_TIME_IMAGE_RESOURCE_IDS[1], GPoint(0, yloc));
            set_bitmap_image(&tz2_digits_image[1], tz2_digits_layer[1], DAY_TIME_IMAGE_RESOURCE_IDS[2], GPoint(26, yloc));
         }
         else
         {
            set_bitmap_image(&tz2_digits_image[0], tz2_digits_layer[0], NIGHT_TIME_IMAGE_RESOURCE_IDS[1], GPoint(0, yloc));
            set_bitmap_image(&tz2_digits_image[1], tz2_digits_layer[1], NIGHT_TIME_IMAGE_RESOURCE_IDS[2], GPoint(26, yloc));
         }
      }
      else
      {
         if ((current_time2.tm_hour >= 6) && (current_time2.tm_hour <= 18))
         {
            set_bitmap_image(&tz2_digits_image[0], tz2_digits_layer[0], DAY_TIME_IMAGE_RESOURCE_IDS[(current_time2.tm_hour % 12) / 10], GPoint(0, yloc));
            set_bitmap_image(&tz2_digits_image[1], tz2_digits_layer[1], DAY_TIME_IMAGE_RESOURCE_IDS[(current_time2.tm_hour % 12) % 10], GPoint(26, yloc));
         }
         else
         {
            set_bitmap_image(&tz2_digits_image[0], tz2_digits_layer[0], NIGHT_TIME_IMAGE_RESOURCE_IDS[(current_time2.tm_hour % 12) / 10], GPoint(0, yloc));
            set_bitmap_image(&tz2_digits_image[1], tz2_digits_layer[1], NIGHT_TIME_IMAGE_RESOURCE_IDS[(current_time2.tm_hour % 12) % 10], GPoint(26, yloc));
         }

         if ((current_time2.tm_hour % 12) < 10)
         {
            if ((current_time2.tm_hour >= 6) && (current_time2.tm_hour <= 18))
            {
               set_bitmap_image(&tz2_digits_image[0], tz2_digits_layer[0], RESOURCE_ID_IMAGE_DAY_TIME_BLANK, GPoint(0, yloc));
            }
            else
            {
               set_bitmap_image(&tz2_digits_image[0], tz2_digits_layer[0], RESOURCE_ID_IMAGE_NIGHT_TIME_BLANK, GPoint(0, yloc));
            }
         }
      }
   }

   // display tz2 colon & minutes
   if ((current_time2.tm_hour >= 6) && (current_time2.tm_hour <= 18))
   {
      set_bitmap_image(&tz2_digits_image[2], tz2_digits_layer[2], RESOURCE_ID_IMAGE_DAY_TIME_COLON, GPoint(52, yloc));
      set_bitmap_image(&tz2_digits_image[3], tz2_digits_layer[3], DAY_TIME_IMAGE_RESOURCE_IDS[current_time2.tm_min / 10], GPoint(62, yloc));
      set_bitmap_image(&tz2_digits_image[4], tz2_digits_layer[4], DAY_TIME_IMAGE_RESOURCE_IDS[current_time2.tm_min % 10], GPoint(88, yloc));
   }
   else
   {
      set_bitmap_image(&tz2_digits_image[2], tz2_digits_layer[2], RESOURCE_ID_IMAGE_NIGHT_TIME_COLON, GPoint(52, yloc));
      set_bitmap_image(&tz2_digits_image[3], tz2_digits_layer[3], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time2.tm_min / 10], GPoint(62, yloc));
      set_bitmap_image(&tz2_digits_image[4], tz2_digits_layer[4], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time2.tm_min % 10], GPoint(88, yloc));
   }
}  // display_tz2()


static void display_tz3(void)
{
   int yloc = 132;

   current_time3.tm_min += tz3_offset_mins;

   if (current_time3.tm_min >= 60)
   {
      current_time3.tm_min %= 60;

      current_time3.tm_hour += 1;
   }
   else
   {
      if (current_time3.tm_min < 0)
      {
         current_time3.tm_min += 60;

         current_time3.tm_hour -= 1;
      }
   }

   current_time3.tm_hour += tz3_offset_hours;

   if (current_time3.tm_hour >= 24)
   {
      current_time3.tm_hour %= 24;

      if ((current_time3.tm_hour >= 6) && (current_time3.tm_hour <= 18))
      {
         // display + offset
         set_bitmap_image(&tz3_digits_image[5], tz3_digits_layer[5], RESOURCE_ID_IMAGE_DAY_TIME_PLUS, GPoint(114, yloc));
      }
      else
      {
         // display + offset
         set_bitmap_image(&tz3_digits_image[5], tz3_digits_layer[5], RESOURCE_ID_IMAGE_NIGHT_TIME_PLUS, GPoint(114, yloc));
      }
   }
   else
   {
      if (current_time3.tm_hour < 0)
      {
         current_time3.tm_hour += 24;

         if ((current_time3.tm_hour >= 6) && (current_time3.tm_hour <= 18))
         {
            // display - offset
            set_bitmap_image(&tz3_digits_image[5], tz3_digits_layer[5], RESOURCE_ID_IMAGE_DAY_TIME_MINUS, GPoint(114, yloc));
         }
         else
         {
            // display - offset
            set_bitmap_image(&tz3_digits_image[5], tz3_digits_layer[5], RESOURCE_ID_IMAGE_NIGHT_TIME_MINUS, GPoint(114, yloc));
         }
      }
      else
      {
         if ((current_time3.tm_hour >= 6) && (current_time3.tm_hour <= 18))
         {
            // display no offset
            set_bitmap_image(&tz3_digits_image[5], tz3_digits_layer[5], RESOURCE_ID_IMAGE_DAY_TIME_NONE, GPoint(114, yloc));
         }
         else
         {
            // display no offset
            set_bitmap_image(&tz3_digits_image[5], tz3_digits_layer[5], RESOURCE_ID_IMAGE_NIGHT_TIME_NONE, GPoint(114, yloc));
         }
      }
   }

   // display tz3 name
   if (app_state == APP_SET_TZ3_STATE)
   {
      if (toggle_flag == false)
      {
         set_bitmap_image(&tz3_name_image, tz3_name_layer, timezones[tz3_index].day_image_id, GPoint(0, yloc - 20));
      }
      else
      {
         set_bitmap_image(&tz3_name_image, tz3_name_layer, timezones[tz3_index].night_image_id, GPoint(0, yloc - 20));
      }
   }
   else
   {
      if ((current_time3.tm_hour >= 6) && (current_time3.tm_hour <= 18))
      {
         set_bitmap_image(&tz3_name_image, tz3_name_layer, timezones[tz3_index].day_image_id, GPoint(0, yloc - 20));
      }
      else
      {
         set_bitmap_image(&tz3_name_image, tz3_name_layer, timezones[tz3_index].night_image_id, GPoint(0, yloc - 20));
      }
   }

   // display time hour
   if (clock_is_24h_style())
   {
      if ((current_time3.tm_hour >= 6) && (current_time3.tm_hour <= 18))
      {
         set_bitmap_image(&tz3_digits_image[0], tz3_digits_layer[0], DAY_TIME_IMAGE_RESOURCE_IDS[current_time3.tm_hour / 10], GPoint(0, yloc));
         set_bitmap_image(&tz3_digits_image[1], tz3_digits_layer[1], DAY_TIME_IMAGE_RESOURCE_IDS[current_time3.tm_hour % 10], GPoint(26, yloc));

         // display no AM/PM
         set_bitmap_image(&tz3_digits_image[6], tz3_digits_layer[6], RESOURCE_ID_IMAGE_DAY_TIME_NONE, GPoint(114, yloc + 18));
      }
      else
      {
         set_bitmap_image(&tz3_digits_image[0], tz3_digits_layer[0], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time3.tm_hour / 10], GPoint(0, yloc));
         set_bitmap_image(&tz3_digits_image[1], tz3_digits_layer[1], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time3.tm_hour % 10], GPoint(26, yloc));

         // display no AM/PM
         set_bitmap_image(&tz3_digits_image[6], tz3_digits_layer[6], RESOURCE_ID_IMAGE_NIGHT_TIME_NONE, GPoint(114, yloc + 18));
      }
   }
   else
   {
      // display AM/PM
      if (current_time3.tm_hour >= 12)
      {
         if (current_time3.tm_hour <= 18)
         {
            set_bitmap_image(&tz3_digits_image[6], tz3_digits_layer[6], RESOURCE_ID_IMAGE_DAY_TIME_PM, GPoint(114, yloc + 18));
         }
         else
         {
            set_bitmap_image(&tz3_digits_image[6], tz3_digits_layer[6], RESOURCE_ID_IMAGE_NIGHT_TIME_PM, GPoint(114, yloc + 18));
         }
      }
      else
      {
         if (current_time3.tm_hour >= 6)
         {
            set_bitmap_image(&tz3_digits_image[6], tz3_digits_layer[6], RESOURCE_ID_IMAGE_DAY_TIME_AM, GPoint(114, yloc + 18));
         }
         else
         {
            set_bitmap_image(&tz3_digits_image[6], tz3_digits_layer[6], RESOURCE_ID_IMAGE_NIGHT_TIME_AM, GPoint(114, yloc + 18));
         }
      }

      if ((current_time3.tm_hour % 12) == 0)
      {
         if ((current_time3.tm_hour >= 6) && (current_time3.tm_hour <= 18))
         {
            set_bitmap_image(&tz3_digits_image[0], tz3_digits_layer[0], DAY_TIME_IMAGE_RESOURCE_IDS[1], GPoint(0, yloc));
            set_bitmap_image(&tz3_digits_image[1], tz3_digits_layer[1], DAY_TIME_IMAGE_RESOURCE_IDS[2], GPoint(26, yloc));
         }
         else
         {
            set_bitmap_image(&tz3_digits_image[0], tz3_digits_layer[0], NIGHT_TIME_IMAGE_RESOURCE_IDS[1], GPoint(0, yloc));
            set_bitmap_image(&tz3_digits_image[1], tz3_digits_layer[1], NIGHT_TIME_IMAGE_RESOURCE_IDS[2], GPoint(26, yloc));
         }
      }
      else
      {
         if ((current_time3.tm_hour >= 6) && (current_time3.tm_hour <= 18))
         {
            set_bitmap_image(&tz3_digits_image[0], tz3_digits_layer[0], DAY_TIME_IMAGE_RESOURCE_IDS[(current_time3.tm_hour % 12) / 10], GPoint(0, yloc));
            set_bitmap_image(&tz3_digits_image[1], tz3_digits_layer[1], DAY_TIME_IMAGE_RESOURCE_IDS[(current_time3.tm_hour % 12) % 10], GPoint(26, yloc));
         }
         else
         {
            set_bitmap_image(&tz3_digits_image[0], tz3_digits_layer[0], NIGHT_TIME_IMAGE_RESOURCE_IDS[(current_time3.tm_hour % 12) / 10], GPoint(0, yloc));
            set_bitmap_image(&tz3_digits_image[1], tz3_digits_layer[1], NIGHT_TIME_IMAGE_RESOURCE_IDS[(current_time3.tm_hour % 12) % 10], GPoint(26, yloc));
         }

         if ((current_time3.tm_hour % 12) < 10)
         {
            if ((current_time3.tm_hour >= 6) && (current_time3.tm_hour <= 18))
            {
               set_bitmap_image(&tz3_digits_image[0], tz3_digits_layer[0], RESOURCE_ID_IMAGE_DAY_TIME_BLANK, GPoint(0, yloc));
            }
            else
            {
               set_bitmap_image(&tz3_digits_image[0], tz3_digits_layer[0], RESOURCE_ID_IMAGE_NIGHT_TIME_BLANK, GPoint(0, yloc));
            }
         }
      }
   }

   // display tz3 colon & minutes
   if ((current_time3.tm_hour >= 6) && (current_time3.tm_hour <= 18))
   {
      set_bitmap_image(&tz3_digits_image[2], tz3_digits_layer[2], RESOURCE_ID_IMAGE_DAY_TIME_COLON, GPoint(52, yloc));
      set_bitmap_image(&tz3_digits_image[3], tz3_digits_layer[3], DAY_TIME_IMAGE_RESOURCE_IDS[current_time3.tm_min / 10], GPoint(62, yloc));
      set_bitmap_image(&tz3_digits_image[4], tz3_digits_layer[4], DAY_TIME_IMAGE_RESOURCE_IDS[current_time3.tm_min % 10], GPoint(88, yloc));
   }
   else
   {
      set_bitmap_image(&tz3_digits_image[2], tz3_digits_layer[2], RESOURCE_ID_IMAGE_NIGHT_TIME_COLON, GPoint(52, yloc));
      set_bitmap_image(&tz3_digits_image[3], tz3_digits_layer[3], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time3.tm_min / 10], GPoint(62, yloc));
      set_bitmap_image(&tz3_digits_image[4], tz3_digits_layer[4], NIGHT_TIME_IMAGE_RESOURCE_IDS[current_time3.tm_min % 10], GPoint(88, yloc));
   }
}  // display_tz3()


static void down_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
   switch (app_state)
   {
      case APP_SET_LOCAL_STATE:
         if (local_index < (NUM_TIMEZONES - 1))
         {
            local_index++;

            // save local_index into persistent storage
            persist_write_int(PKEY_LOCAL_INDEX, local_index);

            local_offset_hours = timezones[local_index].offset_hours;
            local_offset_mins = timezones[local_index].offset_mins;
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ1_STATE:
         if (tz1_index < (NUM_TIMEZONES - 1))
         {
            tz1_index++;

            // save tz1_index into persistent storage
            persist_write_int(PKEY_TZ1_INDEX, tz1_index);

            tz1_offset_hours = timezones[tz1_index].offset_hours;
            tz1_offset_mins = timezones[tz1_index].offset_mins;
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ2_STATE:
         if (tz2_index < (NUM_TIMEZONES - 1))
         {
            tz2_index++;

            // save tz2_index into persistent storage
            persist_write_int(PKEY_TZ2_INDEX, tz2_index);

            tz2_offset_hours = timezones[tz2_index].offset_hours;
            tz2_offset_mins = timezones[tz2_index].offset_mins;
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ3_STATE:
         if (tz3_index < (NUM_TIMEZONES - 1))
         {
            tz3_index++;

            // save tz3_index into persistent storage
            persist_write_int(PKEY_TZ3_INDEX, tz3_index);

            tz3_offset_hours = timezones[tz3_index].offset_hours;
            tz3_offset_mins = timezones[tz3_index].offset_mins;
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      default:
         break;
   }
}  // down_single_click_handler()


static void handle_accel_tap(AccelAxisType axis, int32_t direction)
{
   light_on = !light_on;

   if (light_on)
   {
      light_enable(true);
      light_timer = LIGHT_TIMER_SECONDS;
   }
   else
   {
      light_enable(false);
      light_timer = 0;
   }
}  // accel_tap_handler()


void handle_second_tick(struct tm *tick_time, TimeUnits units_changed)
{
   time_t t = time(NULL);
   struct tm *current_time = localtime(&t);

   if (splash_timer > 0)
   {
      splash_timer--;

      if (splash_timer == 0)
      {
         set_bitmap_image(&splash_image, splash_layer, RESOURCE_ID_IMAGE_WHITE_BACK, GPoint (0, 0));
      }
   }
   else
   {
      if (light_timer > 0)
      {
         light_timer--;

         if (light_timer == 0)
         {
            light_enable(false);
            light_on = false;
         }
      }

      if (app_state != APP_IDLE_STATE)
      {
         if (setmode_timer > 0)
         {
            setmode_timer--;

            if (setmode_timer == 0)
            {
               app_state = APP_IDLE_STATE;
            }
         }
      }

      if (switch_seconds != 0)
      {
         switch_seconds--;
      }
   }

   update_display(current_time);
}  // handle_second_tick()


static void init(void)
{
   time_t t = time(NULL);
   struct tm *current_time = localtime(&t);

   GRect dummy_frame = { {0, 0}, {0, 0} };

   window = window_create();

   if (window == NULL)
   {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "...couldn't allocate window memory...");
      return;
   }

   // Get all settings from persistent storage for use if they exist, otherwise use the default
   display_is_local = persist_exists(PKEY_DISPLAY_IS_LOCAL) ? persist_read_int(PKEY_DISPLAY_IS_LOCAL) : DISPLAY_IS_LOCAL_DEFAULT;
   local_index = persist_exists(PKEY_LOCAL_INDEX) ? persist_read_int(PKEY_LOCAL_INDEX) : LOCAL_INDEX_DEFAULT;
   tz1_index = persist_exists(PKEY_TZ1_INDEX) ? persist_read_int(PKEY_TZ1_INDEX) : TZ1_INDEX_DEFAULT;
   tz2_index = persist_exists(PKEY_TZ2_INDEX) ? persist_read_int(PKEY_TZ2_INDEX) : TZ2_INDEX_DEFAULT;
   tz3_index = persist_exists(PKEY_TZ3_INDEX) ? persist_read_int(PKEY_TZ3_INDEX) : TZ3_INDEX_DEFAULT;

   window_set_fullscreen(window, true);
   window_stack_push(window, true /* Animated */);
   Layer *window_layer = window_get_root_layer(window);

   window_set_click_config_provider(window, click_config_provider);

   if ((current_time->tm_hour >= 6) && (current_time->tm_hour <= 18))
   {
      splash_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SPLASH);
   }
   else
   {
      splash_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_INV_SPLASH);
   }
   splash_layer = bitmap_layer_create(dummy_frame);
   bitmap_layer_set_bitmap(splash_layer, splash_image);
   layer_add_child(window_layer, bitmap_layer_get_layer(splash_layer));
 
   tz1_name_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DAY_UTC); 
   tz1_name_layer = bitmap_layer_create(dummy_frame);
   bitmap_layer_set_bitmap(tz1_name_layer, tz1_name_image);
   layer_add_child(window_layer, bitmap_layer_get_layer(tz1_name_layer));

   tz2_name_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DAY_UTC); 
   tz2_name_layer = bitmap_layer_create(dummy_frame);
   bitmap_layer_set_bitmap(tz2_name_layer, tz2_name_image);
   layer_add_child(window_layer, bitmap_layer_get_layer(tz2_name_layer));

   tz3_name_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DAY_UTC); 
   tz3_name_layer = bitmap_layer_create(dummy_frame);
   bitmap_layer_set_bitmap(tz3_name_layer, tz3_name_image);
   layer_add_child(window_layer, bitmap_layer_get_layer(tz3_name_layer));

   for (int i = 0; i < TOTAL_IMAGES; i++)
   {
      tz1_digits_image[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DAY_TIME_0); 
      tz1_digits_layer[i] = bitmap_layer_create(dummy_frame);
      bitmap_layer_set_bitmap(tz1_digits_layer[i], tz1_digits_image[i]);
      layer_add_child(window_layer, bitmap_layer_get_layer(tz1_digits_layer[i]));

      tz2_digits_image[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DAY_TIME_0); 
      tz2_digits_layer[i] = bitmap_layer_create(dummy_frame);
      bitmap_layer_set_bitmap(tz2_digits_layer[i], tz2_digits_image[i]);
      layer_add_child(window_layer, bitmap_layer_get_layer(tz2_digits_layer[i]));

      tz3_digits_image[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DAY_TIME_0); 
      tz3_digits_layer[i] = bitmap_layer_create(dummy_frame);
      bitmap_layer_set_bitmap(tz3_digits_layer[i], tz3_digits_image[i]);
      layer_add_child(window_layer, bitmap_layer_get_layer(tz3_digits_layer[i]));
   }

   if ((current_time->tm_hour >= 6) && (current_time->tm_hour <= 18))
   {
      set_bitmap_image(&splash_image, splash_layer, RESOURCE_ID_IMAGE_SPLASH, GPoint (0, 0));
   }
   else
   {
      set_bitmap_image(&splash_image, splash_layer, RESOURCE_ID_IMAGE_INV_SPLASH, GPoint (0, 0));
   }

   accel_tap_service_subscribe(&handle_accel_tap);
   tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);

   app_state = APP_IDLE_STATE;

   local_offset_hours = timezones[local_index].offset_hours;
   local_offset_mins = timezones[local_index].offset_mins;

   tz1_offset_hours = timezones [tz1_index].offset_hours;
   tz1_offset_mins = timezones [tz1_index].offset_mins;

   tz2_offset_hours = timezones [tz2_index].offset_hours;
   tz2_offset_mins = timezones [tz2_index].offset_mins;

   tz3_offset_hours = timezones [tz3_index].offset_hours;
   tz3_offset_mins = timezones [tz3_index].offset_mins;

   // kick initial display
   update_display(current_time);
}  // init()


static void select_long_click_handler(ClickRecognizerRef recognizer, void *context)
{
   if (app_state == APP_IDLE_STATE)
   {
      app_state = APP_SET_LOCAL_STATE;

      setmode_timer = SETMODE_SECONDS;

      switch_seconds = SWITCH_SECONDS;

      display_is_local = true;

      // save display_is_local into persistant storage
      persist_write_int(PKEY_DISPLAY_IS_LOCAL, display_is_local);
   }
   else
   {
      app_state = APP_IDLE_STATE;

      setmode_timer = 0;
   }
}  // select_long_click_handler()


static void select_long_release_handler(ClickRecognizerRef recognizer, void *context)
{
}  // select_long_release_handler()


static void select_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
   if (app_state != APP_IDLE_STATE)
   {
      app_state++;

      switch (app_state)
      {
         case APP_SET_TZ1_STATE:
            display_is_local = false;

            // save display_is_local into persistant storage
            persist_write_int(PKEY_DISPLAY_IS_LOCAL, display_is_local);

            switch_seconds = SWITCH_SECONDS;
            break;

         case STATE_COUNT:
            app_state = APP_SET_LOCAL_STATE;
            display_is_local = true;

            // save display_is_local into persistant storage
            persist_write_int(PKEY_DISPLAY_IS_LOCAL, display_is_local);

            switch_seconds = SWITCH_SECONDS;
            break;
      }

      setmode_timer = SETMODE_SECONDS;
   }
   else
   {
      if (light_timer == 4)
      {
         switch_seconds = SWITCH_SECONDS;

         display_is_local = !display_is_local;

         // save display_is_local into persistant storage
         persist_write_int(PKEY_DISPLAY_IS_LOCAL, display_is_local);

         light_on = true;
         light_enable(true);
      }
      else
      {
         light_timer = 4;

         light_on = !light_on;

         if (light_on)
         {
            light_enable(true);
         }
         else
         {
            light_enable(false);
         }
      }
   }
}  // select_single_click_handler()


static void set_bitmap_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint this_origin)
{
   gbitmap_destroy(*bmp_image);

   *bmp_image = gbitmap_create_with_resource(resource_id);
   GRect frame = (GRect)
   {
      .origin = this_origin,
      .size = (*bmp_image)->bounds.size
   };
   bitmap_layer_set_compositing_mode(bmp_layer, GCompOpAssign);
   layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);
   bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
}  // set_bitmap_image()


static void up_single_click_handler(ClickRecognizerRef recognizer, void *context)
{
   switch (app_state)
   {
      case APP_SET_LOCAL_STATE:
         if (local_index > 0)
         {
            local_index--;

            // save local_index into persistent storage
            persist_write_int(PKEY_LOCAL_INDEX, local_index);

            local_offset_hours = timezones[local_index].offset_hours;
            local_offset_mins = timezones[local_index].offset_mins;
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ1_STATE:
         if (tz1_index > 0)
         {
            tz1_index--;

            // save tz1_index into persistent storage
            persist_write_int(PKEY_TZ1_INDEX, tz1_index);

            tz1_offset_hours = timezones[tz1_index].offset_hours;
            tz1_offset_mins = timezones[tz1_index].offset_mins;
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ2_STATE:
         if (tz2_index > 0)
         {
            tz2_index--;

            // save tz2_index into persistent storage
            persist_write_int(PKEY_TZ2_INDEX, tz2_index);

            tz2_offset_hours = timezones[tz2_index].offset_hours;
            tz2_offset_mins = timezones[tz2_index].offset_mins;
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ3_STATE:
         if (tz3_index > 0)
         {
            tz3_index--;

            // save tz3_index into persistent storage
            persist_write_int(PKEY_TZ3_INDEX, tz3_index);

            tz3_offset_hours = timezones[tz3_index].offset_hours;
            tz3_offset_mins = timezones[tz3_index].offset_mins;
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      default:
         break;
   }
}  // up_single_click_handler()


static void update_display(struct tm *current_time)
{
   toggle_flag = !toggle_flag;

   current_time->tm_min -= local_offset_mins;

   if (current_time->tm_min < 0)
   {
      current_time->tm_min += 60;

      current_time->tm_hour -= 1;
   }
   else
   {
      if (current_time->tm_min >= 60)
      {
         current_time->tm_min %= 60;

         current_time->tm_hour += 1;
      }
   }

   current_time->tm_hour -= local_offset_hours;

   if (current_time->tm_hour < 0)
   {
      current_time->tm_hour += 24;
   }
   else
   {
      if (current_time->tm_hour >= 24)
      {
         current_time->tm_hour %= 24;
      }
   }

    if (splash_timer == 0)
    {
      if (display_is_local == true)
      {
         display_local();
      }
      else
      {
         display_tz1();
      }

      display_tz2();
      display_tz3();
    }

   current_time_local = *current_time;
   current_time1 = *current_time;
   current_time2 = *current_time;
   current_time3 = *current_time;

   Layer *window_layer = window_get_root_layer(window);
   layer_mark_dirty(window_layer);
}  // update_display()


int main(void)
{
   init();
   app_event_loop();
   deinit();
}  // main()









