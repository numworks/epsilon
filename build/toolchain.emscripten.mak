CC = emcc
CXX = emcc
LD = emcc

EMSCRIPTEN_ASYNC_SYMBOLS = \
SAFE_HEAP_LOAD \
SAFE_HEAP_STORE \
_IonEventsEmscriptenPushEvent \
_IonEventsEmscriptenPushKey \
__Z8ion_mainiPPc \
__ZN10Invocation7performEPv \
__ZN11MicroPython20ExecutionEnvironment7runCodeEPKc \
__ZN13AppsContainer13dispatchEventEN3Ion6Events5EventE \
__ZN13AppsContainer3runEv \
__ZN13AppsContainer8switchToEPN3App8SnapshotE \
__ZN19ButtonRowController23didBecomeFirstResponderEv \
__ZN19ModalViewController23didBecomeFirstResponderEv \
__ZN19StackViewController23didBecomeFirstResponderEv \
__ZN19StackViewController25setupActiveViewControllerEv \
__ZN19StackViewController4pushEP14ViewController7KDColorS2_S2_ \
__ZN3App12processEventEN3Ion6Events5EventE \
__ZN3App15didBecomeActiveEP6Window \
__ZN3App17setFirstResponderEP9Responder \
__ZN3Ion6Events13isAlphaActiveEv \
__ZN3Ion6Events13isShiftActiveEv \
__ZN3Ion6Events5EventC2Ei \
__ZN3Ion6Events5EventC2ENS_8Keyboard3KeyEbb \
__ZN3Ion6Events8getEventEPi \
__ZN3Ion6EventsL16sleepWithTimeoutEiPi \
__ZN4Code14MenuController21openConsoleWithScriptENS_6ScriptE \
__ZN4Code14MenuController23didBecomeFirstResponderEv \
__ZN4Code14MenuController28openConsoleWithScriptAtIndexEi \
__ZN4Code17ConsoleController10autoImportEv \
__ZN4Code17ConsoleController14viewWillAppearEv \
__ZN4Code17ConsoleController16autoImportScriptENS_6ScriptEb \
__ZN4Code17ConsoleController21runAndPrintForCommandEPKc \
__ZN4Code17ConsoleController23autoImportScriptAtIndexEib \
__ZN4Code17ConsoleController25textFieldDidFinishEditingEP9TextFieldPKcN3Ion6Events5EventE \
__ZN4Code17ConsoleController9inputTextEPKc \
__ZN4Code25ScriptParameterController11handleEventEN3Ion6Events5EventE \
__ZN5QueueIN3Ion6Events5EventELi1024EE4nextEPS2_ \
__ZN5QueueIN3Ion6Events5EventELi1024EE4sizeEv \
__ZN5QueueIN3Ion6Events5EventELi1024EE7enqueueES2_ \
__ZN6Button11handleEventEN3Ion6Events5EventE \
__ZN7RunLoop3runEv \
__ZN7RunLoop4stepEv \
__ZN7RunLoop8runWhileEPFbPvES0_ \
__ZN9Container13dispatchEventEN3Ion6Events5EventE \
__ZN9Container3runEv \
__ZN9Container8switchToEPN3App8SnapshotE \
__ZN9TextField11handleEventEN3Ion6Events5EventE \
__ZN9TextField18privateHandleEventEN3Ion6Events5EventE \
__ZThn28_N4Code17ConsoleController25textFieldDidFinishEditingEP9TextFieldPKcN3Ion6Events5EventE \
__ZThn28_N6Button11handleEventEN3Ion6Events5EventE \
__ZThn32_N4Code17ConsoleController9inputTextEPKc \
__ZThn36_N4Code17ConsoleController9inputTextEPKc \
__ZZN4Code14MenuControllerC1EP9ResponderPNS_11ScriptStoreEP19ButtonRowControllerbEN3__08__invokeEPvS8_ \
__ZZN4Code14MenuControllerC1EP9ResponderPNS_11ScriptStoreEP19ButtonRowControllerbENK3__0clEPvS8_ \
_do_load \
_do_load_from_lexer \
_fun_bc_call \
_fun_builtin_var_call \
_main \
_mp_builtin___import__ \
_mp_builtin_input \
_mp_call_function_0 \
_mp_call_function_n_kw \
_mp_execute_bytecode \
_mp_hal_input \
_mp_import_name \
_mp_parse_compile_execute

EMTERPRETIFY_WHITELIST = $(foreach sym,$(EMSCRIPTEN_ASYNC_SYMBOLS),"$(sym)",)END
EMFLAGS = -s PRECISE_F32=1 -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY_WHITELIST='[$(EMTERPRETIFY_WHITELIST:,END=)]'

ifeq ($(DEBUG),1)
EMFLAGS += --profiling-funcs
EMFLAGS += -s ASSERTIONS=1
EMFLAGS += -s SAFE_HEAP=1
endif

EMFLAGS += -s MODULARIZE=1 -s 'EXPORT_NAME="Epsilon"'

SFLAGS += $(EMFLAGS)
LDFLAGS += $(EMFLAGS) -Oz -s EXPORTED_FUNCTIONS='["_main", "_IonEventsEmscriptenPushKey", "_IonEventsEmscriptenPushEvent", "_IonSoftwareVersion", "_IonPatchLevel"]'
