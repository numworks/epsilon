#pragma once

#include <apps/shared/math_app.h>
#include <apps/shared/menu_controller.h>
#include <escher/app.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>
#include <omg/ring_buffer.h>

#include "controllers/controller_container.h"
#include "controllers/tables/dynamic_cells_data_source.h"
#include "inference/controllers/results_controller.h"
#include "inference/controllers/tables/homogeneity_data_source.h"
#include "models/inference_buffer.h"

namespace Inference {

class App : public Shared::MathApp, public Shared::MenuControllerDelegate {
  using LargeStackViewController = Escher::StackViewController::Custom<9>;

 public:
  // Descriptor
  class Descriptor : public Escher::App::Descriptor {
   public:
    I18n::Message name() const override { return I18n::Message::InferenceApp; };
    I18n::Message upperName() const override {
      return I18n::Message::InferenceAppCapital;
    };
    const Escher::Image* icon() const override;
  };

  // Snapshot
  class Snapshot : public Shared::SharedApp::Snapshot {
   public:
    App* unpack(Escher::Container* container) override;
    const Descriptor* descriptor() const override;
    void tidy() override;
    void reset() override;

    InferenceModel* inference() { return m_inferenceBuffer.inference(); }
    const InferenceModel* inference() const {
      return m_inferenceBuffer.inference();
    }

    OMG::RingBuffer<Escher::ViewController*,
                    LargeStackViewController::k_maxNumberOfChildren>*
    pageQueue() {
      return &m_pageQueue;
    }

   private:
    friend App;
    // TODO: optimize size of Stack
    OMG::RingBuffer<Escher::ViewController*,
                    LargeStackViewController::k_maxNumberOfChildren>
        m_pageQueue;
    InferenceBuffer m_inferenceBuffer;
  };

  static App* app() { return static_cast<App*>(Escher::App::app()); }
  void didBecomeActive(Escher::Window* window) override;
  bool storageCanChangeForRecordName(
      const Ion::Storage::Record::Name recordName) const override;

  // Navigation
  void willOpenPage(Escher::ViewController* controller) override;
  void didExitPage(Escher::ViewController* controller) override;

  // Cells buffer API
  void* buffer(size_t offset = 0) { return m_buffer + offset; }
  void cleanBuffer(DynamicCellsDataSourceDestructor* destructor);

  constexpr static int k_bufferSize =  // 21056
      std::max(
          {sizeof(ResultCell) *
               ResultsControllerDimensions::k_numberOfReusableCells,
           sizeof(ParameterCell) *
               InputControllerDimensions::k_maxNumberOfParameterCell,
           sizeof(InferenceEvenOddBufferCell) *
               (HomogeneityTableDimensions::k_numberOfHeaderReusableCells +
                HomogeneityTableDimensions::k_numberOfInnerReusableCells),
           sizeof(InferenceEvenOddEditableCell) *
                   ANOVATableDimensions::k_numberOfInputInnerReusableCells +
               sizeof(InferenceEvenOddBufferCell) *
                   ANOVATableDimensions::k_numberOfInputHeaderReusableCells,
           sizeof(InferenceEvenOddEditableCell) *
                   HomogeneityTableDimensions::k_numberOfInnerReusableCells +
               sizeof(InferenceEvenOddBufferCell) *
                   HomogeneityTableDimensions::k_numberOfHeaderReusableCells,
           sizeof(InferenceEvenOddEditableCell) *
               DoubleColumnTableDimensions::k_numberOfReusableCells});

  // Shared::MenuControllerDelegate
  void selectSubApp(int subAppIndex) override;
  int selectedSubApp() const override {
    return static_cast<int>(snapshot()->inference()->subApp());
  }
  int numberOfSubApps() const override {
    return static_cast<int>(k_numberOfSubApps);
  }

  Escher::InputViewController* inputViewController() {
    return &m_inputViewController;
  }

 private:
  App(Snapshot* snapshot);
  Snapshot* snapshot() {
    return static_cast<Snapshot*>(Escher::App::snapshot());
  }
  const Snapshot* snapshot() const {
    return static_cast<const Snapshot*>(Escher::App::snapshot());
  }

  // Controllers
  ControllerContainer m_controllerContainer;
  Shared::MenuController m_menuController;
  LargeStackViewController m_stackViewController;
  Escher::InputViewController m_inputViewController;
  /* Buffer used for allocating table cells to avoid duplicating required
   * space for these memory-needy tables. */
  char m_buffer[k_bufferSize];
  DynamicCellsDataSourceDestructor* m_bufferDestructor;
};

}  // namespace Inference
