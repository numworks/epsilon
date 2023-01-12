CC = emcc
CXX = emcc
LD = emcc
CPP = cpp

EMSCRIPTEN_ASYNC_SYMBOLS = \
SAFE_HEAP_LOAD \
SAFE_HEAP_STORE \
_IonEventsEmscriptenKeyDown \
_IonEventsEmscriptenKeyUp \
_IonEventsEmscriptenPushEvent \
_IonSimulatorCallbackDidScanKeyboard \
__Z8ion_mainiPKPKc \
__ZN6Escher10Invocation7performEPv \
__ZN11MicroPython20ExecutionEnvironment7runCodeEPKc \
__ZN13AppsContainer13dispatchEventEN3Ion6Events5EventE \
__ZN13AppsContainer3runEv \
__ZN13AppsContainer18switchToBuiltinAppEPN6Escher3App8SnapshotE \
__ZN6Escher19ButtonRowController23didBecomeFirstResponderEv \
__ZN6Escher19ModalViewController23didBecomeFirstResponderEv \
__ZN6Escher19StackViewController23didBecomeFirstResponderEv \
__ZN6Escher19StackViewController25setupActiveViewControllerEv \
__ZN6Escher19StackViewController4pushEPNS_14ViewControllerE \
__ZN6Escher3App12processEventEN3Ion6Events5EventE \
__ZN6Escher3App15didBecomeActiveEPNS_6WindowE \
__ZN6Escher3App17setFirstResponderEPNS_9ResponderE \
__ZN3Ion6Events13isAlphaActiveEv \
__ZN3Ion6Events13isShiftActiveEv \
__ZN3Ion6Events5EventC2Ei \
__ZN3Ion6Events5EventC2ENS_8Keyboard3KeyEbb \
__ZN3Ion6Events8getEventEPi \
__ZN3Ion6Events14sharedGetEventEPi \
__ZN3Ion6Events16popKeyboardStateEv \
__ZN3Ion6Events21privateSharedGetEventEPi \
__ZN3Ion6Events24waitForInterruptingEventEiPi \
__ZN3Ion8Keyboard8popStateEv \
__ZN3Ion6EventsL16sleepWithTimeoutEiPi \
__ZN3Ion6Timing6msleepEj \
__ZN3Ion8Keyboard4scanEv \
__ZN3Ion8Keyboard30scanForInterruptionAndPopStateEv \
__ZN3Ion9Simulator8Keyboard7didScanEv \
__ZN4Code14MenuController21openConsoleWithScriptENS_6ScriptE \
__ZN4Code14MenuController23didBecomeFirstResponderEv \
__ZN4Code14MenuController28openConsoleWithScriptAtIndexEi \
__ZN4Code17ConsoleController10autoImportEv \
__ZN4Code17ConsoleController14viewWillAppearEv \
__ZN4Code17ConsoleController16autoImportScriptENS_6ScriptEb \
__ZN4Code17ConsoleController21runAndPrintForCommandEPKc \
__ZN4Code17ConsoleController23autoImportScriptAtIndexEib \
__ZN4Code17ConsoleController25textFieldDidFinishEditingEPN6Escher17AbstractTextFieldEPKcN3Ion6Events5EventE \
__ZN4Code17ConsoleController9inputTextEPKc \
__ZN4Code25ScriptParameterController11handleEventEN3Ion6Events5EventE \
__ZN5QueueIN3Ion6Events5EventELi1024EE4nextEPS2_ \
__ZN5QueueIN3Ion6Events5EventELi1024EE4sizeEv \
__ZN5QueueIN3Ion6Events5EventELi1024EE7enqueueES2_ \
__ZN6Turtle10setHeadingEd \
__ZN6Turtle10setVisibleEb \
__ZN6Turtle3dotEdd \
__ZN6Turtle4drawEb \
__ZN6Turtle4goToEdd \
__ZN6Turtle4leftEd \
__ZN6Turtle5resetEv \
__ZN6Turtle5rightEd \
__ZN6Turtle6circleEld \
__ZN6Turtle7forwardEd \
__ZN6Turtle8backwardEd \
__ZN6Escher19StackViewController15setupActiveViewEv \
__ZN6Escher7RunLoop3runEv \
__ZN6Escher7RunLoop4stepEv \
__ZN6Escher7RunLoop8runWhileEPFbPvES1_ \
__ZN6Escher9Container13dispatchEventEN3Ion6Events5EventE \
__ZN6Escher9Container3runEv \
__ZN6Escher9Container18switchToBuiltinAppEPNS_3App8SnapshotE \
__ZN6Escher17AbstractTextField11handleEventEN3Ion6Events5EventE \
__ZN6Escher17AbstractTextField30privateHandleEventWhileEditingEN3Ion6Events5EventE \
__ZThn36_N4Code17ConsoleController25textFieldDidFinishEditingEPN6Escher17AbstractTextFieldEPKcN3Ion6Events5EventE \
__ZThn32_N4Code17ConsoleController9inputTextEPKc \
__ZThn36_N4Code17ConsoleController9inputTextEPKc \
__ZZN4Code14MenuControllerC1EP6Escher9ResponderPNS_11ScriptStoreEP6Escher19ButtonRowControllerbEN3__08__invokeEPvS8_ \
__ZZN4Code14MenuControllerC1EP6Escher9ResponderPNS_11ScriptStoreEP6Escher19ButtonRowControllerbENK3__0clEPvS8_ \
_closure_call \
_do_load \
_do_load_from_lexer \
_emscripten_sleep \
_emscripten_sleep_with_yield \
_fun_bc_call \
_fun_builtin_1_call \
_fun_builtin_var_call \
_main \
_micropython_port_interruptible_msleep \
_micropython_port_interrupt_if_needed \
_micropython_port_vm_hook_loop \
_modion_keyboard_keydown \
_modtime_sleep \
_modturtle_backward \
_modturtle_circle \
_modturtle_forward \
_modturtle_goto \
_modturtle_left \
_modturtle_reset \
_modturtle_right \
_modturtle_setheading \
_modturtle_showturtle \
_mp_builtin___import__ \
_mp_builtin_input \
_mp_call_function_0 \
_mp_call_function_n_kw \
_mp_execute_bytecode \
_mp_hal_input \
_mp_import_name \
_mp_parse_compile_execute \
_get_clipboard_text \
_set_clipboard_text \
__ZN3Ion9Clipboard24fetchFromSystemClipboardEPcm \
__ZN3Ion9Clipboard21sendToSystemClipboardEPKc \
__ZN3Ion9Clipboard4readEPcm \
__ZN3Ion9Clipboard5writeEPKc \
__ZN6Escher9Clipboard5storeEPKci \
__ZN6Escher9Clipboard10storedTextEv \
__ZN6Escher11LayoutField18privateHandleEventEN3Ion6Events5EventE \
__ZN6Escher11LayoutField11handleEventEN3Ion6Events5EventE \
__ZN6Escher8TextArea18privateHandleEventEN3Ion6Events5EventE \
__ZN6Escher8TextArea11handleEventEN3Ion6Events5EventE \
__ZN6Escher15ExpressionField11handleEventEN3Ion6Events5EventE \
__ZN3Ion14CircuitBreaker14loadCheckpointENS0_14CheckpointTypeE \
_msleep \
_SDL_Delay \
_SDL_RenderPresent \
_SDL_UpdateWindowSurface \
_SDL_UpdateWindowSurfaceRects \
_Emscripten_UpdateWindowFramebuffer \
__ZN3Ion9Simulator6Window7refreshEv

EMTERPRETIFY_WHITELIST = $(foreach sym,$(EMSCRIPTEN_ASYNC_SYMBOLS),"$(sym)",)END
EMFLAGS = -s PRECISE_F32=1 -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY_WHITELIST='[$(EMTERPRETIFY_WHITELIST:,END=)]'
EMFLAGS += -Wno-emterpreter # We know Emterpreter is deprecated...

ifeq ($(DEBUG),1)
EMFLAGS += --profiling-funcs
EMFLAGS += -s ASSERTIONS=1
EMFLAGS += -s SAFE_HEAP=1
EMFLAGS += -s STACK_OVERFLOW_CHECK=1
endif

# Configure EMFLAGS
EMFLAGS += -s WASM=0

# Configure LDFLAGS
EMSCRIPTEN_MODULARIZE ?= 1
LDFLAGS += -s MODULARIZE=$(EMSCRIPTEN_MODULARIZE) -s 'EXPORT_NAME="Epsilon"' --memory-init-file 0

SFLAGS += $(EMFLAGS)
LDFLAGS += $(EMFLAGS) -Oz -s EXPORTED_FUNCTIONS='["_main", "_IonSimulatorKeyboardKeyDown", "_IonSimulatorKeyboardKeyUp", "_IonSimulatorEventsPushEvent", "_IonSoftwareVersion", "_IonPatchLevel"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["UTF8ToString"]'
