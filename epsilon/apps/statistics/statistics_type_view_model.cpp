#include "statistics_type_view_model.h"

namespace Statistics {

I18n::Message DataTypeViewModel::MessageForDataType(DataType dataType) {
  constexpr I18n::Message k_messages[k_numberOfDataTypes] = {
      I18n::Message::Quantitative, I18n::Message::Categorical};
  return k_messages[IndexOfDataTypeView(dataType)];
}

I18n::Message DataTypeViewModel::SubtitleForDataType(DataType dataType) {
  constexpr I18n::Message k_subtitles[k_numberOfDataTypes] = {
      I18n::Message::QuantitativeDescriptor,
      I18n::Message::CategoricalDescriptor,
  };
  return k_subtitles[IndexOfDataTypeView(dataType)];
}

DataTypeViewModel::DataType DataTypeViewModel::DataTypeAtIndex(uint8_t index) {
  assert(index < k_numberOfDataTypes);
  return static_cast<DataType>(index);
}

uint8_t DataTypeViewModel::IndexOfDataTypeView(DataType dataType) {
  assert(static_cast<uint8_t>(dataType) < k_numberOfDataTypes);
  return static_cast<uint8_t>(dataType);
}

}  // namespace Statistics
