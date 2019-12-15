#ifdef SIMULATOR
#include <FL/Fl.H>
#endif
#include <unistd.h>
#include "port.h"

#include <ion.h>
#include <kandinsky.h>
#include <poincare/preferences.h>
#include "../../apps/apps_container.h"
#include "../../apps/global_preferences.h"

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>

extern "C" {
#include "py/builtin.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/lexer.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/repl.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "mphalport.h"
#include "mod/turtle/modturtle.h"
}

static MicroPython::ScriptProvider * sScriptProvider = nullptr;
static MicroPython::ExecutionEnvironment * sCurrentExecutionEnvironment = nullptr;

MicroPython::ExecutionEnvironment * MicroPython::ExecutionEnvironment::currentExecutionEnvironment() {
  return sCurrentExecutionEnvironment;
}

void MicroPython::ExecutionEnvironment::runCode(const char * str) {
  assert(sCurrentExecutionEnvironment == nullptr);
  sCurrentExecutionEnvironment = this;

  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(0, str, strlen(str), false);
    mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
    mp_obj_t module_fun = mp_compile(&pt, lex->source_name, MP_EMIT_OPT_NONE, true);
    mp_hal_set_interrupt_char((int)Ion::Keyboard::Key::Back);
    mp_call_function_0(module_fun);
    mp_hal_set_interrupt_char(-1); // Disable interrupt
    nlr_pop();
  } else { // Uncaught exception
    /* mp_obj_print_exception is supposed to handle error printing. However,
     * because we want to print custom information, we copied and modified the
     * content of mp_obj_print_exception instead of calling it. */
    if (mp_obj_is_exception_instance((mp_obj_t)nlr.ret_val)) {
        size_t n, *values;
        mp_obj_exception_get_traceback((mp_obj_t)nlr.ret_val, &n, &values);
        if (n > 0) {
            assert(n % 3 == 0);
            for (int i = n - 3; i >= 0; i -= 3) {
              if (values[i] != 0 || i == 0) {
                if (values[i] == 0) {
                  mp_printf(&mp_plat_print, "  Last command\n");
                } else {
#if MICROPY_ENABLE_SOURCE_LINE
                  mp_printf(&mp_plat_print, "  File \"%q\", line %d", values[i], (int)values[i + 1]);
#else
                  mp_printf(&mp_plat_print, "  File \"%q\"", values[i]);
#endif
                  // the block name can be NULL if it's unknown
                  qstr block = values[i + 2];
                  if (block == MP_QSTR_NULL) {
                    mp_print_str(&mp_plat_print, "\n");
                  } else {
                    mp_printf(&mp_plat_print, ", in %q\n", block);
                  }
                }
              }
            }
        }
    }
    mp_obj_print_helper(&mp_plat_print, (mp_obj_t)nlr.ret_val, PRINT_EXC);
    mp_print_str(&mp_plat_print, "\n");
    /* End of mp_obj_print_exception. */
  }

    assert(sCurrentExecutionEnvironment == this);
    sCurrentExecutionEnvironment = nullptr;
  }

void MicroPython::ExecutionEnvironment::interrupt() {
  mp_keyboard_interrupt();
}

void MicroPython::ExecutionEnvironment::setSandboxIsDisplayed(bool display) {
  if (m_sandboxIsDisplayed && !display) {
    modturtle_view_did_disappear();
  }
  m_sandboxIsDisplayed = display;
}

extern "C" {
  extern const void * _stack_start;
  extern const void * _stack_end;
}

void MicroPython::init(void * heapStart, void * heapEnd) {
#if MP_PORT_USE_STACK_SYMBOLS
  mp_stack_set_top(&_stack_start);
  mp_stack_set_limit(&_stack_start - &_stack_end);
#else
  volatile int stackTop;
  mp_stack_set_top((void *)(&stackTop));
  mp_stack_set_limit(8192);
#endif
  gc_init(heapStart, heapEnd);
  mp_init();
}

void MicroPython::deinit() {
  mp_deinit();
}

void MicroPython::registerScriptProvider(ScriptProvider * s) {
  sScriptProvider = s;
}

void gc_collect(void) {
  void * python_stack_top = MP_STATE_THREAD(stack_top);
  assert(python_stack_top != NULL);

  gc_collect_start();

  modturtle_gc_collect();

  /* get the registers.
   * regs is the also the last object on the stack so the stack is bound by
   * &regs and python_stack_top. */
  jmp_buf regs;
  setjmp(regs);

  void **regs_ptr = (void**)&regs;

  /* On the device, the stack is stored in reverse order, but it might not be
   * the case on a computer. We thus have to take the absolute value of the
   * addresses difference. */
  size_t stackLengthInByte;
  void ** scanStart;
  if ((uintptr_t)python_stack_top > (uintptr_t)regs_ptr) {

    /* To compute the stack length:
     *                                  regs
     *                             <----------->
     * STACK ->  ...|  |  |  |  |  |--|--|--|--|  |  |  |  |  |  |
     *                             ^&regs                        ^python_stack_top
     * */

    stackLengthInByte = (uintptr_t)python_stack_top - (uintptr_t)regs_ptr;
    scanStart = regs_ptr;

  } else {

    /* When computing the stack length, take into account regs' size.
     *                                                 regs
     *                                            <----------->
     * STACK ->  |  |  |  |  |  |  |  |  |  |  |  |--|--|--|--|  |  |  |...
     *           ^python_stack_top                ^&regs
     * */

    stackLengthInByte = (uintptr_t)regs_ptr - (uintptr_t)python_stack_top + sizeof(regs);
    scanStart = (void **)python_stack_top;

  }
  /* Memory error detectors might find an error here as they might split regs
   * and stack memory zones. */
  gc_collect_root(scanStart, stackLengthInByte/sizeof(void *));

  gc_collect_end();
}

void nlr_jump_fail(void *val) {
    while (1);
}

mp_lexer_t * mp_lexer_new_from_file(const char * filename) {
  if (sScriptProvider != nullptr) {
    const char * script = sScriptProvider->contentOfScript(filename);
    if (script != nullptr) {
      return mp_lexer_new_from_str_len(qstr_from_str(filename), script, strlen(script), 0 /* size_t free_len*/);
    } else {
      mp_raise_OSError(MP_ENOENT);
    }
  } else {
    mp_raise_OSError(MP_ENOENT);
  }
}

mp_import_stat_t mp_import_stat(const char *path) {
  if (sScriptProvider && sScriptProvider->contentOfScript(path)) {
    return MP_IMPORT_STAT_FILE;
  }
  return MP_IMPORT_STAT_NO_EXIST;
}

void mp_hal_stdout_tx_strn_cooked(const char * str, size_t len) {
  if (sCurrentExecutionEnvironment != nullptr)
    sCurrentExecutionEnvironment->printText(str, len);
}

const char * mp_hal_input(const char * prompt) {
  if (sCurrentExecutionEnvironment != nullptr)
    return sCurrentExecutionEnvironment->inputText(prompt);
  return 0;
}

/* C-- SDK , (c) B. Parisse 2019 */

const char * read_file(const char * filename){
#if 1
  Ion::Storage * s=Ion::Storage::sharedStorage();
  const Ion::Storage::Record r=s->recordNamed(filename);
  if (r.isNull())
    return 0;
  Ion::Storage::Record::Data d=r.value();
  const char * ptr=(const char *)d.buffer;
  if (ptr)
    return ptr+1;
  else
    return 0;
#endif
  if (sScriptProvider != nullptr)
    return sScriptProvider->contentOfScript(filename);
  return "undef";
}

bool write_file(const char * filename,const char * content,size_t len){
  Ion::Storage * s=Ion::Storage::sharedStorage();
  auto res=s->createRecordWithFullName(filename,content,strlen(content)+1);
  if (res==Ion::Storage::Record::ErrorStatus::NameTaken){
    auto r=s->recordNamed(filename);
    Ion::Storage::Record::Data d;
    d.buffer=content;
    d.size=(len?len:strlen(content))+1;
    return r.setValue(d)==Ion::Storage::Record::ErrorStatus::None;
  }
  if (res==Ion::Storage::Record::ErrorStatus::None)
    return write_file(filename,content,len);
  return false;
}

bool file_exists(const char * filename){
  Ion::Storage * s=Ion::Storage::sharedStorage();
  return s->isFullNameTaken(filename);
}

bool erase_file(const char * filename){
  Ion::Storage * s=Ion::Storage::sharedStorage();
  auto r= s->recordNamed(filename);
  if (r.isNull())
    return false;
  r.destroy();
  return true;
}


#if 1
int os_file_browser(const char ** filenames,int maxrecords,const char * extension){
  Ion::Storage * s=Ion::Storage::sharedStorage();
  int n=s->numberOfRecordsWithExtension(extension);
  if (!n) return 0;
  if (n>maxrecords) n=maxrecords;
  for (int i=0;i<n;i++){
    const Ion::Storage::Record & r=s->recordWithExtensionAtIndex(extension, i);
    filenames[i]=r.fullName();
  }
  filenames[n]=0;
  return n;
}
  // const char * ptr=(const char *)r.value().buffer;
  /*     storage.h
	 Ion::Storage::Record::Data structure avec 2 membres
	 const void * buffer et size_t size

	 Record(const char * fullName = nullptr);
	 Record(const char * basename, const char * extension);
	 Data value();
	 Ion::Storage::Record::ErrorStatus setValue(Ion::Storage::Record::Data);
	 void destroy(void);

  // Record creation
  Record::ErrorStatus createRecordWithFullName(const char * fullName, const void * data, size_t size);
  Record::ErrorStatus createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size);

  // Record getters
  Record recordWithExtensionAtIndex(const char * extension, int index);
  Record recordNamed(const char * fullName);
  Record recordBaseNamedWithExtension(const char * baseName, const char * extension);
  Record recordBaseNamedWithExtensions(const char * baseName, const char * const extension[], size_t numberOfExtensions);

  // Record destruction
  void destroyAllRecords();
  void destroyRecordWithBaseNameAndExtension(const char * baseName, const char * extension);
  void destroyRecordsWithExtension(const char * extension);


  */
#endif

void statuslinemsg(const char * msg){
  AppsContainer::sharedAppsContainer()->reloadTitleBarView();
  auto ptr=MicroPython::ExecutionEnvironment::currentExecutionEnvironment();
  if (ptr)
    ptr->displaySandbox();
  auto ctx=KDIonContext::sharedContext();
  KDRect save=ctx->m_clippingRect;
  KDPoint o=ctx->m_origin;
  ctx->setClippingRect(KDRect(0,0,280,18));
  ctx->setOrigin(KDPoint(0,0));
  KDRect rect(0,0,280,18);
  KDIonContext::sharedContext()->pushRectUniform(rect,64934 /* Palette::YellowDark*/);
  if (strlen(msg)>25)
    ctx->drawString(msg, KDPoint(0,0), KDFont::SmallFont, 0, 64934);
  else
    ctx->drawString(msg, KDPoint(0,0), KDFont::LargeFont, 0, 64934);
  ctx->setClippingRect(save);
  ctx->setOrigin(o);
}

bool os_set_angle_unit(int mode){
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
 if (mode==0){ // rad
    preferences->setAngleUnit(Poincare::Preferences::AngleUnit::Radian);
    return true;
  }
  if (mode==1){ // deg
    preferences->setAngleUnit(Poincare::Preferences::AngleUnit::Degree);
    return true;
  }
  return false;
  if (mode==2){ // grad
    return true;
  }
}

int os_get_angle_unit(){
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  if (preferences->angleUnit()==Poincare::Preferences::AngleUnit::Radian)
    return 0;
  if (preferences->angleUnit()==Poincare::Preferences::AngleUnit::Degree)
    return 1;
  return 2;
}

#ifdef SIMULATOR
#define TICKS_PER_MINUTE 60000
#else
#define TICKS_PER_MINUTE 11862
extern const void * _stack_start;
extern const void * _heap_start;
#endif
//int time_shift=0; // set it via time() command in KhiCAS
void statusline(int mode, size_t heap){
  AppsContainer::sharedAppsContainer()->reloadTitleBarView();
  auto ctx=KDIonContext::sharedContext();
  KDRect save=ctx->m_clippingRect;
  KDPoint o=ctx->m_origin;
  ctx->setClippingRect(KDRect(0,0,320,18));
  ctx->setOrigin(KDPoint(0,0));
  KDRect rect(0,0,mode==1?320:280,18);
  KDIonContext::sharedContext()->pushRectUniform(rect,64934 /* Palette::YellowDark*/);
  const char * text=0;
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  if (preferences->angleUnit() == Poincare::Preferences::AngleUnit::Radian)
    text="rad";
  else
    text="deg";
  // ctx->drawString(text, KDPoint(5,1), KDFont::SmallFont, 0, 63488 /* Palette::Red*/);
  ctx->drawString(text, KDPoint(5,1), KDFont::SmallFont, 0, 64934);
#ifdef GIAC_LINKED
  if (khicas_eval)
    text="KHICAS";
  else
    text="PYTHON";
#else
  text="KHICAS";
#endif
  ctx->drawString(text, KDPoint(70,1), KDFont::SmallFont, 0, 64934);
  char bufheap[16];
#if 0 //ndef SIMULATOR
#endif
  int x;
  x=(heap&0xf0000000)>>28;
  bufheap[0]=(x>9?'a'+(x-10):'0'+x);
  x=(heap&0xf000000)>>24;
  bufheap[1]=(x>9?'a'+(x-10):'0'+x);
  x=(heap&0xf00000)>>20;
  bufheap[2]=(x>9?'a'+(x-10):'0'+x);
  x=(heap&0xf0000)>>16;
  bufheap[3]=(x>9?'a'+(x-10):'0'+x);
  x=(heap&0xf000)>>12;
  bufheap[4]=(x>9?'a'+(x-10):'0'+x);
  x=(heap&0xf00)>>8;
  bufheap[5]=(x>9?'a'+(x-10):'0'+x);
  x=(heap&0xf0)>>4;
  bufheap[6]=(x>9?'a'+(x-10):'0'+x);
  x=(heap&0xf);
  bufheap[7]=(x>9?'a'+(x-10):'0'+x);
  bufheap[8]=0;
  if(heap != 0) ctx->drawString(bufheap,KDPoint(130,1),KDFont::SmallFont, 0, 64934);
#ifdef GIAC_SHOWTIME
  int d=(Ion::Timing::millis()/TICKS_PER_MINUTE +time_shift) % (24*60); // minutes
  char buf[32]={0,0,0,0};
  int h=d/60;
  buf[0]='0'+h/10;
  buf[1]='0'+(h%10);
  buf[2]=':';
  ctx->drawString(buf, KDPoint(148,1), KDFont::SmallFont, 0, 64934);
  int mn=d%60;
  buf[0]='0'+mn/10;
  buf[1]='0'+(mn%10);
  buf[2]=0;
  ctx->drawString(buf, KDPoint(168,1), KDFont::SmallFont, 0, 64934);
#endif
  text="     ";
  if (Ion::Events::shiftAlphaStatus()==Ion::Events::ShiftAlphaStatus::Shift)
    text="shift ";
  else {
    if (Ion::Events::isAlphaActive()){
      if (Ion::Events::shiftAlphaStatus()==Ion::Events::ShiftAlphaStatus::AlphaLock)
	text="alphal";
      if (Ion::Events::shiftAlphaStatus()==Ion::Events::ShiftAlphaStatus::ShiftAlphaLock)
	text="ALPHAL";
      if (Ion::Events::shiftAlphaStatus()==Ion::Events::ShiftAlphaStatus::Alpha)
	text="1alpha";
      if (Ion::Events::shiftAlphaStatus()==Ion::Events::ShiftAlphaStatus::ShiftAlpha)
	text="1ALPHA";
    }
  }
  ctx->drawString(text, KDPoint(232,1), KDFont::SmallFont, 0, 64934 /* Palette::YellowDark*/);
  if (mode==1){
    if (Ion::USB::isPlugged())
      text="charge";
    else {
      auto c=Ion::Battery::level();
      if (c==Ion::Battery::Charge::EMPTY)
	text="empty ";
      if (c==Ion::Battery::Charge::LOW)
	text="low   ";
      if (c==Ion::Battery::Charge::FULL)
	text="full  ";
    }
    ctx->drawString(text, KDPoint(280,1), KDFont::SmallFont, 0, 64934 /* Palette::YellowDark*/);
  }
  ctx->setClippingRect(save);
  ctx->setOrigin(o);
}

bool isalphaactive(){
  return Ion::Events::isAlphaActive();
}

void lock_alpha(){
  Ion::Events::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::AlphaLock);
  statusline(0,0);
}

void reset_kbd(){
  Ion::Events::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  statusline(0,0);
}

bool alphawasactive_=false;
bool alphawasactive(){
  return alphawasactive_;
}

bool waitforvblank(){
  return Ion::Display::waitForVBlank();
}

bool back_key_pressed() {
  static int c = 0;

  ++c ;
  if (c<400 || (c & 0xf)!= 0) {
    return false;
  }
#ifdef SIMULATOR
  Fl::wait(0.00001);
#endif

  Ion::Keyboard::State scan = Ion::Keyboard::scan();
  // if (scan!=16) std::cerr << scan << '\n';
  Ion::Keyboard::Key interruptKey = static_cast<Ion::Keyboard::Key>(mp_interrupt_char);
  if (scan.keyDown(interruptKey)
      // || scan.keyDown(static_cast<Ion::Keyboard::Key>(16))
      )
    return true;
  return false;
}

int getkey_raw(bool allow_suspend){
  int key=-1;
  size_t t1=Ion::Timing::millis();
  for (;;){
    int timeout=10000;
    alphawasactive_=Ion::Events::isAlphaActive();
    Ion::Events::Event event=Ion::Events::getEvent(&timeout);
    auto ctx=KDIonContext::sharedContext();
    KDRect save=ctx->m_clippingRect;
    KDPoint o=ctx->m_origin;
    ctx->setClippingRect(KDRect(0,0,320,240));
    ctx->setOrigin(KDPoint(0,18));
    KDRect rect(90,63,140,75);
    if (event==Ion::Events::None){
      size_t t2=Ion::Timing::millis();
      if (t2-t1>2*TICKS_PER_MINUTE){
	// KDIonContext::sharedContext()->pushRectUniform(rect,64934 /* Palette::YellowDark*/);
	event=Ion::Events::OnOff;
      }
    }
    else
      t1=Ion::Timing::millis();
    if (event == Ion::Events::USBPlug) {
      statusline(0,0);
      // KDIonContext::sharedContext()->pushRectUniform(rect,33333);
      if (Ion::USB::isPlugged()) {
	if (GlobalPreferences::sharedGlobalPreferences()->examMode()) {
	  Ion::LED::setColor(KDColorBlack);
	  Ion::LED::updateColorWithPlugAndCharge();
	  GlobalPreferences::sharedGlobalPreferences()->setExamMode(false);
	  // displayExamModePopUp(false);
	} else {
	  Ion::USB::enable();
	}
	Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel());
      } else {
	Ion::USB::disable();
      }
    }
    if (event == Ion::Events::USBEnumeration || event == Ion::Events::USBPlug || event == Ion::Events::BatteryCharging) {
      Ion::LED::updateColorWithPlugAndCharge();
    }
    if (event == Ion::Events::USBEnumeration
	) {
      KDIonContext::sharedContext()->pushRectUniform(rect,64934 /* Palette::YellowDark*/);
      if (Ion::USB::isPlugged()) {
	/* Just after a software update, the battery timer does not have time to
	 * fire before the calculator enters DFU mode. As the DFU mode blocks the
	 * event loop, we update the battery state "manually" here.
	 * We do it before switching to USB application to redraw the battery
	 * pictogram. */
	// updateBatteryState();
	KDIonContext::sharedContext()->pushRectUniform(rect,22222);
	auto ctx=KDIonContext::sharedContext();
	int y=58;
	ctx->drawString("Connecte ! ", KDPoint(100,y), KDFont::LargeFont, 65535, 0);
	y+=18;
	ctx->drawString(" DFU mode  ", KDPoint(100,y), KDFont::LargeFont, 65535, 0);
	y+=18;
	ctx->drawString("Back quitte", KDPoint(100,y), KDFont::LargeFont, 65535, 0);
	y-=18;
	Ion::USB::DFU();
	KDIonContext::sharedContext()->pushRectUniform(rect,44444);
	ctx->drawString("Deconnecte!", KDPoint(100,y), KDFont::LargeFont, 65535, 0);
	// Update LED when exiting DFU mode
	Ion::LED::updateColorWithPlugAndCharge();
      } else {
	/* Sometimes, the device gets an ENUMDNE interrupts when being unplugged
	 * from a non-USB communicating host (e.g. a USB charger). The interrupt
	 * must me cleared: if not the next enumeration attempts will not be
	 * detected. */
	Ion::USB::clearEnumerationInterrupt();
      }
    }
    if (event.isKeyboardEvent()) {
      // m_backlightDimmingTimer.reset();
      // m_suspendTimer.reset();
      Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel());
    }
    ctx->setClippingRect(save);
    ctx->setOrigin(o);

    if (event==Ion::Events::Shift || event==Ion::Events::Alpha){
      statusline(0,0);
      continue;
    }
    if (event.isKeyboardEvent()){
      key=event.id();
      if (key==17 || key==4 || key==5 || key==52)
	reset_kbd();
      if (allow_suspend && (key==7 || key==8) ){ // power
	Ion::Power::suspend(true);
	numworks_fill_rect(0,0,320,240,65535);
	Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel());
	AppsContainer::sharedAppsContainer()->reloadTitleBarView();
	//AppsContainer::sharedAppsContainer()->redrawWindow();
	statusline(1,0);
	//continue;
      }
      else
	statusline(0,0);
      break;
    }
  }
  return key;
}

const short int translated_keys[]=
  {
   // non shifted
   KEY_CTRL_LEFT,KEY_CTRL_UP,KEY_CTRL_DOWN,KEY_CTRL_RIGHT,KEY_CTRL_OK,KEY_CTRL_EXIT,
   KEY_CTRL_MENU,KEY_PRGM_ACON,KEY_PRGM_ACON,9,10,11,
   KEY_CTRL_SHIFT,KEY_CTRL_ALPHA,KEY_CTRL_XTT,KEY_CTRL_VARS,KEY_CTRL_CATALOG,KEY_CTRL_DEL,
   KEY_CHAR_EXPN,KEY_CHAR_LN,KEY_CHAR_LOG,KEY_CHAR_IMGNRY,',',KEY_CHAR_POW,
   KEY_CHAR_SIN,KEY_CHAR_COS,KEY_CHAR_TAN,KEY_CHAR_PI,KEY_CHAR_ROOT,KEY_CHAR_SQUARE,
   '7','8','9','(',')',-1,
   '4','5','6','*','/',-1,
   '1','2','3','+','-',-1,
   '0','.',KEY_CHAR_EXPN10,KEY_CHAR_ANS,KEY_CTRL_EXE,-1,
   // shifted
   KEY_SHIFT_LEFT,KEY_CTRL_PAGEUP,KEY_CTRL_PAGEDOWN,KEY_SHIFT_RIGHT,KEY_CTRL_OK,KEY_CTRL_EXIT,
   KEY_CTRL_MENU,KEY_PRGM_ACON,KEY_PRGM_ACON,9,10,11,
   KEY_CTRL_SHIFT,KEY_CTRL_ALPHA,KEY_CTRL_CUT,KEY_CTRL_CLIP,KEY_CTRL_PASTE,KEY_CTRL_AC,
   KEY_CHAR_LBRCKT,KEY_CHAR_RBRCKT,KEY_CHAR_LBRACE,KEY_CHAR_RBRACE,'_',KEY_CHAR_STORE,
   KEY_CHAR_ASIN,KEY_CHAR_ACOS,KEY_CHAR_ATAN,'=','<','>',
   KEY_CTRL_F7,KEY_CTRL_F8,KEY_CTRL_F9,KEY_CTRL_F13,KEY_CTRL_F14,-1,
   KEY_CTRL_F4,KEY_CTRL_F5,KEY_CTRL_F6,KEY_CHAR_FACTOR,'%',-1,
   KEY_CTRL_F1,KEY_CTRL_F2,KEY_CTRL_F3,KEY_CHAR_NORMAL,'\\',-1,
   KEY_CTRL_F10,KEY_CTRL_F11,KEY_CTRL_F12,KEY_SHIFT_ANS,KEY_CTRL_EXE,-1,
   // alpha
   KEY_CTRL_LEFT,KEY_CTRL_UP,KEY_CTRL_DOWN,KEY_CTRL_RIGHT,KEY_CTRL_OK,KEY_CTRL_EXIT,
   KEY_CTRL_MENU,KEY_PRGM_ACON,KEY_PRGM_ACON,9,10,11,
   KEY_CTRL_SHIFT,KEY_CTRL_ALPHA,':',';','"',KEY_CTRL_DEL,
   'a','b','c','d','e','f',
   'g','h','i','j','k','l',
   'm','n','o','p','q',-1,
   'r','s','t','u','v',-1,
   'w','x','y','z',' ',-1,
   '?','!',KEY_CHAR_EXPN10,KEY_CHAR_ANS,KEY_CTRL_EXE,-1,
   // alpha shifted
   KEY_SHIFT_LEFT,KEY_CTRL_PAGEUP,KEY_CTRL_PAGEDOWN,KEY_SHIFT_RIGHT,KEY_CTRL_OK,KEY_CTRL_EXIT,
   KEY_CTRL_MENU,KEY_PRGM_ACON,KEY_PRGM_ACON,9,10,11,
   KEY_CTRL_SHIFT,KEY_CTRL_ALPHA,':',';','\'','%',
   'A','B','C','D','E','F',
   'G','H','I','J','K','L',
   'M','N','O','P','Q',-1,
   'R','S','T','U','V',-1,
   'W','X','Y','Z',' ',-1,
   '?','!',KEY_CHAR_EXPN10,KEY_CHAR_ANS,KEY_CTRL_EXE,-1,
  };

int getkey(bool allow_suspend){
  int k=getkey_raw(allow_suspend);
  // translate
  return translated_keys[k];
}

// Casio prototype
void GetKey(int * key){
  *key=getkey(true);
}


void numworks_wait_1ms(int ms){
  for (int i=0;i<ms/128;++i){
#ifdef SIMULATOR
    Fl::wait(0.00001);
#endif
    Ion::Keyboard::State scan = Ion::Keyboard::scan();
      // if (scan!=16) std::cerr << scan << '\n';
    Ion::Keyboard::Key interruptKey = static_cast<Ion::Keyboard::Key>(Ion::Keyboard::Key::Back);
    if (scan.keyDown(interruptKey))
      return;
    Ion::Timing::msleep(128);
  }
  Ion::Timing::msleep(ms % 128);
}

void enable_back_interrupt(){
  mp_interrupt_char = (int)Ion::Keyboard::Key::Back;
}

void disable_back_interrupt(){
  mp_interrupt_char = -1;
}

