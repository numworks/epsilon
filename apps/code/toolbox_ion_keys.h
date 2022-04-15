#include <escher.h>

namespace Code {

  class ToolboxIonKeys : public ViewController {
    public :
      ToolboxIonKeys();
      View * view() override;
      bool handleEvent(Ion::Events::Event e) override;
      void setSender(InputEventHandler * sender) { m_sender = sender; }
    private :
      class toolboxIonView : public View {
        public :
          toolboxIonView();
          void drawRect(KDContext * ctx, KDRect rect) const override;
      };
      toolboxIonView m_view;
      InputEventHandler * m_sender;
  };

}
