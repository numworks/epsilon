#include <quiz.h>
#include <assert.h>
#include <ion/events.h>
#include <escher/clipboard.h>
#include <escher/layout_field.h>

using namespace Escher;

void assert_events_lead_to_selection(const Ion::Events::Event * events, int numberOfEvents, const char * selectedParsedAndSerializedText) {
  LayoutField field = LayoutField(nullptr, nullptr);
  field.setEditing(true);
  for (int i = 0; i < numberOfEvents; i++) {
    field.handleEvent(events[i]);
  }
  Clipboard::SharedClipboard()->reset();
  field.handleEvent(Ion::Events::Copy);
  quiz_assert(strcmp(Clipboard::SharedClipboard()->storedText(), selectedParsedAndSerializedText) == 0);
}

QUIZ_CASE(escher_layout_field_select_left_right) {
  // 123
  {
    // Select in a simple horizontal layout towards the left
    const int eventsCount = 6;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One, Ion::Events::Two, Ion::Events::Three,
      Ion::Events::ShiftLeft, Ion::Events::ShiftLeft, Ion::Events::ShiftLeft};
    assert_events_lead_to_selection(events, eventsCount - 2, "3");
    assert_events_lead_to_selection(events, eventsCount - 1, "23");
    assert_events_lead_to_selection(events, eventsCount, "123");
  }
  {
    // Select in a simple horizontal layout towards the right
    const int eventsCount = 9;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One, Ion::Events::Two, Ion::Events::Three,
      Ion::Events::Left, Ion::Events::Left, Ion::Events::Left,
      Ion::Events::ShiftRight, Ion::Events::ShiftRight, Ion::Events::ShiftRight};
    assert_events_lead_to_selection(events, eventsCount - 2, "1");
    assert_events_lead_to_selection(events, eventsCount - 1, "12");
    assert_events_lead_to_selection(events, eventsCount, "123");
  }
  {
    // Select in a simple horizontal layout left then right
    const int eventsCount = 7;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One, Ion::Events::Two, Ion::Events::Three,
      Ion::Events::Left, Ion::Events::ShiftLeft, Ion::Events::ShiftLeft,
      Ion::Events::ShiftRight};
    assert_events_lead_to_selection(events, eventsCount, "2");
  }
  {
    /* Select in a simple horizontal layout towards the right and hit the end of
     * layout. */
    const int eventsCount = 11;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One, Ion::Events::Two, Ion::Events::Three,
      Ion::Events::Left, Ion::Events::Left, Ion::Events::Left,
      Ion::Events::ShiftRight, Ion::Events::ShiftRight, Ion::Events::ShiftRight,
      Ion::Events::ShiftRight, Ion::Events::ShiftRight};
    assert_events_lead_to_selection(events, eventsCount, "123");
  }
  {
    /* Select in a simple horizontal layout towards the right, hit the end of
     * layout and select left. */
    const int eventsCount = 11;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One, Ion::Events::Two, Ion::Events::Three,
      Ion::Events::Left, Ion::Events::Left, Ion::Events::Left,
      Ion::Events::ShiftRight, Ion::Events::ShiftRight, Ion::Events::ShiftRight,
      Ion::Events::ShiftRight, Ion::Events::ShiftLeft};
    assert_events_lead_to_selection(events, eventsCount, "12");
  }

  /*    g34
   *  1+---+78
   *    h56    */
  {
    /* Select in a horizontal layout with a fraction, starting outside the
     * fraction. */
    const int eventsCount = 17;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One,    Ion::Events::Plus,  Ion::Events::LowerG,
      Ion::Events::Three,  Ion::Events::Four,  Ion::Events::Division,
      Ion::Events::LowerH, Ion::Events::Five,  Ion::Events::Six,
      Ion::Events::Right,  Ion::Events::Plus,  Ion::Events::Seven,
      Ion::Events::Eight, Ion::Events::ShiftLeft, Ion::Events::ShiftLeft,
      Ion::Events::ShiftLeft, Ion::Events::ShiftLeft};
    assert_events_lead_to_selection(events, eventsCount, "g34/h56+78");
  }
  {
    /* Select in a horizontal layout with a fraction, starting inside the
     * fraction. */
    const int eventsCount = 21;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One,    Ion::Events::Plus, Ion::Events::LowerG,
      Ion::Events::Three,  Ion::Events::Four, Ion::Events::Division,
      Ion::Events::LowerH, Ion::Events::Five, Ion::Events::Six,
      Ion::Events::Right,  Ion::Events::Plus, Ion::Events::Seven,
      Ion::Events::Eight,  Ion::Events::Left, Ion::Events::Left,
      Ion::Events::Left,   Ion::Events::Left, Ion::Events::Left,
      Ion::Events::ShiftLeft, Ion::Events::ShiftLeft, Ion::Events::ShiftLeft,
    };
    assert_events_lead_to_selection(events, eventsCount, "g34/h56");
  }
}

QUIZ_CASE(escher_layout_field_select_up_down) {
  // 12
  {
    // Select in a simple horizontal layout up
    const int eventsCount = 5;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One, Ion::Events::Two, Ion::Events::ShiftLeft,
      Ion::Events::ShiftUp};
    assert_events_lead_to_selection(events, eventsCount, "2");
  }
   {
    // Select in a simple horizontal layout up
    const int eventsCount = 5;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One, Ion::Events::Two, Ion::Events::ShiftLeft,
      Ion::Events::ShiftDown};
    assert_events_lead_to_selection(events, eventsCount, "2");
  }
  /*  123
   *  ---
   *  456  */
  {
    // Select up in a fraction
    const int eventsCount = 11;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One,      Ion::Events::Two,  Ion::Events::Three,
      Ion::Events::Division, Ion::Events::Four, Ion::Events::Five,
      Ion::Events::Six,      Ion::Events::Left, Ion::Events::ShiftLeft,
      Ion::Events::ShiftUp,  Ion::Events::ShiftRight};
    assert_events_lead_to_selection(events, eventsCount - 1, "123/456");
    assert_events_lead_to_selection(events, eventsCount, "");
  }
  {
    // Select down in a fraction
    const int eventsCount = 12;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One,       Ion::Events::Two,  Ion::Events::Three,
      Ion::Events::Division,  Ion::Events::Four, Ion::Events::Five,
      Ion::Events::Six,       Ion::Events::Left, Ion::Events::Up,
      Ion::Events::ShiftLeft, Ion::Events::ShiftDown, Ion::Events::ShiftLeft};
    assert_events_lead_to_selection(events, eventsCount - 1, "123/456");
    assert_events_lead_to_selection(events, eventsCount, "");
  }
  /*     123
   *  89+---
   *     456  */
  {
    // Select up next to a fraction (cursor on fraction)
    const int eventsCount = 15;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::Eight,    Ion::Events::Nine, Ion::Events::Plus,
      Ion::Events::One,      Ion::Events::Two,  Ion::Events::Three,
      Ion::Events::Division, Ion::Events::Four, Ion::Events::Five,
      Ion::Events::Six,      Ion::Events::Left, Ion::Events::Left,
      Ion::Events::Left,     Ion::Events::Left, Ion::Events::ShiftUp};
    assert_events_lead_to_selection(events, eventsCount, "");
  }
  {
    // Select down next to a fraction (cursor not on the fraction)
    const int eventsCount = 17;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::Eight,    Ion::Events::Nine, Ion::Events::Plus,
      Ion::Events::One,      Ion::Events::Two,  Ion::Events::Three,
      Ion::Events::Division, Ion::Events::Four, Ion::Events::Five,
      Ion::Events::Six,      Ion::Events::Left, Ion::Events::Left,
      Ion::Events::Left,     Ion::Events::Left, Ion::Events::Left,
      Ion::Events::Right,    Ion::Events::ShiftDown};
    assert_events_lead_to_selection(events, eventsCount, "");
  }
  /*  12345
   *  -----
   *   ghi
   *   ----
   *   k|lm  */
  {
    // Select up next to a fraction (cursor on fraction)
    const int eventsCount = 17;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One,      Ion::Events::Two,    Ion::Events::Three,
      Ion::Events::Four,     Ion::Events::Five,   Ion::Events::Division,
      Ion::Events::LowerG,   Ion::Events::LowerH, Ion::Events::LowerI,
      Ion::Events::Division, Ion::Events::LowerK, Ion::Events::LowerL,
      Ion::Events::LowerM,   Ion::Events::Left,   Ion::Events::Left,
      Ion::Events::ShiftUp,  Ion::Events::ShiftUp

    };
    assert_events_lead_to_selection(events, eventsCount - 1, "ghi/klm");
    assert_events_lead_to_selection(events, eventsCount, "12345/\x12ghi/klm\x13");
  }
}


QUIZ_CASE(escher_layout_field_unselect) {
  // 1234
  {
    // Move left after selecting
    const int eventsCount = 8;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One,  Ion::Events::Two, Ion::Events::Three,
      Ion::Events::Four, Ion::Events::ShiftLeft, Ion::Events::Left,
      Ion::Events::Left, Ion::Events::ShiftRight};
    assert_events_lead_to_selection(events, eventsCount, "3");
  }
}

QUIZ_CASE(escher_layout_field_delete) {
  // 1234
  {
    // Delete the selection
    const int eventsCount = 17;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One,  Ion::Events::Two, Ion::Events::Three,
      Ion::Events::Four, Ion::Events::Left, Ion::Events::ShiftLeft,
      Ion::Events::ShiftLeft, Ion::Events::Backspace, Ion::Events::Left,
      Ion::Events::ShiftRight, Ion::Events::ShiftRight
    };
    assert_events_lead_to_selection(events, eventsCount, "14");
  }
  {
    // Delete the by adding text
    const int eventsCount = 19;
    Ion::Events::Event events[eventsCount] = {
      Ion::Events::One,  Ion::Events::Two, Ion::Events::Three,
      Ion::Events::Four, Ion::Events::Left, Ion::Events::ShiftLeft,
      Ion::Events::ShiftLeft, Ion::Events::Zero, Ion::Events::Left,
      Ion::Events::Left, Ion::Events::ShiftRight, Ion::Events::ShiftRight,
      Ion::Events::ShiftRight
    };
    assert_events_lead_to_selection(events, eventsCount, "104");
  }

}
