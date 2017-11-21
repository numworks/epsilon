#ifndef PYTHON_PORT_H
#define PYTHON_PORT_H

namespace MicroPython {

class ScriptProvider {
public:
  virtual const char * contentOfScript(const char * name) = 0;
};

class ExecutionEnvironment {
public:
  ExecutionEnvironment();
  static ExecutionEnvironment * currentExecutionEnvironment();
  void didModifyFramebuffer();
  bool isFramebufferModified() const { return m_framebufferHasBeenModified; }
  void didCleanFramebuffer();
  void runCode(const char * );
  virtual void printText(const char * text, size_t length) {
  }
  virtual void redraw() {
  }
private:
  bool m_framebufferHasBeenModified;
};

void init(void * heapStart, void * heapEnd);
void deinit();
void registerScriptProvider(ScriptProvider * s);

};

// Will implement :
// mp_lexer_new_from_file -> Ask the context about a file
// mp_import_stat
// mp_hal_stdout_tx_strn_cooked -> Tell the context Python printed text

#endif
