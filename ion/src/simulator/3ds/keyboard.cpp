#include <ion/keyboard.h>
#include "platform.h"
#include "main.h"

#include <3ds.h>
#include <stdio.h>

void IonSimulatorKeyboardKeyDown(int keyNumber) {
}

void IonSimulatorKeyboardKeyUp(int keyNumber) {
}

#define DETECT_KEY(x1, y1, w, h, key) \
  if (touch.px >= (x1) && touch.py >= (y1) && touch.px <= ((x1) + (w) - 1) && touch.py <= ((y1) + (h) - 1)) state.setKey(key);

namespace Ion {
namespace Keyboard {

bool m_heldLastFrame = false;

State scan() {
	hidScanInput();
	
	State state;
	
	touchPosition touch;
	hidTouchRead(&touch);
	
	
	if (touch.px == 0 && touch.py == 0) {
	  if (m_heldLastFrame) {
	    m_heldLastFrame = false;
	  }
	  
	} else {
	  if (!m_heldLastFrame) {
	    m_heldLastFrame = true;
	    
      DETECT_KEY(271, 13 , 31, 31, Key::OK)
      DETECT_KEY(271, 46 , 31, 31, Key::Back)
      DETECT_KEY(227, 16 , 37, 25, Key::Home)
      DETECT_KEY(227, 48 , 37, 25, Key::OnOff)
      
      DETECT_KEY(174, 9  , 22, 23, Key::Up)
      DETECT_KEY(174, 54 , 22, 23, Key::Down)
      DETECT_KEY(151, 32 , 23, 22, Key::Left)
      DETECT_KEY(196, 32 , 23, 22, Key::Right)
      
      
      DETECT_KEY(3  , 30 , 41, 27, Key::LeftParenthesis)
      DETECT_KEY(52 , 30 , 41, 27, Key::RightParenthesis)
      DETECT_KEY(101, 30 , 41, 27, Key::Plus)
      
      DETECT_KEY(3  , 66 , 41, 27, Key::Multiplication)
      DETECT_KEY(52 , 66 , 41, 27, Key::Division)
      DETECT_KEY(101, 66 , 41, 27, Key::Minus)
      
      DETECT_KEY(3  , 102, 41, 27, Key::Seven)
      DETECT_KEY(52 , 102, 41, 27, Key::Eight)
      DETECT_KEY(101, 102, 41, 27, Key::Nine)
      
      DETECT_KEY(3  , 138, 41, 27, Key::Four)
      DETECT_KEY(52 , 138, 41, 27, Key::Five)
      DETECT_KEY(101, 138, 41, 27, Key::Six)
      
      DETECT_KEY(3  , 174, 41, 27, Key::One)
      DETECT_KEY(52 , 174, 41, 27, Key::Two)
      DETECT_KEY(101, 174, 41, 27, Key::Three)
      
      DETECT_KEY(3  , 210, 41, 27, Key::Zero)
      DETECT_KEY(52 , 210, 41, 27, Key::Dot)
      DETECT_KEY(101, 210, 41, 27, Key::EE)
      DETECT_KEY(150, 210, 41, 27, Key::Ans)
      DETECT_KEY(199, 210, 41, 27, Key::EXE)
      
      
      DETECT_KEY(149, 82 , 34, 23, Key::Sqrt)
      DETECT_KEY(189, 82 , 34, 23, Key::Square)
      DETECT_KEY(229, 82 , 34, 23, Key::Comma)
      DETECT_KEY(269, 82 , 34, 23, Key::Power)
      
      DETECT_KEY(149, 114, 34, 23, Key::Shift)
      DETECT_KEY(189, 114, 34, 23, Key::Alpha)
      DETECT_KEY(229, 114, 34, 23, Key::XNT)
      DETECT_KEY(269, 114, 34, 23, Key::Var)
      
      DETECT_KEY(149, 146, 34, 23, Key::Exp)
      DETECT_KEY(189, 146, 34, 23, Key::Ln)
      DETECT_KEY(229, 146, 34, 23, Key::Log)
      DETECT_KEY(269, 146, 34, 23, Key::Imaginary)
      
      DETECT_KEY(149, 178, 34, 23, Key::Sine)
      DETECT_KEY(189, 178, 34, 23, Key::Cosine)
      DETECT_KEY(229, 178, 34, 23, Key::Tangent)
      DETECT_KEY(269, 178, 34, 23, Key::Pi)
      
      
      DETECT_KEY(245, 212, 34, 23, Key::Toolbox)
      DETECT_KEY(285, 212, 34, 23, Key::Backspace)
      
      
	    // printf("\x1b[2;0H%03d; %03d", touch.px, touch.py);
	    // Handle touchscreen here.
	  }
	}
	
	u32 kDown = hidKeysDown();
	
	if (kDown & KEY_DUP) {
	  state.setKey(Key::Up);
	}
	if (kDown & KEY_DDOWN) {
	  state.setKey(Key::Down);
	}
	if (kDown & KEY_DLEFT) {
	  state.setKey(Key::Left);
	}
	if (kDown & KEY_DRIGHT) {
	  state.setKey(Key::Right);
	}
	if (kDown & KEY_A) {
	  state.setKey(Key::OK);
	}
	if (kDown & KEY_B) {
	  state.setKey(Key::Back);
	}
	if (kDown & KEY_Y) {
	  state.setKey(Key::Shift);
	}
	if (kDown & KEY_X) {
	  state.setKey(Key::Alpha);
	}
	if (kDown & KEY_START) {
	  state.setKey(Key::Home);
	}
	if (kDown & KEY_SELECT) {
	  state.setKey(Key::OnOff);
	}
	
  Simulator::Main::refresh();
  
  return state;
}

}
}
