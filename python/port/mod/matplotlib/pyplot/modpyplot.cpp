extern "C" {
#include "modpyplot.h"
}
#include <assert.h>
#include <escher/palette.h>
#include "port.h"
#include "plot_controller.h"

Matplotlib::PlotStore * sPlotStore = nullptr;
Matplotlib::PlotController * sPlotController = nullptr;
static int paletteIndex = 0;

// Private helper

// Method to populate items with a scalar or an array argument

static size_t extractArgument(mp_obj_t arg, mp_obj_t ** items) {
  size_t itemLength;
  if (mp_obj_is_type(arg, &mp_type_tuple) || mp_obj_is_type(arg, &mp_type_list)) {
    mp_obj_get_array(arg, &itemLength, items);
  } else {
    itemLength = 1;
    *items = m_new(mp_obj_t, 1);
    (*items)[0] = arg;
  }
  return itemLength;
}

// Extract two scalar or array arguments and check for their strickly equal dimension

static size_t extractArgumentsAndCheckEqualSize(mp_obj_t x, mp_obj_t y, mp_obj_t ** xItems, mp_obj_t ** yItems) {
  size_t xLength = extractArgument(x, xItems);
  size_t yLength = extractArgument(y, yItems);
  if (xLength != yLength) {
    mp_raise_ValueError("x and y must be the same size");
  }
  return xLength;
}

/* Extract one scalar or array arguments and check that it is either:
 * - of size 1
 * - of the required non null size
*/

static size_t extractArgumentAndValidateSize(mp_obj_t arg, size_t requiredlength, mp_obj_t ** items) {
  size_t itemLength = extractArgument(arg, items);
  if (itemLength == 0 || !(itemLength == 1 || requiredlength == 1 || itemLength == requiredlength)) {
    mp_raise_ValueError("shape mismatch");
  }
  return itemLength;
}

// Get color from keyword arguments if possible

bool colorFromKeywordArgument(mp_map_elem_t * elemColor, KDColor * color) {
  if (elemColor != nullptr) {
    *color = MicroPython::Color::Parse(elemColor->value);
    return true;
  } else {
    *color = Escher::Palette::nextDataColor(&paletteIndex);
    return false;
  }
}

// Internal functions

mp_obj_t modpyplot___init__() {
  static Matplotlib::PlotStore plotStore;
  static Matplotlib::PlotController plotController(&plotStore);
  sPlotStore = &plotStore;
  sPlotController = &plotController;
  sPlotStore->flush();
  paletteIndex = 0;
  return mp_const_none;
}

void modpyplot_gc_collect() {
  if (sPlotStore == nullptr) {
    return;
  }
  MicroPython::collectRootsAtAddress(
    reinterpret_cast<char *>(sPlotStore),
    sizeof(Matplotlib::PlotStore)
  );
}

void modpyplot_flush_used_heap() {
  if (sPlotStore) {
    // Clean the store object
    sPlotStore->flush();
  }
}

/* arrow(x,y,dx,dy, KW : head_width, color)
 * x, y, dx, dy scalars
 * */

mp_obj_t modpyplot_arrow(size_t n_args, const mp_obj_t *args, mp_map_t* kw_args) {
  assert(sPlotStore != nullptr);
  sPlotStore->setShow(true);

  if (n_args > 4) {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,"arrow() takes 4 positional arguments but %d were given",n_args));
  }

  mp_map_elem_t * elem;
  // Setting arrow width
  elem = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_head_width), MP_MAP_LOOKUP);
  /* Default head_width is 0.0f because we want a default width in pixel
   * coordinates which is handled by CurveView::drawArrow. */
  mp_obj_t arrowWidth = (elem == nullptr) ? mp_obj_new_float(0.0f) : elem->value;

  // Setting arrow color
  KDColor color;
  // color keyword
  elem = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_color), MP_MAP_LOOKUP);
  colorFromKeywordArgument(elem, &color);

  // Adding the object to the plot
  assert(n_args >= 4);
  sPlotStore->addSegment(args[0], args[1], mp_obj_new_float(mp_obj_get_float(args[0]) + mp_obj_get_float(args[2])), mp_obj_new_float(mp_obj_get_float(args[1]) + mp_obj_get_float(args[3])), color, arrowWidth);
  return mp_const_none;
}

/* axis(arg)
 *  - arg = "on", "off", "auto"
 *  - arg = True, False
 *  - arg = [xmin, xmax, ymin, ymax], (xmin, xmax, ymin, ymax)
 * Returns : (xmin, xmax, ymin, ymax) : float */

mp_obj_t modpyplot_axis(size_t n_args, const mp_obj_t *args) {
  assert(sPlotStore != nullptr);
  sPlotStore->setShow(true);
  if (n_args == 1) {
    mp_obj_t arg = args[0];
    if (mp_obj_is_str(arg)) {
      if (mp_obj_str_equal(arg, mp_obj_new_str("on", 2))) {
        sPlotStore->setAxesRequested(true);
      } else if (mp_obj_str_equal(arg, mp_obj_new_str("off", 3))) {
        sPlotStore->setAxesRequested(false);
      } else if (mp_obj_str_equal(arg, mp_obj_new_str("auto", 4))) {
        sPlotStore->setAxesRequested(true);
        sPlotStore->setAxesAuto(true);
      } else {
        mp_raise_ValueError("Unrecognized string given to axis; try 'on', 'off' or 'auto'");
      }
    } else if (mp_obj_is_bool(arg)) {
      sPlotStore->setAxesRequested(mp_obj_is_true(arg));
    } else if (mp_obj_is_type(arg, &mp_type_tuple) || mp_obj_is_type(arg, &mp_type_list)) {
      mp_obj_t * items;
      mp_obj_get_array_fixed_n(arg, 4, &items);
      sPlotStore->setXMin(mp_obj_get_float(items[0]));
      sPlotStore->setXMax(mp_obj_get_float(items[1]));
      sPlotStore->setYMin(mp_obj_get_float(items[2]));
      sPlotStore->setYMax(mp_obj_get_float(items[3]));
      sPlotStore->setAxesAuto(false);
    } else {
      mp_raise_TypeError("the first argument to axis() must be an iterable of the form [xmin, xmax, ymin, ymax]");
    }
  }

  // Build the return value
  mp_obj_t coords[4];
  coords[0] = mp_obj_new_float(sPlotStore->xMin());
  coords[1] = mp_obj_new_float(sPlotStore->xMax());
  coords[2] = mp_obj_new_float(sPlotStore->yMin());
  coords[3] = mp_obj_new_float(sPlotStore->yMax());
  return mp_obj_new_tuple(4, coords);
}

/* bar(x, height, width, bottom, KW :color)
 * 'x', 'height', 'width' and 'bottom' can either be a scalar or an array/tuple of
 * scalar.
 * 'width' default value is 0.8
 * 'bottom' default value is None
 * */

mp_obj_t modpyplot_bar(size_t n_args, const mp_obj_t *args, mp_map_t* kw_args) {
  assert(sPlotStore != nullptr);
  if (n_args > 4) {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,"bar() takes from 2 to 4 positional arguments but %d were given",n_args));
  }
  sPlotStore->setShow(true);
  mp_obj_t * xItems;
  mp_obj_t * hItems;
  mp_obj_t * wItems;
  mp_obj_t * bItems;

  assert(n_args >= 2);

  // x arg
  size_t xLength = extractArgument(args[0], &xItems);
  if (xLength == 0) {
    return mp_const_none;
  }

  // height arg
  size_t hLength = extractArgumentAndValidateSize(args[1], xLength, &hItems);

  // width arg
  size_t wLength = 1;
  if (n_args >= 3) {
    wLength = extractArgumentAndValidateSize(args[2], xLength, &wItems);
  } else {
    wItems = m_new(mp_obj_t, 1);
    wItems[0] = mp_obj_new_float(0.8f);
  }

  // bottom arg
  size_t bLength = 1;
  if (n_args >= 4) {
    bLength = extractArgumentAndValidateSize(args[3], xLength, &bItems);
  } else {
    bItems = m_new(mp_obj_t, 1);
    bItems[0] = mp_obj_new_float(0.0f);
  }
  assert(hLength > 0 && wLength > 0 && bLength > 0);

  // Setting bar color
  // color keyword
  KDColor color;
  // color keyword
  mp_map_elem_t * elem = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_color), MP_MAP_LOOKUP);
  colorFromKeywordArgument(elem, &color);

  for (size_t i=0; i<xLength; i++) {
    mp_obj_t iH = hItems[hLength > 1 ? i : 0];
    mp_obj_t iW = wItems[wLength > 1 ? i : 0];
    mp_obj_t iB = bItems[bLength > 1 ? i : 0];
    mp_obj_t iX = xItems[i];

    float iWf = mp_obj_get_float(iW);
    float iXf = mp_obj_get_float(iX);
    mp_obj_t rectLeft = mp_obj_new_float(iXf - iWf/2.0f);
    mp_obj_t rectRight = mp_obj_new_float(iXf + iWf/2.0f);
    mp_obj_t rectBottom = iB;
    mp_obj_t rectTop = mp_obj_new_float(mp_obj_get_float(iH) + mp_obj_get_float(iB));
    if (mp_obj_get_float(iH) < 0.0) {
      mp_obj_t temp = rectTop;
      rectTop = rectBottom;
      rectBottom = temp;
    }
    sPlotStore->addRect(rectLeft, rectRight, rectTop, rectBottom, color);
  }
  return mp_const_none;
}

mp_obj_t modpyplot_grid(size_t n_args, const mp_obj_t *args) {
  assert(sPlotStore != nullptr);
  sPlotStore->setShow(true);
  if (n_args == 0) {
    // Toggle the grid visibility
    sPlotStore->setGridRequested(!sPlotStore->gridRequested());
  } else {
    assert(n_args >= 1);
    sPlotStore->setGridRequested(mp_obj_is_true(args[0]));
  }
  return mp_const_none;
}

/* hist(x, bins KW : color)
 * 'x' array
 * 'bins': (default value 10)
 *    - int (number of bins)
 *    - sequence of bins
 * */

mp_obj_t modpyplot_hist(size_t n_args, const mp_obj_t *args, mp_map_t* kw_args ) {
  if (n_args > 2) {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,"hist() takes from 1 to 2 positional arguments but %d were given",n_args));
  }
  assert(sPlotStore != nullptr);
  sPlotStore->setShow(true);
  // Sort data to easily get the minimal and maximal value and count bin sizes
  mp_obj_t * xItems;
  assert(n_args >= 1);
  size_t xLength = extractArgument(args[0], &xItems);
  if (xLength == 0) {
    return mp_const_none;
  }
  mp_obj_t xList = mp_obj_new_list(xLength, xItems);
  mp_obj_list_sort(1, &xList, (mp_map_t*)&mp_const_empty_map);
  mp_obj_list_get(xList, &xLength, &xItems);
  assert(xLength > 0);
  mp_float_t min = mp_obj_get_float(xItems[0]);
  mp_float_t max = mp_obj_get_float(xItems[xLength - 1]);

  // TODO: memory optimization
  // Don't create a list of edges, compute the edge on the go if not present?
  mp_obj_t * edgeItems;
  size_t nBins;
  // bin arg
  if (n_args >= 2 && (mp_obj_is_type(args[1], &mp_type_tuple) || mp_obj_is_type(args[1], &mp_type_list))) {
    size_t nEdges;
    mp_obj_get_array(args[1], &nEdges, &edgeItems);
    nBins = nEdges -1;
  } else {
    nBins = 10;
    if (n_args >= 2) {
      nBins = mp_obj_get_int(args[1]);
    }

    mp_float_t binWidth = (max-min)/nBins;
    // Handle empty range case
    if (max - min <= FLT_EPSILON) {
      binWidth = 1.0;
      nBins = 1;
    }
    // Create a array of bins
    edgeItems = m_new(mp_obj_t, nBins + 1);

    // Fill the bin edges list
    for (size_t i = 0; i < nBins+1; i++) {
      edgeItems[i] = mp_obj_new_float(min+i*binWidth);
    }
  }

  // Initialize bins list
  mp_obj_t * binItems = m_new(mp_obj_t, nBins);
  for (size_t i=0; i<nBins; i++) {
    binItems[i] = MP_OBJ_NEW_SMALL_INT(0);
  }

  // Fill bins list by linearly scanning the x and incrementing the bin count
  // Linearity is enabled thanks to sorting
  size_t binIndex = 0;
  size_t xIndex = 0;
  while (binIndex < nBins) {
    mp_float_t lowerBound = mp_obj_get_float(edgeItems[binIndex]);
    // Skip xItem if below the lower bound
    while (xIndex < xLength && mp_obj_get_float(xItems[xIndex]) < lowerBound) {
      xIndex++;
    }
    mp_float_t upperBound = mp_obj_get_float(edgeItems[binIndex+1]);
    while (xIndex < xLength && (mp_obj_get_float(xItems[xIndex]) < upperBound || (binIndex == nBins - 1 && mp_obj_get_float(xItems[xIndex]) == upperBound))) {
      assert(mp_obj_get_float(xItems[xIndex]) >= lowerBound);
      // Increment the bin count
      binItems[binIndex] = MP_OBJ_NEW_SMALL_INT(MP_OBJ_SMALL_INT_VALUE(binItems[binIndex]) + 1);
      xIndex++;
    }
    binIndex++;
  }

  // Setting hist color
  // color keyword
  KDColor color;
  // color keyword
  mp_map_elem_t * elem = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_color), MP_MAP_LOOKUP);
  colorFromKeywordArgument(elem, &color);

  for (size_t i=0; i<nBins; i++) {
    sPlotStore->addRect(edgeItems[i], edgeItems[i+1], binItems[i], mp_obj_new_float(0.0), color);
  }
  return mp_const_none;
}

/* scatter(x, y, KW : color)
 * - x, y: list
 * - x, y: scalar
 * */

mp_obj_t modpyplot_scatter(size_t n_args, const mp_obj_t *args, mp_map_t* kw_args) {
  assert(sPlotStore != nullptr);
  if (n_args < 2) {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,"scatter() takes 2 positional arguments but %d were given",n_args));
  }
  sPlotStore->setShow(true);
  mp_obj_t * xItems, * yItems;
  assert(n_args >= 2);
  size_t length = extractArgumentsAndCheckEqualSize(args[0], args[1], &xItems, &yItems);

  // Setting scatter color
  // color keyword
  KDColor color;
  // c keyword
  mp_map_elem_t * elem = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_c), MP_MAP_LOOKUP);
  colorFromKeywordArgument(elem, &color);
  // color keyword
  elem = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_color), MP_MAP_LOOKUP);
  colorFromKeywordArgument(elem, &color);

  for (size_t i=0; i<length; i++) {
    sPlotStore->addDot(xItems[i], yItems[i], color);
  }

  return mp_const_none;
}

/* plot(x, y) plots the curve (x, y, KW : color)
 * plot(y) plots the curve x as index array ([0,1,2...],y)
 * */

mp_obj_t modpyplot_plot(size_t n_args, const mp_obj_t *args,mp_map_t* kw_args) {
  assert(sPlotStore != nullptr);
  sPlotStore->setShow(true);
  if (n_args > 3) {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,"plot() takes 3 positional arguments but %d were given",n_args));
  }
  mp_obj_t * xItems, * yItems;
  size_t length;
  if (n_args == 1) {
    length = extractArgument(args[0], &yItems);

    // Create the default xItems: [0, 1, 2,...]
    xItems = m_new(mp_obj_t, length);
    for (size_t i = 0; i < length; i++) {
      xItems[i] = mp_obj_new_float((float)i);
    }
  } else {
    assert(n_args >= 2);
    length = extractArgumentsAndCheckEqualSize(args[0], args[1], &xItems, &yItems);
  }

  // Setting plot color
  KDColor color;
  bool isUserSet = false;
  // c keyword
  mp_map_elem_t * elem = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_c), MP_MAP_LOOKUP);
  isUserSet = colorFromKeywordArgument(elem, &color);
  // color keyword
  elem = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_color), MP_MAP_LOOKUP);
  isUserSet = isUserSet | colorFromKeywordArgument(elem, &color);
  // Eventual third positional argument
  if (!isUserSet && n_args >= 3) {
    color = MicroPython::Color::Parse(args[2]);
  }

  for (int i=0; i<(int)length-1; i++) {
    sPlotStore->addSegment(xItems[i], yItems[i], xItems[i+1], yItems[i+1], color);
  }

  return mp_const_none;
}

mp_obj_t modpyplot_text(mp_obj_t x, mp_obj_t y, mp_obj_t s) {
  assert(sPlotStore != nullptr);
  sPlotStore->setShow(true);
  // Input parameter validation
  mp_obj_get_float(x);
  mp_obj_get_float(y);
  mp_obj_str_get_str(s);

  sPlotStore->addLabel(x, y, s);

  return mp_const_none;
}

mp_obj_t modpyplot_show() {
  if (!sPlotStore->show()) {
    return mp_const_none;
  }
  MicroPython::ExecutionEnvironment * env = MicroPython::ExecutionEnvironment::currentExecutionEnvironment();
  sPlotController->setMicroPythonExecutionEnvironment(env);
  /* WARNING: We never unset the MicroPython::ExecutionEnvironment, so there
   * might be a case of dangling pointer, as sPlotController is never destroyed.
   * It's fine as long as we don't call sPlotController's PlotView's drawRect
   * without re-setting its environment before. */
  env->displayViewController(sPlotController);
  sPlotStore->setShow(false);
  return mp_const_none;
}
