#include "external_flash.h"
#include <ion/src/device/regs/rcc.h>
#include <ion/src/device/regs/gpio.h>
#include <ion/src/device/regs/quadspi.h>

namespace Ion {
namespace ExternalFlash {
namespace Device {

void waitController() {
  while (QUADSPI.SR()->getBUSY()) {
  }
}

static enum DeviceStatusRegister : uint16_t {
  // Status Register 1
    BUSY = 1 << 0, // Erase, Program, Write Status Register in Progress
    WEL  = 1 << 1, // Write Enable Latch
    BP0  = 1 << 2, // Block Protect
    BP1  = 1 << 3, // Block Protect
    BP2  = 1 << 4, // Block Protect
    TB   = 1 << 5, // Top/Bottom Write Protect
    SEC  = 1 << 6, // Sector Protect
    SRP  = 1 << 7, // Status Register Protect 0
  // Status Register 2
    SRP1 = 1 << 8, // Status Register Protect 1
    QE   = 1 << 9, // Quad-SPI Enable
    // Bits 10, 11, 12, 13 are reserved
    CMP  = 1 << 14, //
    SUS  = 1 << 15
};

/*
------------------------------------------------------------------------------------------------------------------------------------------------
			Bit	Volatile?	Reset value	Short name	Long name
------------------------------------------------------------------------------------------------------------------------------------------------
Status Register-1	 0	    volatile			BUSY		Erase, Program, Write Status Register in Progress

	Any instruction, except for Read Register, will be ignored while BUSY=1.
	It is recommended to check the BUSY bit before sending a new instruction, after a Program, Erase, Write Status Register operation.
	TODO Status polling instead of while...

			 1	    volatile			WEL		Write Enable Latch

	must be set before any Program, Erase or Write Status Register instruction, with the Write Enable instruction.
	is automatically reset write-disable status of “0” after Power‐up and upon completion of any Program, Erase or Write Status Register instruction.

			 2	non-volatile			BP0		Block Protect
			 3	non-volatile			BP1		Block Protect
			 4	non-volatile			BP2		Block Protect
			 5	non-volatile			TB		Top/Bottom Write Protect
			 6	non-volatile	0		SEC		Sector Protect

	controls if the Block Protect Bits (BP2, BP1, BP0) protect 4KB Sectors (SEC=1) or 64KB Blocks (SEC=0) in the Top (TB=0) or the Bottom (TB=1) of the array.

			 7	non-volatile			SRP		Status Register Protect 0
Status Register-2	 8	non-volatile			SRP1		Status Register Protect 1
			 9	non-volatile			QE		Quad SPI Enable
			14	non-volatile	0		CMP		Complement Protect

	CMP=0: a top 4KB sector can be protected while the rest of the array is not
	CMP=1: the top 4KB sector will become unprotected while the rest of the array become read-only.

			15	    volatile	0		SUS		Suspend Status
------------------------------------------------------------------------------------------------------------------------------------------------
*/

void waitDevice() {
  do {
    ReadStatusRegister1.write();
  } while(QUADSPI.DR()->get() & 1);
}

void initGPIO() {
 /*  Pin | Role                 | Mode                  | Function
  * -----+----------------------+-----------------------+-----------------
  *  PB2 | QUADSPI CLK          | Alternate Function  9 | QUADSPI CLK
  *  PB6 | QUADSPI BK1_NCS      | Alternate Function 10 | QUADSPI BK1_NCS
  *  PC9 | QUADSPI BK1_IO0/SO   | Alternate Function  9 | QUADSPI BK1_IO0
  * PD12 | QUADSPI BK1_IO1/SI   | Alternate Function  9 | QUADSPI BK1_IO1
  *  PC8 | QUADSPI BK1_IO2/WP   | Alternate Function  9 | QUADSPI BK1_IO2
  * PD13 | QUADSPI BK1_IO3/HOLD | Alternate Function  9 | QUADSPI BK1_IO3 */

  // Enable GPIOB, C, D AHB1 peripheral clocks
  RCC.AHB1ENR()->setGPIOBEN(true);
  RCC.AHB1ENR()->setGPIOCEN(true);
  RCC.AHB1ENR()->setGPIODEN(true);

  constexpr static GPIOPin QUADSPIPins[] = {
    GPIOPin(GPIOB, 2),
    GPIOPin(GPIOB, 6),
    GPIOPin(GPIOC, 9),
    GPIOPin(GPIOD,12),
    GPIOPin(GPIOC, 8),
    GPIOPin(GPIOD,13)
  }
  for(const GPIOPin & g : QUADSPIPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), (g == QUADSPIPins[1]) ? GPIO::AFR::AlternateFunction::AF10 : GPIO::AFR::AlternateFunction::AF9);
  }
}

void init() {
  initGPIO();

  // Enable QUADSPI AHB3 peripheral clocks
  RCC.AHB3ENR()->setQSPIEN(true);
  // Configure controller for target device
  QUADSPI.DCR()->setFSIZE(22); // 2^(22+1) bytes in device
  /* TODO CKMODE, CSHT
   * Instructions are initiated on the falling edge of CS and are completed on its rising edge.
   * Mode 0: SCK is low  when CS is high
   * Mode 3: SCK is high when CS is high
   * Input  bits are sampled/latched on the  rising edge of SCK.
   * Output bits are shifted out     on the falling edge of SCK.
   *
   * The IO pins should be at high impedance prior to the falling edge of the first data out clock.
   * Data bytes are shifted with Most Significant Bit first.
   *
   * Configure and enable QUADSPI peripheral
   * All instructions, except Read Data, supported SPI clock frequencies of up to 104MHz.
   * Read Data supports up to 50Mhz.
   * TODO PRESCALER
   * TODO trade-off ? consumption? */
  QUADSPI.CR()->setPRESCALER(255);
  QUADSPI.CR()->setEN(true);
}

class Instruction {
public:
  using QUADSPI::CCR::OperatingMode;
  using QUADSPI::CCR::Size;
  using QUADSPI::CCR::FunctionalMode;
  constexpr Instruction(
    uint8_t instructionCode, OperatingMode IMode, bool SIOO,
    OperatingMode ADMode,
    uint8_t DCyc,
    OperatingMode DMode, uint32_t DLength,
    FunctionalMode FMode
  ) {
    m_ccr = QUADSPI::CCR((uint32_t)0);
    m_ccr.setINSTRUCTION(instructionCode);
    m_ccr.setIMODE (IMode);
    m_ccr.setSIOO  (SIOO);
    m_ccr.setADMODE(ADMode);
    m_ccr.setADSIZE(Size::ThreeBytes);
    m_ccr.setDCYC  (DCyc);
    m_ccr.setDMODE (DMode);
    m_ccr.setFMODE (FMode);
    // DDR not supported by Adesto
  }
  void setAlternateBytes(OperatingMode ABMode, Size ABSize) {
    m_ccr.setABMODE(ABMode);
    m_ccr.setABSIZE(ABSize);
  }
  void write() {
    QUADSPI.CCR()->set(m_ccr);
    QUADSPI.DLR()->set(0);
  }
private:
  class QUADSPI::CCR m_ccr;
  uint32_t m_dlr;
};

//TODO default ccr
/* Modes IMODE ADMODE ABMODE DMODE
 * SPIStandard   1-1-1-1
 * SPIDualOutput 1-1-1-2
 * SPIDualIO     1-2-2-2
 * SPIQuadOutput 1-1-1-4
 * SPIQuadIO     1-4-4-4
 *   Quad SPI instructions require the QE bit in Status Register-2 to be set.
 * QPI           4-4-4-4
 *   Enable/Disable QPI instruction
 * ADSIZE is always ThreeBytes
 * MODE may be NoData*/

/* All Read instructions can be completed after any clocked bit.
 * However, all Write, Program or Erase instructions must complete on a byte (CS driven high
 * after a full byte has been clocked) otherwise the instruction will be terminated. */

using QUADSPI::CCR::OperatingMode;
using QUADSPI::CCR::Size;
using QUADSPI::CCR::FunctionalMode;

static enum InstructionSet : Instruction {
  ReadStatusRegister1 = Instruction(0x05, OperatingMode::Single, false, OperatingMode::NoData, 0, OperatingMode::NoData, FunctionalMode::IndirectRead),
  WriteEnable = Instruction(0x06, OperatingMode::Single, false, OperatingMode::NoData, 0, OperatingMode::NoData, FunctionalMode::IndirectWrite),
  // Before any Program, Erase or Write Status Register instruction
  EraseChip = Instruction(0x60, OperatingMode::Single, false, OperatingMode::NoData, 0, OperatingMode::NoData, FunctionalMode::IndirectWrite)
  /*
  ReadStatusRegister2 = Instruction(0x35),
  ReadData            = Instruction(0x03),
  Erase4kBlock        = Instruction(0x20),
  PageProgram         = Instruction(0x02)
  */
};

void eraseChip() {
  WriteEnable.write();
  EraseChip.write();
  waitDevice();
}

}
}
}

#endif
