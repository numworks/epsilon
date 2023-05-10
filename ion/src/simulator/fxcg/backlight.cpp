#include <ion/backlight.h>

#include <gint/drivers/r61524.h>
#include <gint/defs/util.h>

// From gint:
/* Interface with the controller */
static volatile uint16_t *intf = (uint16_t *)0xb4000000;
/* Bit 4 of Port R controls the RS bit of the display driver */
static volatile uint8_t *PRDR = (uint8_t *)0xa405013c;

GINLINE static void select(uint16_t reg)
{
	/* Clear RS and write the register number */
	*PRDR &= ~0x10;
	synco();
	*intf = reg;
	synco();

	/* Set RS back. We don't do this in read()/write() because the display
	   driver is optimized for consecutive GRAM access. LCD-transfers will
	   be faster when executing select() followed by several calls to
	   write(). (Although most applications should use the DMA instead.) */
	*PRDR |= 0x10;
	synco();
}

// From Utilities addin:
// START OF POWER MANAGEMENT CODE
#define LCDC *(unsigned int*)(0xB4000000)
int getRawBacklightSubLevel()
{
  // Bdisp_DDRegisterSelect(0x5a1);
  select(0x5a1);
  return (LCDC & 0xFF) - 6;
}
void setRawBacklightSubLevel(int level)
{
  // Bdisp_DDRegisterSelect(0x5a1);
  select(0x5a1);
  LCDC = (level & 0xFF) + 6;
}
// END OF POWER MANAGEMENT CODE 

namespace Ion {
namespace Backlight {

uint8_t brightness() {
  return getRawBacklightSubLevel();
}

void setBrightness(uint8_t b) {
  setRawBacklightSubLevel(b);
}

void init() {
}

bool isInitialized() {
  return true;
}

void shutdown() {
}

}
}
