#include "script_template.h"

namespace Code {

constexpr ScriptTemplate emptyScriptTemplate(".py", "\x01" R"(from math import *
from mathsup import *
)");


const ScriptTemplate * ScriptTemplate::Empty() {
  return &emptyScriptTemplate;
}

}
