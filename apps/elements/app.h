#ifndef ELEMENTS_APP_H
#define ELEMENTS_APP_H

#include <apps/shared/app_with_store_menu.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>

#include "elements_view_data_source.h"
#include "main_controller.h"

namespace Elements {

class App : public Shared::AppWithStoreMenu {
 public:
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image* icon() const override;
  };

  class Snapshot : public Shared::SharedApp::Snapshot {
   public:
    Snapshot() { init(); };

    App* unpack(Escher::Container* container) override;
    const Descriptor* descriptor() const override;
    void reset() override;

    const DataField* field() const { return m_field; }
    void setField(const DataField* field) { m_field = field; }
    AtomicNumber selectedElement() const { return m_selectedElement; }
    void setSelectedElement(AtomicNumber selectedElement) {
      m_selectedElement = selectedElement;
    }
    AtomicNumber previousElement() const { return m_previousElement; }
    void setPreviousElement(AtomicNumber previousElement) {
      m_previousElement = previousElement;
    }

   private:
    void init();

    const DataField* m_field;
    AtomicNumber m_selectedElement;
    AtomicNumber m_previousElement;
  };
  TELEMETRY_ID("ElementsTable");

  static App* app() { return static_cast<App*>(Escher::App::app()); }

  Snapshot* snapshot() {
    return static_cast<Snapshot*>(Escher::App::snapshot());
  }
  ElementsViewDataSource* elementsViewDataSource() { return &m_dataSource; }

 private:
  App(Snapshot* snapshot);

  Escher::StackViewController::Default m_stackController;
  MainController m_mainController;
  ElementsViewDataSource m_dataSource;
};

}  // namespace Elements

#endif
