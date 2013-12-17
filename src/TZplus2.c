/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* *                                                                 * */
/* *                            TZplus2                              * */
/* *                                                                 * */
/* *      A watchface to display three timezones + local time        * */
/* *                                                                 * */
/* *                 [ SDK 2.0 compatible version ]                  * */
/* *                                                                 * */
/* *                    by Mark J Culross, KD5RXT                    * */
/* *                                                                 * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <pebble.h>

static Window *window;
static Layer *window_layer;

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

int setmode_timer = SETMODE_SECONDS;
int switch_seconds = SWITCH_SECONDS;
int light_timer = LIGHT_TIMER_SECONDS;

bool toggle_flag = false;
bool display_is_local = DISPLAY_IS_LOCAL_DEFAULT;

bool light_on = false;

int local_index = LOCAL_INDEX_DEFAULT;
int tz1_index = TZ1_INDEX_DEFAULT;
int tz2_index = TZ2_INDEX_DEFAULT;
int tz3_index = TZ3_INDEX_DEFAULT;

int app_state = APP_IDLE_STATE;
int splash_timer = 5;

GBitmap *tz1_name_image;
GBitmap *tz2_name_image;
GBitmap *tz3_name_image;
GBitmap *tz1_digits_image[TOTAL_IMAGES];
GBitmap *tz2_digits_image[TOTAL_IMAGES];
GBitmap *tz3_digits_image[TOTAL_IMAGES];
GBitmap *splash_image;

BitmapLayer *splash_layer;

const int TIME_IMAGE_RESOURCE_IDS[] =
{
   RESOURCE_ID_IMAGE_TIME_0,
   RESOURCE_ID_IMAGE_TIME_1,
   RESOURCE_ID_IMAGE_TIME_2,
   RESOURCE_ID_IMAGE_TIME_3,
   RESOURCE_ID_IMAGE_TIME_4,
   RESOURCE_ID_IMAGE_TIME_5,
   RESOURCE_ID_IMAGE_TIME_6,
   RESOURCE_ID_IMAGE_TIME_7,
   RESOURCE_ID_IMAGE_TIME_8,
   RESOURCE_ID_IMAGE_TIME_9,
};

typedef struct
{
   int image_id;
   int offset_hours;
   int offset_mins;
} timezone_t;


timezone_t timezones[] =
{
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_11,      .offset_hours = -11,  .offset_mins =   0 },  // 000
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_10,      .offset_hours = -10,  .offset_mins =   0 },  // 001
   { .image_id = RESOURCE_ID_IMAGE_HAWAII,            .offset_hours = -10,  .offset_mins =   0 },  // 002
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_9,       .offset_hours =  -9,  .offset_mins =   0 },  // 003
   { .image_id = RESOURCE_ID_IMAGE_ALASKA,            .offset_hours =  -9,  .offset_mins =   0 },  // 004
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_8,       .offset_hours =  -8,  .offset_mins =   0 },  // 005
   { .image_id = RESOURCE_ID_IMAGE_PST,               .offset_hours =  -8,  .offset_mins =   0 },  // 006
   { .image_id = RESOURCE_ID_IMAGE_LOSANGELES,        .offset_hours =  -8,  .offset_mins =   0 },  // 007
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_7,       .offset_hours =  -7,  .offset_mins =   0 },  // 008
   { .image_id = RESOURCE_ID_IMAGE_PDT,               .offset_hours =  -7,  .offset_mins =   0 },  // 009
   { .image_id = RESOURCE_ID_IMAGE_MST,               .offset_hours =  -7,  .offset_mins =   0 },  // 010
   { .image_id = RESOURCE_ID_IMAGE_DENVER,            .offset_hours =  -7,  .offset_mins =   0 },  // 011
   { .image_id = RESOURCE_ID_IMAGE_PHOENIX,           .offset_hours =  -7,  .offset_mins =   0 },  // 012
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_6,       .offset_hours =  -6,  .offset_mins =   0 },  // 013
   { .image_id = RESOURCE_ID_IMAGE_MDT,               .offset_hours =  -6,  .offset_mins =   0 },  // 014
   { .image_id = RESOURCE_ID_IMAGE_CST,               .offset_hours =  -6,  .offset_mins =   0 },  // 015
   { .image_id = RESOURCE_ID_IMAGE_ATLANTA,           .offset_hours =  -6,  .offset_mins =   0 },  // 016
   { .image_id = RESOURCE_ID_IMAGE_DALLAS,            .offset_hours =  -6,  .offset_mins =   0 },  // 017
   { .image_id = RESOURCE_ID_IMAGE_STLOUIS,           .offset_hours =  -6,  .offset_mins =   0 },  // 018
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_5,       .offset_hours =  -5,  .offset_mins =   0 },  // 019
   { .image_id = RESOURCE_ID_IMAGE_CDT,               .offset_hours =  -5,  .offset_mins =   0 },  // 020
   { .image_id = RESOURCE_ID_IMAGE_EST,               .offset_hours =  -5,  .offset_mins =   0 },  // 021
   { .image_id = RESOURCE_ID_IMAGE_DETROIT,           .offset_hours =  -5,  .offset_mins =   0 },  // 022
   { .image_id = RESOURCE_ID_IMAGE_MIAMI,             .offset_hours =  -5,  .offset_mins =   0 },  // 023
   { .image_id = RESOURCE_ID_IMAGE_NEWYORK,           .offset_hours =  -5,  .offset_mins =   0 },  // 024
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_4_30,    .offset_hours =  -4,  .offset_mins = -30 },  // 025
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_4,       .offset_hours =  -4,  .offset_mins =   0 },  // 026
   { .image_id = RESOURCE_ID_IMAGE_EDT,               .offset_hours =  -4,  .offset_mins =   0 },  // 027
   { .image_id = RESOURCE_ID_IMAGE_SANJUAN,           .offset_hours =  -4,  .offset_mins =   0 },  // 028
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_3_30,    .offset_hours =  -3,  .offset_mins = -30 },  // 029
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_3,       .offset_hours =  -3,  .offset_mins =   0 },  // 030
   { .image_id = RESOURCE_ID_IMAGE_GREENLAND,         .offset_hours =  -3,  .offset_mins =   0 },  // 031
   { .image_id = RESOURCE_ID_IMAGE_SALVADOR,          .offset_hours =  -3,  .offset_mins =   0 },  // 032
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_2,       .offset_hours =  -2,  .offset_mins =   0 },  // 033
   { .image_id = RESOURCE_ID_IMAGE_UTC_MINUS_1,       .offset_hours =  -1,  .offset_mins =   0 },  // 034
   { .image_id = RESOURCE_ID_IMAGE_AZORES,            .offset_hours =  -1,  .offset_mins =   0 },  // 035
   { .image_id = RESOURCE_ID_IMAGE_UTC,               .offset_hours =   0,  .offset_mins =   0 },  // 036
   { .image_id = RESOURCE_ID_IMAGE_EDINBURGH,         .offset_hours =   0,  .offset_mins =   0 },  // 037
   { .image_id = RESOURCE_ID_IMAGE_LONDON,            .offset_hours =   0,  .offset_mins =   0 },  // 038
   { .image_id = RESOURCE_ID_IMAGE_REYKJAVIK,         .offset_hours =   0,  .offset_mins =   0 },  // 039
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_1,        .offset_hours =  +1,  .offset_mins =   0 },  // 040
   { .image_id = RESOURCE_ID_IMAGE_AMSTERDAM,         .offset_hours =  +1,  .offset_mins =   0 },  // 041
   { .image_id = RESOURCE_ID_IMAGE_BERLIN,            .offset_hours =  +1,  .offset_mins =   0 },  // 042
   { .image_id = RESOURCE_ID_IMAGE_ROME,              .offset_hours =  +1,  .offset_mins =   0 },  // 043
   { .image_id = RESOURCE_ID_IMAGE_STOCKHOLM,         .offset_hours =  +1,  .offset_mins =   0 },  // 044
   { .image_id = RESOURCE_ID_IMAGE_BRUSSELS,          .offset_hours =  +1,  .offset_mins =   0 },  // 045
   { .image_id = RESOURCE_ID_IMAGE_COPENHAGEN,        .offset_hours =  +1,  .offset_mins =   0 },  // 046
   { .image_id = RESOURCE_ID_IMAGE_PARIS,             .offset_hours =  +1,  .offset_mins =   0 },  // 047
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_2,        .offset_hours =  +2,  .offset_mins =   0 },  // 048
   { .image_id = RESOURCE_ID_IMAGE_ATHENS,            .offset_hours =  +2,  .offset_mins =   0 },  // 049
   { .image_id = RESOURCE_ID_IMAGE_BEIRUT,            .offset_hours =  +2,  .offset_mins =   0 },  // 050
   { .image_id = RESOURCE_ID_IMAGE_CAIRO,             .offset_hours =  +2,  .offset_mins =   0 },  // 051
   { .image_id = RESOURCE_ID_IMAGE_HELSINKI,          .offset_hours =  +2,  .offset_mins =   0 },  // 052
   { .image_id = RESOURCE_ID_IMAGE_JERUSALEM,         .offset_hours =  +2,  .offset_mins =   0 },  // 053
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_3,        .offset_hours =  +3,  .offset_mins =   0 },  // 054
   { .image_id = RESOURCE_ID_IMAGE_BAGHDAD,           .offset_hours =  +3,  .offset_mins =   0 },  // 055
   { .image_id = RESOURCE_ID_IMAGE_KUWAIT,            .offset_hours =  +3,  .offset_mins =   0 },  // 056
   { .image_id = RESOURCE_ID_IMAGE_RIYADH,            .offset_hours =  +3,  .offset_mins =   0 },  // 057
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_3_30,     .offset_hours =  +3,  .offset_mins = +30 },  // 058
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_4,        .offset_hours =  +4,  .offset_mins =   0 },  // 059
   { .image_id = RESOURCE_ID_IMAGE_ABUDHABI,          .offset_hours =  +4,  .offset_mins =   0 },  // 060
   { .image_id = RESOURCE_ID_IMAGE_MOSCOW,            .offset_hours =  +4,  .offset_mins =   0 },  // 061
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_4_30,     .offset_hours =  +4,  .offset_mins = +30 },  // 062
   { .image_id = RESOURCE_ID_IMAGE_KABUL,             .offset_hours =  +4,  .offset_mins = +30 },  // 063
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_5,        .offset_hours =  +5,  .offset_mins =   0 },  // 064
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_5_30,     .offset_hours =  +5,  .offset_mins = +30 },  // 065
   { .image_id = RESOURCE_ID_IMAGE_NEWDELHI,          .offset_hours =  +5,  .offset_mins = +30 },  // 066
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_5_45,     .offset_hours =  +5,  .offset_mins = +45 },  // 067
   { .image_id = RESOURCE_ID_IMAGE_KATHMANDU,         .offset_hours =  +5,  .offset_mins = +45 },  // 068
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_6,        .offset_hours =  +6,  .offset_mins =   0 },  // 069
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_6_30,     .offset_hours =  +6,  .offset_mins = +30 },  // 070
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_7,        .offset_hours =  +7,  .offset_mins =   0 },  // 071
   { .image_id = RESOURCE_ID_IMAGE_HANOI,             .offset_hours =  +7,  .offset_mins =   0 },  // 072
   { .image_id = RESOURCE_ID_IMAGE_JAKARTA,           .offset_hours =  +7,  .offset_mins =   0 },  // 073
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_8,        .offset_hours =  +8,  .offset_mins =   0 },  // 074
   { .image_id = RESOURCE_ID_IMAGE_BEIJING,           .offset_hours =  +8,  .offset_mins =   0 },  // 075
   { .image_id = RESOURCE_ID_IMAGE_HONGKONG,          .offset_hours =  +8,  .offset_mins =   0 },  // 076
   { .image_id = RESOURCE_ID_IMAGE_SINGAPORE,         .offset_hours =  +8,  .offset_mins =   0 },  // 077
   { .image_id = RESOURCE_ID_IMAGE_TAIPEI,            .offset_hours =  +8,  .offset_mins =   0 },  // 078
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_9,        .offset_hours =  +9,  .offset_mins =   0 },  // 079
   { .image_id = RESOURCE_ID_IMAGE_TOKYO,             .offset_hours =  +9,  .offset_mins =   0 },  // 080
   { .image_id = RESOURCE_ID_IMAGE_SEOUL,             .offset_hours =  +9,  .offset_mins =   0 },  // 081
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_9_30,     .offset_hours =  +9,  .offset_mins = +30 },  // 082
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_10,       .offset_hours =  +10, .offset_mins =   0 },  // 083
   { .image_id = RESOURCE_ID_IMAGE_SYDNEY,            .offset_hours =  +10, .offset_mins =   0 },  // 084
   { .image_id = RESOURCE_ID_IMAGE_GUAM,              .offset_hours =  +10, .offset_mins =   0 },  // 085
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_11,       .offset_hours =  +11, .offset_mins =   0 },  // 086
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_12,       .offset_hours =  +12, .offset_mins =   0 },  // 087
   { .image_id = RESOURCE_ID_IMAGE_FIJI,              .offset_hours =  +12, .offset_mins =   0 },  // 088
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_13,       .offset_hours =  +13, .offset_mins =   0 },  // 089
   { .image_id = RESOURCE_ID_IMAGE_SAMOA,             .offset_hours =  +13, .offset_mins =   0 },  // 090
   { .image_id = RESOURCE_ID_IMAGE_UTC_PLUS_14,       .offset_hours =  +14, .offset_mins =   0 },  // 091
};



static void click_config_provider(void *config);
static void deinit(void);
static void display_time(GContext *ctx, int yloc, int timeslot, int index);
static void down_single_click_handler(ClickRecognizerRef recognizer, void *context);
static void handle_accel_tap(AccelAxisType axis, int32_t direction);
static void handle_second_tick(struct tm *tick, TimeUnits units_changed);
static void init(void);
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context);
static void select_long_release_handler(ClickRecognizerRef recognizer, void *context);
static void select_single_click_handler(ClickRecognizerRef recognizer, void *context);
static void set_bitmap_image(GContext *ctx, GBitmap **bmp_image, const int resource_id, GPoint this_origin, bool invert);
static void up_single_click_handler(ClickRecognizerRef recognizer, void *context);
static void update_display(Layer *layer, GContext *ctx);


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

   layer_remove_from_parent(bitmap_layer_get_layer(splash_layer));
   bitmap_layer_destroy(splash_layer);
   gbitmap_destroy(splash_image);

   gbitmap_destroy(tz1_name_image); 
   gbitmap_destroy(tz2_name_image); 
   gbitmap_destroy(tz3_name_image); 

   for (int i = 0; i < TOTAL_IMAGES; i++)
   {
      gbitmap_destroy(tz1_digits_image[i]); 
      gbitmap_destroy(tz2_digits_image[i]); 
      gbitmap_destroy(tz3_digits_image[i]); 
   }

   tick_timer_service_unsubscribe();
   accel_tap_service_unsubscribe();
   window_destroy(window);
}  // deinit()


static void display_time(GContext *ctx, int yloc, int timeslot, int index)
{
   time_t t = time(NULL);
   struct tm *current_time = localtime(&t);

   int resource_id_1;
   int resource_id_2;
   int resource_id_3;
   bool night_mode;

   current_time->tm_min -= timezones[local_index].offset_mins;

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

   current_time->tm_hour -= timezones[local_index].offset_hours;

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

   current_time->tm_min += timezones[index].offset_mins;

   if (current_time->tm_min >= 60)
   {
      current_time->tm_min %= 60;

      current_time->tm_hour += 1;
   }
   else
   {
      if (current_time->tm_min < 0)
      {
         current_time->tm_min += 60;

         current_time->tm_hour -= 1;
      }
   }

   current_time->tm_hour += timezones[index].offset_hours;

   if (current_time->tm_hour >= 24)
   {
      current_time->tm_hour %= 24;

      // display + offset
      resource_id_1 = RESOURCE_ID_IMAGE_TIME_PLUS;
   }
   else
   {
      if (current_time->tm_hour < 0)
      {
         current_time->tm_hour += 24;

         // display - offset
         resource_id_1 = RESOURCE_ID_IMAGE_TIME_MINUS;
      }
      else
      {
         // display no offset
         resource_id_1 = RESOURCE_ID_IMAGE_TIME_NONE;
      }
   }

   if ((current_time->tm_hour >= 6) && (current_time->tm_hour <= 18))
   {
      night_mode = false;
   }
   else
   {
      night_mode = true;
   }

   switch (timeslot)
   {
      case 1:
         set_bitmap_image(ctx, &tz1_digits_image[5], resource_id_1, GPoint(114, yloc), night_mode);
         break;

      case 2:
         set_bitmap_image(ctx, &tz2_digits_image[5], resource_id_1, GPoint(114, yloc), night_mode);
         break;

      case 3:
         set_bitmap_image(ctx, &tz3_digits_image[5], resource_id_1, GPoint(114, yloc), night_mode);
         break;
   }

   // display name
   switch (timeslot)
   {
      case 1:
      {
         if (switch_seconds != 0)
         {
            if (display_is_local)
            {
               resource_id_1 = RESOURCE_ID_IMAGE_LOCAL;
            }
            else
            {
               resource_id_1 = timezones[index].image_id;
            }
         }
         else
         {
            resource_id_1 = timezones[index].image_id;
         }


         if ((app_state == APP_SET_LOCAL_STATE) || (app_state == APP_SET_TZ1_STATE))
         {
            night_mode = toggle_flag;
         }
         else
         {
            if ((current_time->tm_hour >= 6) && (current_time->tm_hour <= 18))
            {
               night_mode = false;
            }
            else
            {
               night_mode = true;
            }
         }
      }
      break;

      case 2:
      {
         resource_id_1 = timezones[index].image_id;

         if (app_state == APP_SET_TZ2_STATE)
         {
            night_mode = toggle_flag;
         }
         else
         {
            if ((current_time->tm_hour >= 6) && (current_time->tm_hour <= 18))
            {
               night_mode = false;
            }
            else
            {
               night_mode = true;
            }
         }
      }
      break;

      case 3:
      {
         resource_id_1 = timezones[index].image_id;

         if (app_state == APP_SET_TZ3_STATE)
         {
            night_mode = toggle_flag;
         }
         else
         {
            if ((current_time->tm_hour >= 6) && (current_time->tm_hour <= 18))
            {
               night_mode = false;
            }
            else
            {
               night_mode = true;
            }
         }
      }
      break;
   }

   switch (timeslot)
   {
      case 1:
         set_bitmap_image(ctx, &tz1_name_image, resource_id_1, GPoint(0, yloc - 20), night_mode);
         break;

      case 2:
         set_bitmap_image(ctx, &tz2_name_image, resource_id_1, GPoint(0, yloc - 20), night_mode);
         break;

      case 3:
         set_bitmap_image(ctx, &tz3_name_image, resource_id_1, GPoint(0, yloc - 20), night_mode);
         break;
   }

   // display time hour
   if (clock_is_24h_style())
   {
      resource_id_1 = TIME_IMAGE_RESOURCE_IDS[current_time->tm_hour / 10];
      resource_id_2 = TIME_IMAGE_RESOURCE_IDS[current_time->tm_hour % 10];

      // display no AM/PM
      resource_id_3 = RESOURCE_ID_IMAGE_TIME_NONE;
   }
   else
   {
      // display AM/PM
      if (current_time->tm_hour >= 12)
      {
         resource_id_3 = RESOURCE_ID_IMAGE_TIME_PM;
      }
      else
      {
         resource_id_3 = RESOURCE_ID_IMAGE_TIME_AM;
      }

      if ((current_time->tm_hour % 12) == 0)
      {
         resource_id_1 = TIME_IMAGE_RESOURCE_IDS[1];
         resource_id_2 = TIME_IMAGE_RESOURCE_IDS[2];
      }
      else
      {
         resource_id_1 = TIME_IMAGE_RESOURCE_IDS[(current_time->tm_hour % 12) / 10];
         resource_id_2 = TIME_IMAGE_RESOURCE_IDS[(current_time->tm_hour % 12) % 10];

         if ((current_time->tm_hour % 12) < 10)
         {
            resource_id_1 = RESOURCE_ID_IMAGE_TIME_BLANK;
         }
      }
   }

   if ((current_time->tm_hour >= 6) && (current_time->tm_hour <= 18))
   {
      night_mode = false;
   }
   else
   {
      night_mode = true;
   }

   switch (timeslot)
   {
      case 1:
      {
         set_bitmap_image(ctx, &tz1_digits_image[0], resource_id_1, GPoint(0, yloc), night_mode);
         set_bitmap_image(ctx, &tz1_digits_image[1], resource_id_2, GPoint(26, yloc), night_mode);

         // display AM/PM
         set_bitmap_image(ctx, &tz1_digits_image[6], resource_id_3, GPoint(114, yloc + 18), night_mode);

         // display local colon & minutes
         resource_id_1 = RESOURCE_ID_IMAGE_TIME_COLON;
         resource_id_2 = TIME_IMAGE_RESOURCE_IDS[current_time->tm_min / 10];
         resource_id_3 = TIME_IMAGE_RESOURCE_IDS[current_time->tm_min % 10];

         set_bitmap_image(ctx, &tz1_digits_image[2], resource_id_1, GPoint(52, yloc), night_mode);
         set_bitmap_image(ctx, &tz1_digits_image[3], resource_id_2, GPoint(62, yloc), night_mode);
         set_bitmap_image(ctx, &tz1_digits_image[4], resource_id_3, GPoint(88, yloc), night_mode);
      }
      break;

      case 2:
      {
         set_bitmap_image(ctx, &tz2_digits_image[0], resource_id_1, GPoint(0, yloc), night_mode);
         set_bitmap_image(ctx, &tz2_digits_image[1], resource_id_2, GPoint(26, yloc), night_mode);

         // display AM/PM
         set_bitmap_image(ctx, &tz2_digits_image[6], resource_id_3, GPoint(114, yloc + 18), night_mode);

         // display local colon & minutes
         resource_id_1 = RESOURCE_ID_IMAGE_TIME_COLON;
         resource_id_2 = TIME_IMAGE_RESOURCE_IDS[current_time->tm_min / 10];
         resource_id_3 = TIME_IMAGE_RESOURCE_IDS[current_time->tm_min % 10];

         set_bitmap_image(ctx, &tz2_digits_image[2], resource_id_1, GPoint(52, yloc), night_mode);
         set_bitmap_image(ctx, &tz2_digits_image[3], resource_id_2, GPoint(62, yloc), night_mode);
         set_bitmap_image(ctx, &tz2_digits_image[4], resource_id_3, GPoint(88, yloc), night_mode);
      }
      break;

      case 3:
      {
         set_bitmap_image(ctx, &tz3_digits_image[0], resource_id_1, GPoint(0, yloc), night_mode);
         set_bitmap_image(ctx, &tz3_digits_image[1], resource_id_2, GPoint(26, yloc), night_mode);

         // display AM/PM
         set_bitmap_image(ctx, &tz3_digits_image[6], resource_id_3, GPoint(114, yloc + 18), night_mode);

         // display local colon & minutes
         resource_id_1 = RESOURCE_ID_IMAGE_TIME_COLON;
         resource_id_2 = TIME_IMAGE_RESOURCE_IDS[current_time->tm_min / 10];
         resource_id_3 = TIME_IMAGE_RESOURCE_IDS[current_time->tm_min % 10];

         set_bitmap_image(ctx, &tz3_digits_image[2], resource_id_1, GPoint(52, yloc), night_mode);
         set_bitmap_image(ctx, &tz3_digits_image[3], resource_id_2, GPoint(62, yloc), night_mode);
         set_bitmap_image(ctx, &tz3_digits_image[4], resource_id_3, GPoint(88, yloc), night_mode);
      }
      break;
   }
}  // display_time()


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
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ1_STATE:
         if (tz1_index < (NUM_TIMEZONES - 1))
         {
            tz1_index++;

            // save tz1_index into persistent storage
            persist_write_int(PKEY_TZ1_INDEX, tz1_index);
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ2_STATE:
         if (tz2_index < (NUM_TIMEZONES - 1))
         {
            tz2_index++;

            // save tz2_index into persistent storage
            persist_write_int(PKEY_TZ2_INDEX, tz2_index);
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ3_STATE:
         if (tz3_index < (NUM_TIMEZONES - 1))
         {
            tz3_index++;

            // save tz3_index into persistent storage
            persist_write_int(PKEY_TZ3_INDEX, tz3_index);
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      default:
         break;
   }
}  // down_single_click_handler()


static void handle_accel_tap(AccelAxisType axis, int32_t direction)
{
   splash_timer = 0;

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
   if (splash_timer > 0)
   {
      splash_timer--;
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

   layer_mark_dirty(window_layer);
}  // handle_second_tick()


static void init(void)
{
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
   window_layer = window_get_root_layer(window);

   window_set_click_config_provider(window, click_config_provider);
   layer_set_update_proc(window_layer, update_display);

   splash_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SPLASH);
   splash_layer = bitmap_layer_create(dummy_frame);
   bitmap_layer_set_background_color(splash_layer, GColorClear);
   bitmap_layer_set_bitmap(splash_layer, splash_image);
   layer_add_child(window_layer, bitmap_layer_get_layer(splash_layer));
 
   tz1_name_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UTC); 
   tz2_name_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UTC); 
   tz3_name_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UTC); 

   for (int i = 0; i < TOTAL_IMAGES; i++)
   {
      tz1_digits_image[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIME_0); 
      tz2_digits_image[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIME_0); 
      tz3_digits_image[i] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIME_0); 
   }

   accel_tap_service_subscribe(&handle_accel_tap);
   tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);

   app_state = APP_IDLE_STATE;

   // kick initial display
   layer_mark_dirty(window_layer);
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
         splash_timer = 0;

         switch_seconds = SWITCH_SECONDS;

         display_is_local = !display_is_local;

         // save display_is_local into persistant storage
         persist_write_int(PKEY_DISPLAY_IS_LOCAL, display_is_local);

         light_on = true;
         light_enable(true);
      }
      else
      {
         splash_timer = 0;

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


static void set_bitmap_image(GContext *ctx, GBitmap **bmp_image, const int resource_id, GPoint this_origin, bool invert)
{
   gbitmap_destroy(*bmp_image);

   *bmp_image = gbitmap_create_with_resource(resource_id);

   GRect frame = (GRect)
   {
      .origin = this_origin,
      .size = (*bmp_image)->bounds.size
   };

   if (invert)
   {
      graphics_context_set_compositing_mode(ctx, GCompOpAssignInverted);
   }
   else
   {
      graphics_context_set_compositing_mode(ctx, GCompOpAssign);
   }

   graphics_draw_bitmap_in_rect(ctx, *bmp_image, frame);

   layer_mark_dirty(window_layer);
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
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ1_STATE:
         if (tz1_index > 0)
         {
            tz1_index--;

            // save tz1_index into persistent storage
            persist_write_int(PKEY_TZ1_INDEX, tz1_index);
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ2_STATE:
         if (tz2_index > 0)
         {
            tz2_index--;

            // save tz2_index into persistent storage
            persist_write_int(PKEY_TZ2_INDEX, tz2_index);
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      case APP_SET_TZ3_STATE:
         if (tz3_index > 0)
         {
            tz3_index--;

            // save tz3_index into persistent storage
            persist_write_int(PKEY_TZ3_INDEX, tz3_index);
         }

         setmode_timer = SETMODE_SECONDS;
         break;

      default:
         break;
   }
}  // up_single_click_handler()


static void update_display(Layer *layer, GContext *ctx)
{
   time_t t = time(NULL);
   struct tm *time_now = localtime(&t);

   if (splash_timer > 0)
   {
      if ((time_now->tm_hour < 6) || (time_now->tm_hour > 18))
      {
         set_bitmap_image(ctx, &splash_image, RESOURCE_ID_IMAGE_SPLASH, GPoint(0, 0), true);
      }
      else
      {
         set_bitmap_image(ctx, &splash_image, RESOURCE_ID_IMAGE_SPLASH, GPoint(0, 0), false);
      }
      return;
   }

   toggle_flag = !toggle_flag;

   if (splash_timer == 0)
   {
      if (display_is_local == true)
      {
         display_time(ctx, 20, 1, local_index);
      }
      else
      {
         display_time(ctx, 20, 1, tz1_index);
      }

      display_time(ctx, 76, 2, tz2_index);
      display_time(ctx, 132, 3, tz3_index);
   }

   layer_mark_dirty(window_layer);
}  // update_display()


int main(void)
{
   init();
   app_event_loop();
   deinit();
}  // main()









