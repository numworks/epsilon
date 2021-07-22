/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">INTERNAL_FLASH" to ask for a given section to
 * be stored in Flash. */

SIGNATURE_LENGTH = 64;

/* The few restrictions to layout the RAM memory are the following:
 * - the begining of Kernel data/bss section should be aligned: the isr table
 * is within this section and the VTOR registers can only be configure with
 * some alignments constraints
 * - the kernel RAM should be aligned to be able to add MPU protection over
 * this region: MPU region must be aligned to the size of the region.
 * - Overflowing the userland stack should not impact the kernel (the MPU region
 * should protect it anyway)
 *
 * |                                SRAM                                                 |
 * | USERLAND DATA/BSS | HEAP | USERLAND STACK | KERNEL STACK | KERNEL DATA/BSS - canary |
 *
 * Bootloader SRAM is mapped like:
 * |                                    STACK                                   | canary |
 * Kernel SRAM is mapped like:
 * | xxxxxxxxxxxxxxxxxxxxxxxx | USERLAND STACK | KERNEL STACK | KERNEL DATA/BSS - canary |
 * Userland SRAM is mapped like:
 * | USERLAND DATA/BSS | HEAP | USERLAND STACK | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx |
 *
 */

SRAM_ORIGIN = 0x20000000;
SRAM_LENGTH = 256K;
USERLAND_STACK_LENGTH = 32K;
/* WARNING: when choosing the kernel RAM length, keep in mind that the MPU
 * region address should be aligned on the MPU region size!
 * Experimentally:
 * - kernel data/bss = 2416 bytes < 3K
 * - kernel stack = 236 < 1K
 */
KERNEL_STACK_LENGTH = 1K;
KERNEL_SRAM_DATA_BSS_LENGTH = 3K;

KERNEL_SRAM_LENGTH = KERNEL_SRAM_DATA_BSS_LENGTH + KERNEL_STACK_LENGTH;
KERNEL_SRAM_ORIGIN = SRAM_ORIGIN + SRAM_LENGTH - KERNEL_SRAM_LENGTH;
USERLAND_SRAM_ORIGIN = SRAM_ORIGIN;
USERLAND_SRAM_LENGTH = SRAM_LENGTH - KERNEL_SRAM_LENGTH;

/* For N0110?
 ITCM (rwx) : ORIGIN = 0x00000000, LENGTH = 16K
 DTCM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
 SRAM1 (rwx) : ORIGIN = 0x20010000, LENGTH = 176K
 SRAM2 (rwx) : ORIGIN = 0x2003C000, LENGTH = 16K
*/

