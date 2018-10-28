extern "C" {
#include "modturtle.h"
}
#include <kandinsky.h>
#include <ion.h>
#include <math.h>
#include "port.h"
#include "turtle_icon.h"

constexpr float t_heading_offset = M_PI_2;
constexpr float t_heading_scale = M_PI / 180;
constexpr int t_x_offset = Ion::Display::Width / 2;
constexpr int t_y_offset = (Ion::Display::Height-18) / 2;
constexpr int t_size = 9;
constexpr int t_icons = 8;
constexpr KDPoint t_icon_offset(-t_size/2 + 1, -t_size/2 + 1);
constexpr KDSize t_icon_size(t_size, t_size);

static KDColor t_color;
static float t_heading;
static float t_x, t_y;
static bool t_penup;
static bool t_hidden;
static int t_speed;
static int t_dotsize;

static int t_mileage;
static bool t_drawn;

static KDColor *t_underneath;
static KDColor *t_icon;
static uint8_t *t_dot;

static KDPoint pos_turtle(float x, float y) {
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

    if (t_mileage > 1000) {
      if (t_speed > 0) {
        Ion::msleep(8 * (8 - t_speed));
      }
      t_mileage -= 1000;
    }

    t_drawn = true;
  }
}

void erase_turtle() {
  if (t_drawn) {
    KDIonContext::sharedContext()->fillRectWithPixels(KDRect(pos_turtle(t_x, t_y).translatedBy(t_icon_offset), t_icon_size), t_underneath, nullptr);
    t_drawn = false;
  }
}

void dot_turtle(float x, float y) {
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

mp_obj_t turtle_forward(mp_obj_t px) {
  float x = t_x + mp_obj_get_float(px)*sin(t_heading);
  float y = t_y + mp_obj_get_float(px)*cos(t_heading);
  return turtle_goto(mp_obj_new_float(x), mp_obj_new_float(y));
}

mp_obj_t turtle_backward(mp_obj_t px) {
  return turtle_forward(mp_obj_new_float(-mp_obj_get_float(px)));
}

mp_obj_t turtle_right(mp_obj_t angle) {
  return turtle_left(mp_obj_new_float(-mp_obj_get_float(angle)));
}

mp_obj_t turtle_left(mp_obj_t angle) {
  float new_angle = ((t_heading - t_heading_offset) + (mp_obj_get_float(angle) * t_heading_scale)) / t_heading_scale;
  return turtle_setheading(mp_obj_new_float(new_angle));
}

mp_obj_t turtle_goto(mp_obj_t x, mp_obj_t y) {
  float newx = mp_obj_get_float(x), newy = mp_obj_get_float(y);
  float oldx = t_x, oldy = t_y;
  float length = sqrt((newx - oldx) * (newx - oldx) + (newy - oldy) * (newy - oldy));

  if (length > 1) {
    for (int i = 0; i < length; i++) {
      float progress = i / length;

      if (t_speed > 0) {
        Ion::Display::waitForVBlank();
      }
      erase_turtle();
      dot_turtle(newx * progress + oldx * (1 - progress), newy * progress + oldy * (1 - progress));
      draw_turtle();
    }
  }

  Ion::Display::waitForVBlank();
  erase_turtle();
  dot_turtle(newx, newy);
  draw_turtle();

  return mp_const_none;
}

mp_obj_t turtle_setheading(mp_obj_t angle) {
  micropython_port_should_interrupt();

  float new_angle = mp_obj_get_float(angle) * t_heading_scale + t_heading_offset;

  //if (t_speed == 0) {
    t_heading = new_angle;

    if (t_speed > 0) {
      Ion::Display::waitForVBlank();
    }
    erase_turtle();
    draw_turtle();

    return mp_const_none;/*
  }

  micropython_port_should_interrupt();

  return mp_const_none;*/
}

mp_obj_t turtle_speed(mp_obj_t speed) {
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

mp_obj_t turtle_pendown() {
  t_penup = false;
  return mp_const_none;
}

mp_obj_t turtle_penup() {
  t_penup = true;
  return mp_const_none;
}

mp_obj_t turtle_pensize(mp_obj_t size) {
  int s = mp_obj_get_int(size);
  if (s < 1) {
    s = 1;
  }
  else if (s > 10) {
    s = 10;
  }

  if (t_dot) {
    delete[] t_dot;
  }
  t_dot = new uint8_t[s*s];
  t_dotsize = s;

  float middle = s / 2;
  for (int j = 0; j < s; j++) {
    for (int i = 0; i < s; i++) {
      float distance = sqrt((j - middle)*(j - middle) + (i - middle)*(i - middle)) / middle;
      int value = distance * distance * 255;
      if (value < 0) {
        value = 0;
      }
      else if (value > 255) {
        value = 255;
      }
      t_dot[j*s + i] = value;
    }
  }

  return mp_const_none;
}

mp_obj_t turtle_color(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
  t_color = KDColor::RGB888(mp_obj_get_int(r), mp_obj_get_int(g), mp_obj_get_int(b));
  return mp_const_none;
}

mp_obj_t turtle_showturtle() {
  t_hidden = false;
  draw_turtle();
  return mp_const_none;
}

mp_obj_t turtle_hideturtle() {
  t_hidden = true;
  erase_turtle();
  return mp_const_none;
}

mp_obj_t turtle___init__() {
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

  turtle_pensize(MP_OBJ_NEW_SMALL_INT(5));

  return mp_const_none;
}

void turtle_deinit() {
  delete[] t_underneath;
  t_underneath = nullptr;
  delete[] t_icon;
  t_icon = nullptr;
  delete[] t_dot;
  t_dot = nullptr;
}
