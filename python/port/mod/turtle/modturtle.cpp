extern "C" {
#include "modturtle.h"
}
#include "turtle.h"

static Turtle sTurtle;

mp_obj_t modturtle___init__() {
  sTurtle = Turtle();
  /* Note: we don't even bother writing a destructor for Turtle because this
   * init function is called once, and only once, per MicroPython init cycle.
   * When the previous Turtle object is destroyed, its VM is long gone. */
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

mp_obj_t modturtle_goto(size_t n_args, const mp_obj_t *args) {
  mp_float_t x = 0;
  mp_float_t y = 0;

  if (n_args == 1) {
    mp_obj_t * mp_coords;
    mp_obj_get_array_fixed_n(args[0], 2, &mp_coords);
    x = mp_obj_get_float(mp_coords[0]);
    y = mp_obj_get_float(mp_coords[1]);
  }
  else {
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

mp_obj_t modturtle_speed(mp_obj_t speed) {
  sTurtle.setSpeed(mp_obj_get_int(speed));
  return mp_const_none;
}

mp_obj_t modturtle_position() {
  mp_obj_t mp_pos[2];
  mp_pos[0] = mp_obj_new_float(sTurtle.x());
  mp_pos[1] = mp_obj_new_float(sTurtle.y());
  return mp_obj_new_tuple(2, mp_pos);
}

mp_obj_t modturtle_heading() {
  return mp_obj_new_float(sTurtle.heading());
}

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

mp_obj_t modturtle_color(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
  sTurtle.setColor(KDColor::RGB888(mp_obj_get_int(r), mp_obj_get_int(g), mp_obj_get_int(b)));
  return mp_const_none;
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
