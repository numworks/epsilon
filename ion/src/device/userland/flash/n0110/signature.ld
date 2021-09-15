FILL(0xFF);
/* We write the last byte of the signature with a default value to make
 * sure the userland size takes into account the signature. */
. += SIGNATURE_LENGTH - 1;
BYTE(0xFF)
