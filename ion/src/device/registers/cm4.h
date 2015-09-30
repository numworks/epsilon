#ifndef STM32_REGISTERS_CM4_H
#define STM32_REGISTERS_CM4_H 1

// Coprocessor Access Control Register
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BEHBJHIG.html

#define CPACR (*(volatile uint32_t *)(0xE000ED88))

#define CPACR_ACCESS_DENIED 0
#define CPACR_ACCESS_PRIVILEGED_ONLY 1
#define CPACR_ACCESS_RESERVED 2
#define CPACR_ACCESS_FULL 3

#define LOW_BIT_CPACR_CP(v) (2*v)
#define HIGH_BIT_CPACR_CP(v) (2*v+1)


// Application Interrupt and Reset Control Register
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihehdge.html

#define AIRCR (*(volatile uint32_t *)(0xE000ED0C))

#define AIRCR_VECTKEY_MASK (0x5FA << 16)
#define AIRCR_SYSRESETREQ (1<<2)

#endif
