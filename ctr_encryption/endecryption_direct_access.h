#ifndef ENDECRYPTION_DIRECT_ACCESS_H

#include <stdint.h>
#include <stdlib.h>

void encrypt_block_jumps_hardcoded(uint8_t* block, size_t size, uint16_t** sboxes);
void decrypt_block_jumps_hardcoded(uint8_t* block, size_t size, uint16_t** sboxes);

#endif /* ENDECRYPTION_DIRECT_ACCESS_H */
