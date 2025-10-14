#pragma once

#include <apps/i18n.h>
#include <stdint.h>

namespace Statistics {

/* Model to store the selected data type in the snapshot */
class DataTypeViewModel {
 public:
  enum class DataType : uint8_t {
    Quantitative = 0,
    Qualitative = 1,
    NumberOfDataType,
  };
  constexpr static uint8_t k_numberOfDataTypes =
      static_cast<uint8_t>(DataType::NumberOfDataType);

  static I18n::Message MessageForDataType(DataType dataType);
  static I18n::Message SubtitleForDataType(DataType dataType);
  static DataType DataTypeAtIndex(uint8_t index);
  static uint8_t IndexOfDataTypeView(DataType dataType);

  DataTypeViewModel() : m_selectedDataType(DataType::Quantitative) {}
  DataType selectedDataType() const { return m_selectedDataType; }
  void selectDataType(DataType dataType) { m_selectedDataType = dataType; }
  bool isCategorical() const {
    return m_selectedDataType == DataType::Qualitative;
  }

 private:
  DataType m_selectedDataType = DataType::Quantitative;
};

}  // namespace Statistics
