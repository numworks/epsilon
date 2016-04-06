extern "C" {
#include <assert.h>
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>
#include "../poincare/src/layout/string_layout.h"

#include "utils.h"

const char* kParsingErrorMessage = "PARSING ERROR";

void plot(Expression * e, float xMin, float xMax, float yMin, float yMax) {
  Context plotContext;
  KDCoordinate screenWidth = SCREEN_WIDTH;
  KDCoordinate screenHeight = SCREEN_HEIGHT;
  KDPoint previousPoint;
  for (KDCoordinate i=0;i<screenWidth; i++) {
    float x = xMin + (xMax-xMin)/screenWidth*i;
    Float xExp = Float(x);
    plotContext.setExpressionForSymbolName(&xExp, "x");
    float y = e->approximate(plotContext);
    KDCoordinate j = ((y-yMin)/(yMax-yMin)*screenHeight);
    KDPoint currentPoint = KDPointMake(i,screenHeight-j);
    if (i>0) {
      KDDrawLine(previousPoint, currentPoint, 0xFF);
    }
    previousPoint = currentPoint;
  }
}

//////////////////////////////////////////////////////

#define INPUT_MEMORY 15

typedef struct user_expression_t {
  char* text_input;
  Expression* expression;
  Expression* simplified;
  ExpressionLayout* expression_layout;
  ExpressionLayout* simplified_layout;
} user_expression_t;

class UserExpressions {
  public:
    UserExpressions() {
      m_numberOfExpressions = 0;
      m_position = INPUT_MEMORY;
      for (int i=0; i<INPUT_MEMORY; i++) {
        m_expressions[i] = {nullptr, nullptr, nullptr, nullptr, nullptr};
      }
    }

    // Adds a user expression as the most recent one.
    void append_expression(user_expression_t user_expression) {
      if (m_numberOfExpressions < INPUT_MEMORY) {
        m_numberOfExpressions++;
      }
      m_position = (m_position + 1) % INPUT_MEMORY;
      if (m_position < 0) {
        m_numberOfExpressions += INPUT_MEMORY;
      }

      // The circular buffer is full, now we have to erase stuff.
      if (m_numberOfExpressions == INPUT_MEMORY) {
        user_expression_t tmp = m_expressions[m_position];
        if (tmp.text_input) {
          free(tmp.text_input);
          if (tmp.expression) {
            free(tmp.expression);
            if (tmp.expression_layout) {
              free(tmp.expression_layout);
            }
            if (tmp.simplified && tmp.simplified != tmp.expression) {
              free(tmp.simplified);
              if (tmp.simplified_layout) {
                free(tmp.simplified_layout);
              }
            }
          }
        }
      }
      m_expressions[m_position] = user_expression;
    }

    // Gets the i th latest expression.
    user_expression_t get_expression(uint8_t index) const {
      //assert(index < m_numberOfExpressions);
      int pos = (m_position - index) % INPUT_MEMORY;
      if (pos < 0) {
        pos += INPUT_MEMORY;
      }
      return m_expressions[pos];
    }

    uint8_t numberOfExpressions() const {
      return m_numberOfExpressions;
    }

  private:
    uint8_t m_numberOfExpressions;
    uint8_t m_position;
    user_expression_t m_expressions[INPUT_MEMORY];
};

/////////////////////////////////////////////////

static user_expression_t create_user_input(char* text) {
  user_expression_t user_expression = {text, nullptr, nullptr, nullptr, nullptr};
  user_expression.expression = Expression::parse(user_expression.text_input);
  if (user_expression.expression) {
    user_expression.expression_layout = user_expression.expression->createLayout();
    user_expression.simplified = user_expression.expression->simplify();
    user_expression.simplified_layout = user_expression.simplified->createLayout();
  } else {
    user_expression.expression_layout =
      new StringLayout(kParsingErrorMessage, strlen(kParsingErrorMessage));
  }
  return user_expression;
}

static int16_t print_user_input(user_expression_t user_expression, int16_t yOffset) {
  if (user_expression.expression_layout) {
    int16_t height = user_expression.expression_layout->size().height;
    if (yOffset + height < SCREEN_HEIGHT) {
      user_expression.expression_layout->draw(KDPointMake(0, yOffset));
    }
    yOffset += height;
  }
  if (user_expression.simplified_layout) {
    int16_t height = user_expression.simplified_layout->size().height;
    if (yOffset + height < SCREEN_HEIGHT) {
      int16_t xOffset = SCREEN_WIDTH - user_expression.simplified_layout->size().width;
      user_expression.simplified_layout->draw(KDPointMake(xOffset, yOffset));
    }
    yOffset += height;
  }
  return yOffset;
}

static void print_user_inputs(const UserExpressions& user_inputs, uint8_t startingAt) {
  int16_t yOffset = 0;
  for (uint8_t i=startingAt; i<user_inputs.numberOfExpressions(); i++) {
    yOffset = print_user_input(user_inputs.get_expression(i), yOffset);
    if (yOffset>SCREEN_HEIGHT) {
      break;
    }
  }
}

static void interactive_expression_parsing() {
  UserExpressions user_inputs = UserExpressions();
  int index = 0;
  while (1) {
    text_event_t text_event;
    if (index == 0) {
      text_event = get_text(nullptr);
    } else {
      text_event = get_text(user_inputs.get_expression(index - 1).text_input);
    }
    if (text_event.event == EQUAL) {
      user_inputs.append_expression(create_user_input(text_event.text));
    } else if (text_event.event == UP_ARROW) {
      index--;
      if (index < 0) {
        index = 0;
      }
    } else if (text_event.event == DOWN_ARROW) {
      if (user_inputs.numberOfExpressions() != 0) {
        index++;
        if (index >= user_inputs.numberOfExpressions()) {
          index = user_inputs.numberOfExpressions() - 1;
        }
      }
    } else {
      assert(false); // unreachable.
    }
    clear_screen();
    print_user_inputs(user_inputs, index ? index - 1 : 0);
  }
}

void ion_app() {
  interactive_expression_parsing();
  while (1) {
    ion_sleep();
  }
}
