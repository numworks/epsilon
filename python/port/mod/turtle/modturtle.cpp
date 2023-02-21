extern "C" {
#include "modturtle.h"

#include <py/gc.h>
#include <py/objtuple.h>
#include <py/runtime.h>
}
#include <kandinsky/ion_context.h>

#include "../../port.h"
#include "turtle.h"

static Turtle sTurtle;

void modturtle_gc_collect() {
  // Mark the shared sTurtle object as a GC root
  MicroPython::collectRootsAtAddress((char *)&sTurtle, sizeof(Turtle));
}

void modturtle_view_did_disappear() { sTurtle.viewDidDisappear(); }

mp_obj_t modturtle___init__() {
  sTurtle = Turtle();
  /* Note: we don't even bother writing a destructor for Turtle because this
   * init function is called once, and only once, per MicroPython init cycle.
   * When the previous Turtle object is destroyed, its VM is long gone. */
  return mp_const_none;
}

mp_obj_t modturtle_reset() {
  sTurtle.reset();
  return mp_const_none;
}

mp_obj_t modturtle_forward(mp_obj_t px) {
  sTurtle.forward(mp_obj_get_float(px));
  return mp_const_none;
}

mp_obj_t modturtle_backward(mp_obj_t px) {
  sTurtle.backward(mp_obj_get_float(px));
  return mp_const_none;
}

mp_obj_t modturtle_right(mp_obj_t angle) {
  sTurtle.right(mp_obj_get_float(angle));
  return mp_const_none;
}

mp_obj_t modturtle_left(mp_obj_t angle) {
  sTurtle.left(mp_obj_get_float(angle));
  return mp_const_none;
}

mp_obj_t modturtle_circle(size_t n_args, const mp_obj_t *args) {
  mp_int_t radius = mp_obj_get_int(args[0]);

  if (n_args == 1) {
    sTurtle.circle(radius);
  } else {
    mp_float_t angle = mp_obj_get_float(args[1]);
    sTurtle.circle(radius, angle);
  }
  return mp_const_none;
}

mp_obj_t modturtle_goto(size_t n_args, const mp_obj_t *args) {
  mp_float_t x = 0;
  mp_float_t y = 0;

  if (n_args == 1) {
    mp_obj_t *mp_coords;
    mp_obj_get_array_fixed_n(args[0], 2, &mp_coords);
    x = mp_obj_get_float(mp_coords[0]);
    y = mp_obj_get_float(mp_coords[1]);
  } else {
    x = mp_obj_get_float(args[0]);
    y = mp_obj_get_float(args[1]);
  }

  sTurtle.goTo(x, y);

  return mp_const_none;
}

mp_obj_t modturtle_setheading(mp_obj_t angle) {
  sTurtle.setHeading(mp_obj_get_float(angle));
  return mp_const_none;
}

mp_obj_t modturtle_speed(size_t n_args, const mp_obj_t *args) {
  if (n_args == 0) {
    return MP_OBJ_NEW_SMALL_INT(sTurtle.speed());
  }
  sTurtle.setSpeed(mp_obj_get_int(args[0]));
  return mp_const_none;
}

mp_obj_t modturtle_position() {
  mp_obj_t mp_pos[2];
  mp_pos[0] = mp_obj_new_float(sTurtle.x());
  mp_pos[1] = mp_obj_new_float(sTurtle.y());
  return mp_obj_new_tuple(2, mp_pos);
}

mp_obj_t modturtle_heading() { return mp_obj_new_float(sTurtle.heading()); }

mp_obj_t modturtle_pendown() {
  sTurtle.setPenDown(true);
  return mp_const_none;
}

mp_obj_t modturtle_penup() {
  sTurtle.setPenDown(false);
  return mp_const_none;
}

mp_obj_t modturtle_pensize(size_t n_args, const mp_obj_t *args) {
  if (n_args == 0) {
    return MP_OBJ_NEW_SMALL_INT(sTurtle.penSize());
  }
  sTurtle.setPenSize(mp_obj_get_int(args[0]));
  return mp_const_none;
}

mp_obj_t modturtle_isdown() {
  return sTurtle.isPenDown() ? mp_const_true : mp_const_false;
}

mp_float_t uint8tColorToDouble(uint8_t c) {
  return static_cast<double>(c) / 255.0;
}

mp_obj_t modturtle_pencolor(size_t n_args, const mp_obj_t *args) {
  if (n_args == 0) {
    // pencolor()
    KDColor c = sTurtle.color();
    mp_obj_t mp_col[3];
    if (sTurtle.colorMode() == MicroPython::Color::Mode::MaxIntensity255) {
      mp_col[0] = mp_obj_new_int_from_uint(c.red());
      mp_col[1] = mp_obj_new_int_from_uint(c.green());
      mp_col[2] = mp_obj_new_int_from_uint(c.blue());
    } else {
      mp_col[0] = mp_obj_new_float(uint8tColorToDouble(c.red()));
      mp_col[1] = mp_obj_new_float(uint8tColorToDouble(c.green()));
      mp_col[2] = mp_obj_new_float(uint8tColorToDouble(c.blue()));
    }
    return mp_obj_new_tuple(3, mp_col);
  }
  if (n_args == 2) {
    mp_raise_TypeError("pencolor() takes 0, 1 or 3 arguments");
    return mp_const_none;
  }
  mp_obj_t color;
  if (n_args == 1) {
    color = args[0];
  } else {
    assert(n_args == 3);
    color = mp_obj_new_tuple(n_args, args);
  }
  sTurtle.setColor(MicroPython::Color::Parse(color, sTurtle.colorMode()));
  return mp_const_none;
}

mp_obj_t modturtle_colormode(size_t n_args, const mp_obj_t *args) {
  if (n_args == 0) {
    return mp_obj_new_int_from_uint(static_cast<int>(sTurtle.colorMode()));
  } else {
    // To accept both colormode(1) and colormode(1.0) we try to get args[0] as
    // both int and float
    mp_float_t decimalOne = mp_obj_get_float(args[0]);
    int colorMode;
    // But only 1 is accepted as float, 255 must be int
    if (decimalOne == 1.0) {
      colorMode = static_cast<int>(MicroPython::Color::Mode::MaxIntensity1);
    } else {
      colorMode = mp_obj_get_int(args[0]);
    }
    if (colorMode !=
            static_cast<int>(MicroPython::Color::Mode::MaxIntensity1) &&
        colorMode !=
            static_cast<int>(MicroPython::Color::Mode::MaxIntensity255)) {
      mp_raise_ValueError("Colormode can be 1 or 255");
      return mp_const_none;
    }
    sTurtle.setColorMode(static_cast<MicroPython::Color::Mode>(colorMode));
    return mp_const_none;
  }
}

mp_obj_t modturtle_showturtle() {
  sTurtle.setVisible(true);
  return mp_const_none;
}

mp_obj_t modturtle_hideturtle() {
  sTurtle.setVisible(false);
  return mp_const_none;
}

mp_obj_t modturtle_isvisible() {
  return sTurtle.isVisible() ? mp_const_true : mp_const_false;
}

mp_obj_t modturtle_write(mp_obj_t s) {
  const char *string = mp_obj_str_get_str(s);
  sTurtle.write(string);
  return mp_const_none;
}