CC = emcc
CXX = emcc
LD = emcc

EMSCRIPTEN_ASYNC_SYMBOLS = \
_main \
__ZN3Ion6Events8getEventEPi \
__ZN4Code17ConsoleController9inputTextEPKc \
__ZThn32_N4Code17ConsoleController9inputTextEPKc \
_mp_hal_input \
_mp_builtin_input \
_fun_builtin_var_call \
_mp_call_function_n_kw \
__ZN11MicroPython20ExecutionEnvironment7runCodeEPKc \
__ZN4Code17ConsoleController21runAndPrintForCommandEPKc \
__ZN4Code17ConsoleController25textFieldDidFinishEditingEP9TextFieldPKcN3Ion6Events5EventE \
__ZThn28_N4Code17ConsoleController25textFieldDidFinishEditingEP9TextFieldPKcN3Ion6Events5EventE \
__ZN9TextField18privateHandleEventEN3Ion6Events5EventE \
__ZN9TextField11handleEventEN3Ion6Events5EventE \
__ZN3App12processEventEN3Ion6Events5EventE \
__ZN9Container13dispatchEventEN3Ion6Events5EventE \
__ZN13AppsContainer13dispatchEventEN3Ion6Events5EventE \
__ZN7RunLoop4stepEv \
__ZN7RunLoop8runWhileEPFbPvES0_ \
__ZN7RunLoop3runEv \
__ZN9Container3runEv \
__ZN13AppsContainer3runEv \
__Z8ion_mainiPPc \
_mp_execute_bytecode \
_fun_bc_call \
_mp_call_function_0 \
_IonEventsEmscriptenPushEvent \
_IonEventsEmscriptenPushKey \
__ZN3Ion6Events13isShiftActiveEv \
__ZN3Ion6Events13isAlphaActiveEv \
__ZN3Ion6Events5EventC2ENS_8Keyboard3KeyEbb \
_mp_parse_compile_execute \
_do_load_from_lexer \
_do_load \
_mp_builtin___import__ \
_mp_import_name \
__ZN4Code25ScriptParameterController11handleEventEN3Ion6Events5EventE \
__ZN4Code14MenuController28openConsoleWithScriptAtIndexEi \
__ZN3Ion6EventsL16sleepWithTimeoutEiPi \
__ZN5QueueIN3Ion6Events5EventELi1024EE7enqueueES2_ \
__ZN5QueueIN3Ion6Events5EventELi1024EE4sizeEv \
__ZN5QueueIN3Ion6Events5EventELi1024EE4nextEPS2_ \
__ZN4Code17ConsoleController23autoImportScriptAtIndexEib

EMFLAGS = -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s EMTERPRETIFY_WHITELIST='[$(foreach sym,$(EMSCRIPTEN_ASYNC_SYMBOLS),"$(sym)",)]'

ifeq ($(DEBUG),1)
EMFLAGS += --profiling-funcs -s ASSERTIONS=1
endif

EMFLAGS += -s MODULARIZE=1 -s 'EXPORT_NAME="Epsilon"'

SFLAGS += $(EMFLAGS)
LDFLAGS += $(EMFLAGS) -Oz -s EXPORTED_FUNCTIONS='["_main", "_IonEventsEmscriptenPushKey", "_IonEventsEmscriptenPushEvent"]'
