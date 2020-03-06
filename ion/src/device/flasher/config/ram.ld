/* Config file for a ram binary linked at address 0x20032000, taking all the RAM
 * space available. It is used to build flasher that can write both in flash and
 * in RAM up to the address 0x20030000. */

CONFIG_OFFSET = 192K - 32K;
CONFIG_LENGTH = 256K - 192K;
