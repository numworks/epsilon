#ifndef REGS_OTG_H
#define REGS_OTG_H

#include <shared/regs/register.h>
#include <config/otg.h>

namespace Ion {
namespace Device {
namespace Regs {

class OTG {
public:
  class GOTGINT : public Register32 {
  public:
    REGS_BOOL_FIELD(SEDET, 2);
  };

  class GAHBCFG : public Register32 {
  public:
    REGS_BOOL_FIELD(GINTMSK, 0);
  };

  class GUSBCFG : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(PHYSEL, 6);
    REGS_FIELD(TRDT, uint8_t, 13, 10);
    REGS_BOOL_FIELD(FDMOD, 30);
  };

  class GRSTCTL : public Register32 {
  public:
    REGS_BOOL_FIELD(CSRST, 0);
    REGS_BOOL_FIELD(RXFFLSH, 4);
    REGS_BOOL_FIELD(TXFFLSH, 5);
    REGS_FIELD(TXFNUM, uint8_t, 10, 6);
    REGS_BOOL_FIELD(AHBIDL, 31);
  };

  class GINTSTS : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(MMIS, 1);
    REGS_BOOL_FIELD(SOF, 3);
    REGS_BOOL_FIELD(RXFLVL, 4);
    REGS_BOOL_FIELD(USBSUSP, 11);
    REGS_BOOL_FIELD(USBRST, 12);
    REGS_BOOL_FIELD(ENUMDNE, 13);
    REGS_BOOL_FIELD(IEPINT, 18);
    REGS_BOOL_FIELD(WKUPINT, 31);
  };

  class GINTMSK : public Register32 {
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(RXFLVLM, 4);
    REGS_BOOL_FIELD(USBSUSPM, 11);
    REGS_BOOL_FIELD(USBRST, 12);
    REGS_BOOL_FIELD(ENUMDNEM, 13);
    REGS_BOOL_FIELD(IEPINT, 18);
    REGS_BOOL_FIELD(WUIM, 31);
  };

  class GRXSTSP : public Register32 {
  public:
    using Register32::Register32;
    enum class PKTSTS : uint8_t {
      GlobalOutNAK = 1,
      OutReceived = 2,
      OutTransferCompleted = 3, // After each Out Transaction
      SetupTransactionCompleted = 4, // Supposed to be after each SETUP transaction
      SetupReceived = 6
    };
    REGS_FIELD(EPNUM, uint8_t, 3, 0);
    REGS_FIELD(BCNT, uint16_t, 14, 4);
    PKTSTS getPKTSTS() volatile { return (PKTSTS)getBitRange(20, 17); }
  };

  class GRXFSIZ : public Register32 {
  public:
    REGS_FIELD(RXFD, uint16_t, 15, 0);
  };

  class DIEPTXF0 : public Register32 {
  public:
    REGS_FIELD(TX0FSA, uint16_t, 15, 0);
    REGS_FIELD(TX0FD, uint16_t, 31, 16);
  };

  class GCCFG : public Register32 {
  public:
    REGS_BOOL_FIELD(PWRDWN, 16);
    REGS_BOOL_FIELD(VBDEN, 21);
  };

  class DCFG : public Register32 {
  public:
    enum class DSPD : uint8_t {
      FullSpeed = 3,
    };
    void setDSPD(DSPD s) volatile { setBitRange(1, 0, (uint8_t)s); }
    REGS_FIELD(DAD, uint8_t, 10, 4);
  };

  class DCTL : public Register32 {
  public:
    REGS_BOOL_FIELD(SDIS, 1);
  };

  class DIEPMSK : public Register32 {
  public:
    REGS_BOOL_FIELD(XFRCM, 0);
  };

  class DAINTMSK : public Register32 {
  public:
    REGS_FIELD(IEPM, uint16_t, 15, 0);
    REGS_FIELD(OEPM, uint16_t, 31, 16);
  };

  class DIEPCTL0 : public Register32 {
  public:
    using Register32::Register32;
#if REGS_CONFIG_OTG_LARGE_MPSIZ
    enum class MPSIZ : uint16_t {
      Size64 = 64,
    };
    REGS_TYPE_FIELD(MPSIZ, 10, 0);
#else
    enum class MPSIZ : uint8_t {
      Size64 = 0,
      Size32 = 1,
      Size16 = 2,
      Size8 = 3
    };
    REGS_TYPE_FIELD(MPSIZ, 1, 0);
#endif
    REGS_BOOL_FIELD(STALL, 21);
    REGS_FIELD(TXFNUM, uint8_t, 25, 22);
    REGS_BOOL_FIELD(CNAK, 26);
    REGS_BOOL_FIELD(SNAK, 27);
    REGS_BOOL_FIELD(EPENA, 31);
  };

  class DOEPCTL0 : public Register32 {
  public:
    REGS_BOOL_FIELD(CNAK, 26);
    REGS_BOOL_FIELD(SNAK, 27);
    REGS_BOOL_FIELD(EPENA, 31);
  };

  class DIEPINT : public Register32 {
  public:
    REGS_BOOL_FIELD(XFRC, 0);
    REGS_BOOL_FIELD(INEPNE, 6);
  };

  class DIEPTSIZ0 : public Register32 {
  public:
    using Register32::Register32;
    REGS_FIELD(XFRSIZ, uint8_t, 6, 0);
    REGS_FIELD(PKTCNT, uint8_t, 20, 19);
  };

  class DOEPTSIZ0 : public Register32 {
  public:
    using Register32::Register32;
    REGS_FIELD(XFRSIZ, uint8_t, 6, 0);
    REGS_BOOL_FIELD(PKTCNT, 19);
    REGS_FIELD(STUPCNT, uint8_t, 30, 29);
  };

  class PCGCCTL : public Register32 {
  public:
    REGS_BOOL_FIELD(STPPCLK, 0);
    REGS_BOOL_FIELD(GATEHCLK, 1);
  };

  class DFIFO0 : public Register32 {
  };

  constexpr OTG() {};
  REGS_REGISTER_AT(GOTGINT, 0x004);
  REGS_REGISTER_AT(GAHBCFG, 0x008);
  REGS_REGISTER_AT(GUSBCFG, 0x00C);
  REGS_REGISTER_AT(GRSTCTL, 0x010);
  REGS_REGISTER_AT(GINTSTS, 0x014);
  REGS_REGISTER_AT(GINTMSK, 0x018);
  REGS_REGISTER_AT(GRXSTSP, 0x020);
  REGS_REGISTER_AT(GRXFSIZ, 0x024);
  REGS_REGISTER_AT(DIEPTXF0, 0x28);
  REGS_REGISTER_AT(GCCFG, 0x038);
  REGS_REGISTER_AT(DCFG, 0x800);
  REGS_REGISTER_AT(DCTL, 0x804);
  REGS_REGISTER_AT(DIEPMSK, 0x810);
  REGS_REGISTER_AT(DAINTMSK, 0x81C);
  REGS_REGISTER_AT(DIEPCTL0, 0x900);
  REGS_REGISTER_AT(DIEPTSIZ0, 0x910);
  REGS_REGISTER_AT(DOEPCTL0, 0xB00);
  REGS_REGISTER_AT(DOEPTSIZ0, 0xB10);
  REGS_REGISTER_AT(PCGCCTL, 0xE00);
  REGS_REGISTER_AT(DFIFO0, 0x1000);
  constexpr volatile DIEPINT * DIEPINT(int i) const {
    return (class DIEPINT *)(Base() + 0x908 + i*0x20);
  }
  constexpr uint32_t Base() const {
    return REGS_CONFIG_OTG_BASE_ADDRESS;
  }
};

constexpr OTG OTG;

}
}
}

#endif
