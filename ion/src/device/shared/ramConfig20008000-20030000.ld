/* Config file for a ram binary linked at address 0x20008000, up to 0x20030000.
 * This is used to build benchRAM. In what is left of the device's RAM, we might
 * put a flasher. */

CONFIG_OFFSET = 0x0;
CONFIG_LENGTH = 192K - 32K;
