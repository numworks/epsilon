#include <quiz.h>
#include "execution_environment.h"

static const char * s_pyplotArrowScript = R"(#
from matplotlib.pyplot import *
arrow(2,3,4,5)
show()
)";

QUIZ_CASE(python_matplotlib_pyplot_arrow) {
  assert_script_execution_succeeds(s_pyplotArrowScript);
}

static const char * s_pyplotAxisScript = R"(#
from matplotlib.pyplot import *
axis((2,3,4,5))
axis([2,3,4,5])
print(axis())
scatter(0,1)
show()
)";

static const char * s_pyplotAxisErrorScript = R"(#
from matplotlib.pyplot import *
axis(2,3,4,5)
)";

QUIZ_CASE(python_matplotlib_pyplot_axis) {
  assert_script_execution_succeeds(s_pyplotAxisScript, "(2.0, 3.0, 4.0, 5.0)\n");
  assert_script_execution_fails(s_pyplotAxisErrorScript);
}

static const char * s_pyplotBarScript = R"(#
from matplotlib.pyplot import *
bar([0,2,3],[10,12,23])
bar([0,2,3],10)
bar([],[])
bar([1,2,3],[1,2,3],2,3)
bar([1,2,3],[1,2,3],[1,2,3],[1,2,3])
show()
)";

static const char * s_pyplotBarErrorScript = R"(#
from matplotlib.pyplot import *
bar([1,2,3],[1,2,3,4],[1,2,3],[1,2,3])
show()
)";

QUIZ_CASE(python_matplotlib_pyplot_bar) {
  assert_script_execution_succeeds(s_pyplotBarScript);
  assert_script_execution_fails(s_pyplotBarErrorScript);
}

static const char * s_pyplotGridScript = R"(#
from matplotlib.pyplot import *
grid(True)
grid()
show()
)";

QUIZ_CASE(python_matplotlib_pyplot_grid) {
  assert_script_execution_succeeds(s_pyplotGridScript);
}

static const char * s_pyplotHistScript = R"(#
from matplotlib.pyplot import *
hist([2,3,4,5,6])
hist([2,3,4,5,6],23)
hist([2,3,4,5,6],[0,2,3])
hist([2,3,4,5,6],[0,2,3, 4,5,6,7])
show()
)";

QUIZ_CASE(python_matplotlib_pyplot_hist) {
  assert_script_execution_succeeds(s_pyplotHistScript);
}

static const char * s_pyplotPlotScript = R"(#
from matplotlib.pyplot import *
plot([2,3,4,5,6])
plot([2,3,4,5,6],[3,4,5,6,7])
show()
)";

static const char * s_pyplotPlotErrorScript = R"(#
from matplotlib.pyplot import *
plot([2,3,4,5,6],2)
)";

QUIZ_CASE(python_matplotlib_pyplot_plot) {
  assert_script_execution_succeeds(s_pyplotPlotScript);
  assert_script_execution_fails(s_pyplotPlotErrorScript);
}

static const char * s_pyplotScatterScript = R"(#
from matplotlib.pyplot import *
scatter(2,3)
scatter([2,3,4,5,6],[3,4,5,6,7])
show()
)";

static const char * s_pyplotScatterErrorScript = R"(#
from matplotlib.pyplot import *
scatter([2,3,4,5,6],2)
)";

QUIZ_CASE(python_matplotlib_pyplot_scatter) {
  assert_script_execution_succeeds(s_pyplotScatterScript);
  assert_script_execution_fails(s_pyplotScatterErrorScript);
}

static const char * s_pyplotTextScript = R"(#
from matplotlib.pyplot import *
text(2,3,'hello')
show()
)";

QUIZ_CASE(python_matplotlib_pyplot_text) {
  assert_script_execution_succeeds(s_pyplotTextScript);
}
