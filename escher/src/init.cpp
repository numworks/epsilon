#include <escher/init.h>
#include <kandinsky/ion_context.h>

namespace Escher {

void Init() { KDIonContext::SharedContext.init(); }

}  // namespace Escher
