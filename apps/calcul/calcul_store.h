#ifndef CALCUL_CALCUL_STORE_H
#define CALCUL_CALCUL_STORE_H

#include "calcul.h"

namespace Calcul {

class CalculStore {
public:
  CalculStore();
  Calcul * calculAtIndex(int i);
  Calcul * addEmptyCalcul();
  void removeCalcul(Calcul * c);
  int numberOfCalculs();
private:
  static constexpr int k_maxNumberOfCalculs = 10;
  int m_numberOfCalculs;
  Calcul m_calculs[k_maxNumberOfCalculs];
};

}

#endif
