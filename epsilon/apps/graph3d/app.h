#ifndef GRAPH3D_APP_H
#define GRAPH3D_APP_H

#include <escher/app.h>
#include <escher/view_controller.h>
#include <escher/view.h>
#include <escher/text_field_delegate.h>
#include <escher/expression_input_bar.h>
#include <escher/solid_color_view.h>
#include <apps/i18n.h>
#include <apps/shared/shared_app.h>

namespace Graph3d {

constexpr int k_gridSize = 60; // Maximize resolution to 60x60

class App : public Shared::SharedApp {
public:
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image * icon() const override;
  };

  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    Snapshot();
    App * unpack(Escher::Container * container) override;
    const Descriptor * descriptor() const override;
    void reset() override;
    void tidy() override;

    char m_surfaceExpression[256];
    float m_zGrid[k_gridSize+1][k_gridSize+1];
    bool m_gridNeedsUpdate;
  };

  App(Snapshot * snapshot);
  virtual ~App() = default;

  Snapshot * snapshot() const {
    return static_cast<Snapshot*>(const_cast<Escher::App::Snapshot*>(Escher::App::snapshot()));
  }

  void openMainView();
  void recalculateGrid();

private:
  class MainView : public Escher::View {
  public:
    MainView(App * app) : m_app(app), m_theta(0.5f), m_phi(0.5f) {}
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void setRotation(float dTheta, float dPhi) {
      m_theta += dTheta;
      m_phi += dPhi;
      markRectAsDirty(bounds());
    }
  private:
    App * m_app;
    float m_theta;
    float m_phi;
  };

  class MainViewController : public Escher::ViewController {
  public:
    MainViewController(Escher::Responder * parentResponder, App * app) : Escher::ViewController(parentResponder), m_view(app) {}
    Escher::View * view() override { return &m_view; }
    const char * title() const override { return "Graph3D"; }
    bool handleEvent(Ion::Events::Event event) override;
  private:
    MainView m_view;
  };

  class InputController : public Escher::ViewController, public Escher::LayoutFieldDelegate {
  public:
    InputController(Escher::Responder * parentResponder, App * app);
    Escher::View * view() override { return &m_contentView; }
    const char * title() const override { return "Input Eq"; }

    bool layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) override;
    bool layoutFieldDidFinishEditing(Escher::LayoutField * layoutField, Ion::Events::Event event) override;
    void layoutFieldDidAbortEditing(Escher::LayoutField * layoutField) override;
    void layoutFieldDidChangeSize(Escher::LayoutField * layoutField) override;

  protected:
    void handleResponderChainEvent(ResponderChainEvent event) override;

  private:
    App * m_app;
    class ContentView : public Escher::View {
    public:
      ContentView(Escher::Responder* parentResponder, Escher::LayoutFieldDelegate* layoutFieldDelegate)
          : m_expressionInputBar(parentResponder, layoutFieldDelegate) {}

      Escher::LayoutField* layoutField() { return m_expressionInputBar.layoutField(); }
      void reload() {
        layoutSubviews(true);
        markWholeFrameAsDirty();
      }

    private:
      int numberOfSubviews() const override { return 2; }
      Escher::View* subviewAtIndex(int index) override {
        if (index == 0) return &m_solidView;
        return &m_expressionInputBar;
      }
      void layoutSubviews(bool force = false) override {
        KDCoordinate inputHeight = m_expressionInputBar.minimalSizeForOptimalDisplay().height();
        setChildFrame(&m_solidView, KDRect(0, 0, bounds().width(), bounds().height() - inputHeight), force);
        setChildFrame(&m_expressionInputBar, KDRect(0, bounds().height() - inputHeight, bounds().width(), inputHeight), force);
      }
      Escher::SolidColorView m_solidView{KDColorWhite};
      Escher::ExpressionInputBar m_expressionInputBar;
    };
    ContentView m_contentView;
  };

  MainViewController m_mainViewController;
  InputController m_inputController;
};

}

#endif
