#ifndef EADK_H
#define EADK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Types and constants

typedef uint16_t eadk_color_t;

static const eadk_color_t eadk_color_black = 0x0;
static const eadk_color_t eadk_color_white = 0xFFFF;
static const eadk_color_t eadk_color_red = 0xF800;
static const eadk_color_t eadk_color_green = 0x07E0;
static const eadk_color_t eadk_color_blue = 0x001F;

typedef struct {
  uint16_t x;
  uint16_t y;
} eadk_point_t;

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} eadk_rect_t;

// Keyboard and Events

#define EADK_SCREEN_WIDTH 320
#define EADK_SCREEN_HEIGHT 240
static const eadk_rect_t eadk_screen_rect = {0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT};

typedef uint64_t eadk_keyboard_state_t;
typedef enum {
  eadk_key_left = 0,
  eadk_key_up = 1,
  eadk_key_down = 2,
  eadk_key_right = 3,
  eadk_key_ok = 4,
  eadk_key_back = 5,
  eadk_key_home = 6,
  eadk_key_on_off = 8,
  eadk_key_shift = 12,
  eadk_key_alpha = 13,
  eadk_key_xnt = 14,
  eadk_key_var = 15,
  eadk_key_toolbox = 16,
  eadk_key_backspace = 17,
  eadk_key_exp = 18,
  eadk_key_ln = 19,
  eadk_key_log = 20,
  eadk_key_imaginary = 21,
  eadk_key_comma = 22,
  eadk_key_power = 23,
  eadk_key_sine = 24,
  eadk_key_cosine = 25,
  eadk_key_tangent = 26,
  eadk_key_pi = 27,
  eadk_key_sqrt = 28,
  eadk_key_square = 29,
  eadk_key_seven = 30,
  eadk_key_eight = 31,
  eadk_key_nine = 32,
  eadk_key_left_parenthesis = 33,
  eadk_key_right_parenthesis = 34,
  eadk_key_four = 36,
  eadk_key_five = 37,
  eadk_key_six = 38,
  eadk_key_multiplication = 39,
  eadk_key_division = 40,
  eadk_key_one = 42,
  eadk_key_two = 43,
  eadk_key_three = 44,
  eadk_key_plus = 45,
  eadk_key_minus = 46,
  eadk_key_zero = 48,
  eadk_key_dot = 49,
  eadk_key_ee = 50,
  eadk_key_ans = 51,
  eadk_key_exe = 52
} eadk_key_t;

eadk_keyboard_state_t eadk_keyboard_scan();
static inline bool eadk_keyboard_key_down(eadk_keyboard_state_t state, eadk_key_t key) {
  return (state>>(uint8_t)key) & 1;
}

typedef uint16_t eadk_event_t;
enum {
  eadk_event_left = 0,
  eadk_event_up = 1,
  eadk_event_down  = 2,
  eadk_event_right = 3,
  eadk_event_ok    = 4,
  eadk_event_back  = 5,
  eadk_event_shift = 12,
  eadk_event_alpha = 13,
  eadk_event_xnt   = 14,
  eadk_event_var   = 15,
  eadk_event_toolbox = 16,
  eadk_event_backspace = 17,
  eadk_event_exp = 18,
  eadk_event_ln = 19,
  eadk_event_log = 20,
  eadk_event_imaginary = 21,
  eadk_event_comma = 22,
  eadk_event_power = 23,
  eadk_event_sine = 24,
  eadk_event_cosine = 25,
  eadk_event_tangent = 26,
  eadk_event_pi = 27,
  eadk_event_sqrt = 28,
  eadk_event_square = 29,
  eadk_event_seven = 30,
  eadk_event_eight = 31,
  eadk_event_nine = 32,
  eadk_event_left_parenthesis = 33,
  eadk_event_right_parenthesis = 34,
  eadk_event_four = 36,
  eadk_event_five = 37,
  eadk_event_six = 38,
  eadk_event_multiplication = 39,
  eadk_event_division = 40,
  eadk_event_one  = 42,
  eadk_event_two = 43,
  eadk_event_three = 44,
  eadk_event_plus = 45,
  eadk_event_minus = 46,
  eadk_event_zero = 48,
  eadk_event_dot = 49,
  eadk_event_ee = 50,
  eadk_event_ans = 51,
  eadk_event_exe = 52,
  eadk_event_shift_left  = 54,
  eadk_event_shift_up    = 55,
  eadk_event_shift_down  = 56,
  eadk_event_shift_right = 57,
  eadk_event_alpha_lock = 67,
  eadk_event_cut = 68,
  eadk_event_copy = 69,
  eadk_event_paste = 70,
  eadk_event_clear = 71,
  eadk_event_left_bracket = 72,
  eadk_event_right_bracket = 73,
  eadk_event_left_brace = 74,
  eadk_event_right_brace = 75,
  eadk_event_underscore = 76,
  eadk_event_sto = 77,
  eadk_event_arcsine = 78,
  eadk_event_arccosine = 79,
  eadk_event_arctangent = 80,
  eadk_event_equal = 81,
  eadk_event_lower = 82,
  eadk_event_greater = 83,
  eadk_event_colon = 122,
  eadk_event_semicolon = 123,
  eadk_event_double_quotes = 124,
  eadk_event_percent = 125,
  eadk_event_lower_a = 126,
  eadk_event_lower_b = 127,
  eadk_event_lower_c = 128,
  eadk_event_lower_d = 129,
  eadk_event_lower_e = 130,
  eadk_event_lower_f = 131,
  eadk_event_lower_g = 132,
  eadk_event_lower_h = 133,
  eadk_event_lower_i = 134,
  eadk_event_lower_j = 135,
  eadk_event_lower_k = 136,
  eadk_event_lower_l = 137,
  eadk_event_lower_m = 138,
  eadk_event_lower_n = 139,
  eadk_event_lower_o = 140,
  eadk_event_lower_p = 141,
  eadk_event_lower_q = 142,
  eadk_event_lower_r = 144,
  eadk_event_lower_s = 145,
  eadk_event_lower_t = 146,
  eadk_event_lower_u = 147,
  eadk_event_lower_v = 148,
  eadk_event_lower_w = 150,
  eadk_event_lower_x = 151,
  eadk_event_lower_y = 152,
  eadk_event_lower_z = 153,
  eadk_event_space = 154,
  eadk_event_question = 156,
  eadk_event_exclamation = 157,
  eadk_event_upper_a = 180,
  eadk_event_upper_b = 181,
  eadk_event_upper_c = 182,
  eadk_event_upper_d = 183,
  eadk_event_upper_e = 184,
  eadk_event_upper_f = 185,
  eadk_event_upper_g = 186,
  eadk_event_upper_h = 187,
  eadk_event_upper_i = 188,
  eadk_event_upper_j = 189,
  eadk_event_upper_k = 190,
  eadk_event_upper_l = 191,
  eadk_event_upper_m = 192,
  eadk_event_upper_n = 193,
  eadk_event_upper_o = 194,
  eadk_event_upper_p = 195,
  eadk_event_upper_q = 196,
  eadk_event_upper_r = 198,
  eadk_event_upper_s = 199,
  eadk_event_upper_t = 200,
  eadk_event_upper_u = 201,
  eadk_event_upper_v = 202,
  eadk_event_upper_w = 204,
  eadk_event_upper_x = 205,
  eadk_event_upper_y = 206,
  eadk_event_upper_z = 207,
};

eadk_event_t eadk_event_get(int32_t * timeout);

// Backlight

void eadk_backlight_set_brightness(uint8_t brightness);
uint8_t eadk_backlight_brightness();

// Battery

bool eadk_battery_is_charging();
uint8_t eadk_battery_level();
float eadk_battery_voltage();

// Display

void eadk_display_push_rect(eadk_rect_t rect, const eadk_color_t * pixels);
void eadk_display_push_rect_uniform(eadk_rect_t rect, eadk_color_t color);
void eadk_display_pull_rect(eadk_rect_t rect, eadk_color_t * pixels);
bool eadk_display_wait_for_vblank();
void eadk_display_draw_string(const char * text, eadk_point_t point, bool large_font, eadk_color_t text_color, eadk_color_t background_color);

// Timing

void eadk_timing_usleep(uint32_t us);
void eadk_timing_msleep(uint32_t ms);
uint64_t eadk_timing_millis();

// External data

extern const char * eadk_external_data;
extern size_t eadk_external_data_size;

// Misc

bool eadk_usb_is_plugged();
uint32_t eadk_random();

#endif
