#pragma once

namespace Inference {

/* Dimensions of "reusable cells", which are the number of cells that can appear
 * on screen at the same time. These dimensions take the "inner" table into
 * account. */
struct ReusableCellDimensions {
  int rows;
  int columns;
};

// Number of reusable cells of each type ("inner" or "header" cells)
struct ReusableCellCategories {
  int header;
  int inner;
};

struct ReusableCellCounts {
  ReusableCellDimensions dimensions;
  ReusableCellCategories categories;
};

// Dimensions of the "inner table", not counting row or column headers.
struct InnerTableDimensions {
  int rows;
  int columns;
};

struct TableShape {
  InnerTableDimensions inner;
  ReusableCellCounts reusable;
};

constexpr ReusableCellCounts ReusableCountsOneColumnHeader(
    ReusableCellDimensions dimensions) {
  return ReusableCellCounts{.dimensions = dimensions,
                            .categories = ReusableCellCategories{
                                .header = dimensions.columns,
                                .inner = dimensions.columns * dimensions.rows}};
}

constexpr ReusableCellCounts ReusableCountsOneRowOneColumnHeader(
    ReusableCellDimensions dimensions) {
  return ReusableCellCounts{.dimensions = dimensions,
                            .categories = ReusableCellCategories{
                                .header = dimensions.rows + dimensions.columns,
                                .inner = dimensions.columns * dimensions.rows}};
}

constexpr ReusableCellCounts ReusableCountsTwoColumnHeadersOneRowHeader(
    ReusableCellDimensions dimensions) {
  return ReusableCellCounts{
      .dimensions = dimensions,
      .categories = ReusableCellCategories{
          .header = 2 * dimensions.rows + dimensions.columns,
          .inner = dimensions.columns * dimensions.rows}};
}

}  // namespace Inference
