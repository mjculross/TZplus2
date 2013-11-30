TZplus2 - a Pebble watchapp that displays 3
          selectable timezones

[ SDK 2.0 compatible version ]

NOTE: to allow full use of the buttons, TZplus2
loads as an app (at the same menu level as
"Settings" on the Pebble), not as a watchface
. . . it just displays a watchface

Corrections (v2.1):
- corrected New York offset

Features (v2.1):
- select and display three timezones, plus local time,
  selectable on-the-fly (no recompile required) from
  a picklist of locations and timezones
- toggle between showing timezone #1 & local time
  in upper time slot
- uses local time to determine offset from UTC on all
  displays
- displays nighttime as white characters on black
  background & daytime as black characters on white
  background
- displays +/- 1 to indicate that a particular timezone
  display is 1-day ahead or 1-day behind UTC
- displays 12/24 hour format time (using current
  Pebble Settings)
- preference settings are maintained in persistent
  storage

Operating instructions:
- to turn backlight on/off (automatically turns off
  after 4-seconds), either press "select" (middle)
  button, or tap/shake
- to toggle between local time and timezone 1 display
  in the upper slot, double-click the "select" button
  (when "set mode" is NOT active)
- to enter "set mode" (local), long-press the "select"
  (middle) button (upper slot displays local time,
  blinks "LOCAL" for 2-seconds, then the currently
  selected local timezone blinks)
- with "set mode" on "local", press the "up" &
  "down" buttons to change the local timezone (note
  that the hour & minute displayed for local time may
  temporarily change for a second, but will then return
  since this time corresponds to either the time
  sync'd from your phone or the time set in the
  Pebble's Settings: Date & Time menu)
- to change "set mode" to "timezone 1", press the
  "select" button (upper slot displays timezone 1
  time, blinks "TIMEZONE 1" for 2-seconds, then
  the currently selected timezone 1 blinks)
- with "set mode" on "timezone 1", press the "up" &
  "down" buttons to change timezone 1 (note that
  the time for timezone 1 will adjust in sync with the
  selected timezone to reflect the offset from UTC,
  taking into consideration the local offset from
  UTC selected earlier)
- to change "set mode" to "timezone 2", press the
  "select" button (the currently selected timezone 2
  blinks in the middle slot)
- with "set mode" on "timezone 2", press the "up" &
  "down" buttons to change timezone 2 (note that
  the time for timezone 2 will adjust in sync with the
  selected timezone to reflect the offset from UTC,
  taking into consideration the local offset from
  UTC selected earlier)
- to change "set mode" to "timezone 3", press the
  "select" button (the currently selected timezone 3
  blinks in the lower slot)
- with "set mode" on "timezone 3", press the "up" &
  "down" buttons to change timezone 3 (note that
  the time for timezone 3 will adjust in sync with the
  selected timezone to reflect the offset from UTC,
  taking into consideration the local offset from
  UTC selected earlier)
- to change "set mode" to "local", press the
  "select" button (upper slot displays local time,
  blinks "LOCAL" for 2-seconds, then the currently
  selected local timezone blinks)
- to exit "set mode" at anytime, long-press the "select"
  button (nothing blinking)


Mark J Culross (KD5RXT)
mjculross@sbcglobal.net

