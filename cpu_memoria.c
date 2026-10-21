#include "cpu_memoria.h"
#include "ppu.h"
#include <stdint.h>

// VRAM global compartilhada com a PPU
unsigned char vram_do_gba[0x18000];

static uint8_t bios[0x4000];
static uint8_t ewram[0x40000];
static uint8_t iwram[0x8000];
static uint8_t io_regs[0x400];
static uint8_t palette_ram[0x400];
static uint8_t oam[0x400];

uint8_t gba_memoria_ler8(uint32_t endereco) {
    if (endereco < 0x04000000) {
        return bios[endereco & 0x3FFF];
    }

    if (endereco >= 0x02000000 && endereco < 0x02040000) {
        return ewram[endereco - 0x02000000];
    }

    if (endereco >= 0x03000000 && endereco < 0x03008000) {
        return iwram[endereco - 0x03000000];
    }

    if (endereco >= 0x04000000 && endereco < 0x04000400) {
        return io_regs[endereco - 0x04000000];
    }

    if (endereco >= 0x05000000 && endereco < 0x05000400) {
        return palette_ram[endereco - 0x05000000];
    }

    if (endereco >= 0x06000000 && endereco < 0x06018000) {
        return vram_do_gba[endereco - 0x06000000];
    }

    if (endereco >= 0x07000000 && endereco < 0x07000400) {
        return oam[endereco - 0x07000000];
    }

    return 0;
}

uint16_t gba_memoria_ler16(uint32_t endereco) {
    if (endereco >= 0x04000000 && endereco <= 0x04000006) {
        return ppu_ler_registador(endereco);
    }

    return (uint16_t)(gba_memoria_ler8(endereco) |
                      (gba_memoria_ler8(endereco + 1) << 8));
}

uint32_t gba_memoria_ler32(uint32_t endereco) {
    uint16_t low = gba_memoria_ler16(endereco);
    uint16_t high = gba_memoria_ler16(endereco + 2);
    return ((uint32_t)high << 16) | low;
}

void gba_memoria_escrever8(uint32_t endereco, uint8_t valor) {
    if (endereco >= 0x02000000 && endereco < 0x02040000) {
        ewram[endereco - 0x02000000] = valor;
        return;
    }

    if (endereco >= 0x03000000 && endereco < 0x03008000) {
        iwram[endereco - 0x03000000] = valor;
        return;
    }

    if (endereco >= 0x04000000 && endereco < 0x04000400) {
        io_regs[endereco - 0x04000000] = valor;
        return;
    }

    if (endereco >= 0x05000000 && endereco < 0x05000400) {
        palette_ram[endereco - 0x05000000] = valor;
        return;
    }

    if (endereco >= 0x06000000 && endereco < 0x06018000) {
        vram_do_gba[endereco - 0x06000000] = valor;
        return;
    }

    if (endereco >= 0x07000000 && endereco < 0x07000400) {
        oam[endereco - 0x07000000] = valor;
        return;
    }
}

void gba_memoria_escrever16(uint32_t endereco, uint16_t valor) {
    if (endereco >= 0x04000000 && endereco <= 0x04000006) {
        ppu_escrever_registador(endereco, valor);
        return;
    }

    gba_memoria_escrever8(endereco, valor & 0xFF);
    gba_memoria_escrever8(endereco + 1, (valor >> 8) & 0xFF);
}

void gba_memoria_escrever32(uint32_t endereco, uint32_t valor) {
    gba_memoria_escrever16(endereco, valor & 0xFFFF);
    gba_memoria_escrever16(endereco + 2, (valor >> 16) & 0xFFFF);
}