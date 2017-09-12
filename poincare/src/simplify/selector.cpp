#include "selector.h"
#include "combination.h"
#include <assert.h>

namespace Poincare {

bool Selector::match(const Expression * e, Expression ** captures) const {
  // Si pour commencer, e ne correspond pas  mon petit test  moi perso, c'est mort!
  if (!immediateMatch(e)) {
    return false;
  }

  if (m_captureIndex >= 0) {
    assert(m_captureIndex < 5); // Le upper-bound calcul avant
    //FIXME: No cast
    captures[m_captureIndex] = (Expression *)e;
  }

  // Maintenant si mon petit test a march, encore faut-il que ceux des mes fils marchent aussi
  // En pratique, je veux retourner OUI ssi j'arrive  trouver une combinaison telle que chacun de mes fils matchent.
  // LA, chiant : eviter l'explosion combinatoire !!!
  // C'est MAINTENANT qu'on va utiliser le fait que e est triee !!
  // La difficult tant d'viter de considrer plein de cas inutiles

  // Ici, le code va tre un truc du genre
  //
  if (m_numberOfChildren == 0) {
    return true;
  }
  //
  Combination combination(m_children, m_numberOfChildren, e);
  while (combination.next()){
    bool allChildrenMatched = true;
    for (int i=0; i<m_numberOfChildren; i++) {
      const Selector * child = m_children[i];
      const Expression * expression = combination.expressionForSelectorIndex(i);
      if (!child->match(expression, captures)) {
        allChildrenMatched = false;
        break;
      }
    }
    if (allChildrenMatched) {
      return true;
    }
  }

  return false;
}

}
