#include "elements_data_base.h"
#include <assert.h>

namespace Periodic {

constexpr static ElementData k_elementsData[ElementsDataBase::k_numberOfElements] = {
  ElementData("H", I18n::Message::HName, 1, 1.008, 2.2, 32, -259.16, -252.879, 8.8e-5, 72.7700191, 1312.04602, ElementData::Group::Nonmetal, ElementData::PhysicalState::Gas, ElementData::Block::S),
};

const ElementData * ElementsDataBase::DataForElement(AtomicNumber z) {
  assert(z >= 1 && z <= k_numberOfElements);
  return k_elementsData + (z - 1);
}

}
