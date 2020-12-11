#include "board.h"
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Board {

using namespace Regs;

void initFPU() {
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BABDBFBJ.html
  CORTEX.CPACR()->setAccess(10, CORTEX::CPACR::Access::Full);
  CORTEX.CPACR()->setAccess(11, CORTEX::CPACR::Access::Full);
  // FIXME: The pipeline should be flushed at this point
}


}
}
}
