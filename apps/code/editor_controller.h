#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include "editor_view.h"
#include "python_variable_box_controller.h"
#include "script.h"

namespace Code {

class MenuController;
class ScriptParameterController;
class App;

class EditorController : public Escher::ViewController {
 public:
  EditorController(MenuController* menuController, App* pythonDelegate);
  void setScript(Script script, int scriptIndex);
  int scriptIndex() const { return m_scriptIndex; }
  void willExitApp();

  /* ViewController */
  Escher::View* view() override { return &m_editorView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  Escher::ViewController::TitlesDisplay titlesDisplay() const override {
    return Escher::ViewController::TitlesDisplay::DisplayNoTitle;
  }

 private:
  void cleanStorageEmptySpace();
  Escher::StackViewController* stackController();
  EditorView m_editorView;
  Script m_script;
  int m_scriptIndex;
  MenuController* m_menuController;
};

}  // namespace Code

#endif
