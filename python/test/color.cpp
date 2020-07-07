#include <quiz.h>
#include "execution_environment.h"
#include "../port/mod/kandinsky/modkandinsky.h"

extern mp_obj_t TupleForKDColor(KDColor c);

QUIZ_CASE(python_color) {
  // RGB string tests
  const char * rgb0 = "#ffffff"; // White
  const char * rgb1 = "#ffff00"; // Yellow
  assert_color_from_str(rgb0, KDColorWhite);
  assert_color_from_str(rgb1, KDColorYellow);

  // Grey level string tests
  const char * greyLevel0 = "0"; // Black
  const char * greyLevel1 = "1"; // White
  assert_color_from_str(greyLevel0, KDColorBlack);
  assert_color_from_str(greyLevel1, KDColorWhite);

  // Color name string tests
  const char * color0 = "r"; // Red
  const char * color1 = "blue"; // Blue
  assert_color_from_str(color0, KDColorRed);
  assert_color_from_str(color1, KDColorBlue);

  // Tuple float colors
  mp_obj_t floatTuple0 = floatTuple(mp_obj_new_float(KDColorBlack.red()),
                                    mp_obj_new_float(KDColorBlack.green()),
                                    mp_obj_new_float(KDColorBlack.blue())); // Black
  assert_color_from_mp_obj_t(floatTuple0, KDColorBlack);
  mp_obj_t floatTuple1 = floatTuple(mp_obj_new_float(1.0),
                                    mp_obj_new_float(1.0),
                                    mp_obj_new_float(1.0)); // White
  assert_color_from_mp_obj_t(floatTuple0, KDColorBlack);
  assert_color_from_mp_obj_t(floatTuple1, KDColorWhite, MicroPython::ColorParser::ColorMode::MaxIntensity1);
}

