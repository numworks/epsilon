#include <quiz.h>
#include "execution_environment.h"

static const char * s_pyplotArrowScript = R"(#
from matplotlib.pyplot import *
arrow(2,3,4,5)
show()
print('ok')
)";

static const char * s_pyplotAxisScript = R"(#
from matplotlib.pyplot import *
axis((2,3,4,5))
axis([2,3,4,5])
print(axis())
scatter(0,1)
show()
print('ok')
)";

static const char * s_pyplotAxisErrorScript = R"(#
from matplotlib.pyplot import *
axis(2,3,4,5)
print('fail!')
)";

static const char * s_pyplotBarScript = R"(#
from matplotlib.pyplot import *
bar([0,2,3],[10,12,23])
bar([0,2,3],10)
bar([],[])
bar([1,2,3],[1,2,3],2,3)
bar([1,2,3],[1,2,3],[1,2,3],[1,2,3])
show()
print('ok')
)";

static const char * s_pyplotBarErrorScript = R"(#
from matplotlib.pyplot import *
bar([1,2,3],[1,2,3,4],[1,2,3],[1,2,3])
show()
print('fail!')
)";

static const char * s_pyplotGridScript = R"(#
from matplotlib.pyplot import *
grid(True)
grid()
show()
print('ok')
)";

static const char * s_pyplotHistScript = R"(#
from matplotlib.pyplot import *
hist([2,3,4,5,6])
hist([2,3,4,5,6],23)
hist([2,3,4,5,6],[0,2,3])
hist([2,3,4,5,6],[0,2,3, 4,5,6,7])
show()
print('ok')
)";

static const char * s_pyplotPlotScript = R"(#
from matplotlib.pyplot import *
plot([2,3,4,5,6])
plot([2,3,4,5,6],[3,4,5,6,7])
show()
print('ok')
)";

static const char * s_pyplotPlotErrorScript = R"(#
from matplotlib.pyplot import *
plot([2,3,4,5,6],2)
print('Fail!')
)";

static const char * s_pyplotScatterScript = R"(#
from matplotlib.pyplot import *
scatter(2,3)
scatter([2,3,4,5,6],[3,4,5,6,7])
show()
print('ok')
)";

static const char * s_pyplotScatterErrorScript = R"(#
from matplotlib.pyplot import *
scatter([2,3,4,5,6],2)
print('Fail!')
)";

static const char * s_pyplotTextScript = R"(#
from matplotlib.pyplot import *
text(2,3, "hello")
show()
print('ok')
)";

QUIZ_CASE(python_matplotlib_pyplot) {
  assert_script_execution_succeeds(s_pyplotArrowScript);
  assert_script_execution_succeeds(s_pyplotAxisScript);
  assert_script_execution_succeeds(s_pyplotAxisErrorScript);
  assert_script_execution_succeeds(s_pyplotBarScript);
  assert_script_execution_succeeds(s_pyplotBarErrorScript);
  assert_script_execution_succeeds(s_pyplotGridScript);
  assert_script_execution_succeeds(s_pyplotHistScript);
  assert_script_execution_succeeds(s_pyplotPlotScript);
  assert_script_execution_succeeds(s_pyplotPlotErrorScript);
  assert_script_execution_succeeds(s_pyplotScatterScript);
  assert_script_execution_succeeds(s_pyplotScatterErrorScript);
  assert_script_execution_succeeds(s_pyplotTextScript);
}
