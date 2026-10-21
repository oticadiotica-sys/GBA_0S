#ifndef CPU_MEMORIA_H
#define CPU_MEMORIA_H

#include <stdint.h>

// Leitura em 8, 16 e 32 bits
uint8_t gba_memoria_ler8(uint32_t endereco);
uint16_t gba_memoria_ler16(uint32_t endereco);
uint32_t gba_memoria_ler32(uint32_t endereco);

// Escrita em 8, 16 e 32 bits
void gba_memoria_escrever8(uint32_t endereco, uint8_t valor);
void gba_memoria_escrever16(uint32_t endereco, uint16_t valor);
void gba_memoria_escrever32(uint32_t endereco, uint32_t valor);

#endif