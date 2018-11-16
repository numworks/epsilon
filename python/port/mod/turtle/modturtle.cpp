extern "C" {
#include "modturtle.h"
}
#include "../../helpers.h"
#include <kandinsky.h>
#include <ion.h>
#include <math.h>
#include "port.h"
#include "turtle_icon.h"

constexpr mp_float_t t_heading_offset = M_PI_2;
constexpr mp_float_t t_heading_scale = M_PI / 180;
constexpr int t_x_offset = Ion::Display::Width / 2;
constexpr int t_y_offset = (Ion::Display::Height-18) / 2;
constexpr int t_size = 9;
constexpr int t_icons = 8;
constexpr KDPoint t_icon_offset(-t_size/2 + 1, -t_size/2 + 1);
constexpr KDSize t_icon_size(t_size, t_size);

static KDColor t_color;
static mp_float_t t_heading;
static mp_float_t t_x, t_y;
static bool t_penup;
static bool t_hidden;
static int t_speed;
static int t_dotsize;

static int t_mileage;
static bool t_drawn;

static KDColor *t_underneath;
static KDColor *t_icon;
static uint8_t *t_dot;

static KDPoint pos_turtle(mp_float_t x, mp_float_t y) {
  return KDPoint(round(x + t_x_offset), round(y + t_y_offset));
}

void draw_turtle() {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();

  int frame = (int)((t_heading / (2*M_PI)) * t_icons + 0.5);
  if (frame < 0) {
    frame = t_icons - ((-frame) % t_icons) - 1;
  }
  else {
    frame = frame % t_icons;
  }

  int offset = frame * (t_size*t_size);

  if (!t_hidden) {
    KDIonContext::sharedContext()->fillRectWithPixels(KDRect(pos_turtle(t_x, t_y).translatedBy(t_icon_offset), t_icon_size), &t_icon[offset], nullptr, t_drawn ? nullptr : t_underneath);
    t_drawn = true;
  }

  if (t_mileage > 1000) {
    if (t_speed > 0) {
      micropython_port_interruptible_msleep(8 * (8 - t_speed));
      t_mileage -= 1000;
    }
    else {
      t_mileage = 0;
    }
  }
}

void erase_turtle() {
  if (t_drawn) {
    KDIonContext::sharedContext()->fillRectWithPixels(KDRect(pos_turtle(t_x, t_y).translatedBy(t_icon_offset), t_icon_size), t_underneath, nullptr);
    t_drawn = false;
  }
}

void dot_turtle(mp_float_t x, mp_float_t y) {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();

  if (!t_penup) {
    KDColor colors[t_dotsize*t_dotsize];
    KDRect rect(pos_turtle(x, y).translatedBy(KDPoint(-t_dotsize/2, -t_dotsize/2)), KDSize(t_dotsize, t_dotsize));
    KDIonContext::sharedContext()->blendRectWithMask(rect, t_color, t_dot, colors);
  }

  if (t_speed > 0) {
    t_mileage += sqrt((x - t_x) * (x - t_x) + (y - t_y) * (y - t_y)) * 1000;
  }

  micropython_port_should_interrupt();

  t_x = x;
  t_y = y;
}

mp_obj_t modturtle_forward(mp_obj_t px) {
  mp_float_t x = t_x + mp_obj_get_float(px)*sin(t_heading);
  mp_float_t y = t_y + mp_obj_get_float(px)*cos(t_heading);
  modturtle_moveto(x, y);
  return mp_const_none;
}

mp_obj_t modturtle_backward(mp_obj_t px) {
  return modturtle_forward(mp_obj_new_float(-mp_obj_get_float(px)));
}

mp_obj_t modturtle_right(mp_obj_t angle) {
  return modturtle_left(mp_obj_new_float(-mp_obj_get_float(angle)));
}

mp_obj_t modturtle_left(mp_obj_t angle) {
  mp_float_t new_angle = ((t_heading - t_heading_offset) + (mp_obj_get_float(angle) * t_heading_scale)) / t_heading_scale;
  return modturtle_setheading(mp_obj_new_float(new_angle));
}

mp_obj_t modturtle_goto(size_t n_args, const mp_obj_t *args) {
  mp_float_t newx, newy;

  if (n_args == 1) {
    mp_obj_t *mp_coords;
    mp_obj_get_array_fixed_n(args[0], 2, &mp_coords);
    newx = mp_obj_get_float(mp_coords[0]);
    newy = mp_obj_get_float(mp_coords[1]);
  }
  else {
    newx = mp_obj_get_float(args[0]);
    newy = mp_obj_get_float(args[1]);
  }

  modturtle_moveto(newx, newy);
  return mp_const_none;
}

mp_obj_t modturtle_setheading(mp_obj_t angle) {
  micropython_port_should_interrupt();

  t_heading = mp_obj_get_float(angle) * t_heading_scale + t_heading_offset;

  Ion::Display::waitForVBlank();
  erase_turtle();
  draw_turtle();

  return mp_const_none;
}

mp_obj_t modturtle_speed(mp_obj_t speed) {
  int new_speed = mp_obj_get_int(speed);

  if (new_speed < 0) {
    new_speed = 0;
  }
  else if (new_speed > 10) {
    new_speed = 10;
  }

  t_speed = new_speed;
  return mp_const_none;
}

mp_obj_t modturtle_position() {
  mp_obj_t mp_pos[2];
  mp_pos[0] = mp_obj_new_float(t_x);
  mp_pos[1] = mp_obj_new_float(t_y);
  return mp_obj_new_tuple(2, mp_pos);
}

mp_obj_t modturtle_heading() {
  return mp_obj_new_float((t_heading - t_heading_offset) / t_heading_scale);
}

mp_obj_t modturtle_pendown() {
  t_penup = false;
  return mp_const_none;
}

mp_obj_t modturtle_penup() {
  t_penup = true;
  return mp_const_none;
}

mp_obj_t modturtle_pensize(size_t n_args, const mp_obj_t *args) {
  if (n_args == 0) {
    return MP_OBJ_NEW_SMALL_INT(t_dotsize);
  }

  int size = mp_obj_get_int(args[0]);
  if (size < 1) {
    size = 1;
  }
  else if (size > 10) {
    size = 10;
  }

  modturtle_initpen(size);

  return mp_const_none;
}

mp_obj_t modturtle_isdown() {
  return t_penup ? mp_const_false : mp_const_true;
}

mp_obj_t modturtle_color(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
  t_color = KDColor::RGB888(mp_obj_get_int(r), mp_obj_get_int(g), mp_obj_get_int(b));
  return mp_const_none;
}

mp_obj_t modturtle_showturtle() {
  t_hidden = false;
  draw_turtle();
  return mp_const_none;
}

mp_obj_t modturtle_hideturtle() {
  t_hidden = true;
  erase_turtle();
  return mp_const_none;
}

mp_obj_t modturtle_isvisible() {
  return t_hidden ? mp_const_false : mp_const_true;
}

mp_obj_t modturtle___init__() {
  if (!t_underneath) {
    t_underneath = new KDColor[t_size * t_size];
  }
  if (!t_icon) {
    t_icon = new KDColor[t_size * t_size];

    Ion::decompress(
      ImageStore::TurtleIcon->compressedPixelData(),
      reinterpret_cast<uint8_t *>(t_icon),
      ImageStore::TurtleIcon->compressedPixelDataSize(),
      sizeof(KDColor) * t_size * t_size * t_icons
    );
  }

  t_color = KDColorBlack;
  t_heading = t_heading_offset;
  t_x = t_y = 0;
  t_penup = false;
  t_speed = 6;
  t_mileage = 0;
  t_hidden = false;

  modturtle_initpen(5);

  return mp_const_none;
}

void modturtle_initpen(int size) {
  if (t_dot) {
    delete[] t_dot;
  }
  t_dot = new uint8_t[size*size];
  t_dotsize = size;

  mp_float_t middle = size / 2;
  for (int j = 0; j < size; j++) {
    for (int i = 0; i < size; i++) {
      mp_float_t distance = sqrt((j - middle)*(j - middle) + (i - middle)*(i - middle)) / (middle+1);
      int value = distance * distance * 255;
      if (value < 0) {
        value = 0;
      }
      else if (value > 255) {
        value = 255;
      }
      t_dot[j*size + i] = value;
    }
  }
}

void modturtle_moveto(mp_float_t x, mp_float_t y) {
  mp_float_t oldx = t_x, oldy = t_y;
  mp_float_t length = sqrt((x - oldx) * (x - oldx) + (y - oldy) * (y - oldy));

  if (length > 1) {
    for (int i = 0; i < length; i++) {
      mp_float_t progress = i / length;

      if (t_speed > 0) {
        Ion::Display::waitForVBlank();
      }
      erase_turtle();
      dot_turtle(x * progress + oldx * (1 - progress), y * progress + oldy * (1 - progress));
      draw_turtle();
    }
  }

  Ion::Display::waitForVBlank();
  erase_turtle();
  dot_turtle(x, y);
  draw_turtle();
}

void modturtle_deinit() {
  delete[] t_underneath;
  t_underneath = nullptr;
  delete[] t_icon;
  t_icon = nullptr;
  delete[] t_dot;
  t_dot = nullptr;
}
