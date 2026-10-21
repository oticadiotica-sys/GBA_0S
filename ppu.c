#include "ppu.h"
#include "ppu_sprites.h"
#include <stdint.h>

// Armazenamento interno dos registadores de vídeo emulados
static uint16_t dispcnt = 0;
static uint16_t dispstat = 0;
static uint16_t vcount = 0;

// Acede à tua VRAM global declarada no sistema
extern unsigned char vram_do_gba[0x18000];

static void ppu_preencher_fundo(int linha_atual, uint8_t* buffer_tela_tcl,
                               uint8_t r_base, uint8_t g_base, uint8_t b_base,
                               uint8_t r_var, uint8_t g_var, uint8_t b_var,
                               uint8_t r_alt, uint8_t g_alt, uint8_t b_alt) {
    for (int x = 0; x < 240; x++) {
        uint32_t px = (linha_atual * 240 + x) * 3;

        uint8_t r = r_base + ((x + linha_atual) % r_var);
        uint8_t g = g_base + ((x * 2 + linha_atual) % g_var);
        uint8_t b = b_base + ((x + linha_atual * 2) % b_var);

        if ((x + linha_atual) % 2 == 0) {
            r = r_alt;
            g = g_alt;
            b = b_alt;
        }

        buffer_tela_tcl[px]     = r;
        buffer_tela_tcl[px + 1] = g;
        buffer_tela_tcl[px + 2] = b;
    }
}

static void ppu_renderizar_modo_0(int linha_atual, uint8_t* buffer_tela_tcl) {
    // Modo 0: fundo azul/ciano
    ppu_preencher_fundo(linha_atual, buffer_tela_tcl,
                        20, 35, 90,
                        12, 12, 18,
                        80, 140, 200);
}

static void ppu_renderizar_modo_1(int linha_atual, uint8_t* buffer_tela_tcl) {
    // Modo 1: fundo verde
    ppu_preencher_fundo(linha_atual, buffer_tela_tcl,
                        25, 60, 20,
                        18, 14, 16,
                        120, 160, 60);
}

static void ppu_renderizar_modo_2(int linha_atual, uint8_t* buffer_tela_tcl) {
    // Modo 2: fundo magenta/roxo
    ppu_preencher_fundo(linha_atual, buffer_tela_tcl,
                        55, 20, 65,
                        16, 16, 18,
                        180, 80, 180);
}

static void ppu_renderizar_modo_3(int linha_atual, uint8_t* buffer_tela_tcl) {
    for (int x = 0; x < 240; x++) {
        uint32_t vram_offset = (linha_atual * 240 + x) * 2;

        if (vram_offset < 0x14000) {
            uint16_t cor_gba = vram_do_gba[vram_offset] |
                              (vram_do_gba[vram_offset + 1] << 8);

            uint8_t r = (cor_gba & 0x1F) << 3;
            uint8_t g = ((cor_gba >> 5) & 0x1F) << 3;
            uint8_t b = ((cor_gba >> 10) & 0x1F) << 3;

            uint32_t pixel_destino = (linha_atual * 240 + x) * 3;
            buffer_tela_tcl[pixel_destino]     = r;
            buffer_tela_tcl[pixel_destino + 1] = g;
            buffer_tela_tcl[pixel_destino + 2] = b;
        }
    }
}

void ppu_init(void) {
    dispcnt = 0;
    dispstat = 0;
    vcount = 0;
}

uint16_t ppu_ler_registador(uint32_t endereco) {
    switch (endereco) {
        case REG_DISPCNT:  return dispcnt;
        case REG_DISPSTAT: return dispstat;
        case REG_VCOUNT:   return vcount;
        default:           return 0;
    }
}

void ppu_escrever_registador(uint32_t endereco, uint16_t valor) {
    switch (endereco) {
        case REG_DISPCNT:
            dispcnt = valor;
            break;
        case REG_DISPSTAT:
            dispstat = (dispstat & 0x0007) | (valor & 0xFFF8);
            break;
        default:
            break;
    }
}

void ppu_renderizar_linha(int linha_atual, uint8_t* buffer_tela_tcl) {
    vcount = linha_atual;

    if (linha_atual >= 160) {
        dispstat |= (1 << 0);
        return;
    }

    dispstat &= ~(1 << 0);

    uint8_t modo_grafico = dispcnt & 0x7;

    switch (modo_grafico) {
        case 0:
            ppu_renderizar_modo_0(linha_atual, buffer_tela_tcl);
            break;
        case 1:
            ppu_renderizar_modo_1(linha_atual, buffer_tela_tcl);
            break;
        case 2:
            ppu_renderizar_modo_2(linha_atual, buffer_tela_tcl);
            break;
        case 3:
            ppu_renderizar_modo_3(linha_atual, buffer_tela_tcl);
            break;
        default:
            for (int x = 0; x < 240; x++) {
                uint32_t px = (linha_atual * 240 + x) * 3;
                buffer_tela_tcl[px]     = 0;
                buffer_tela_tcl[px + 1] = 0;
                buffer_tela_tcl[px + 2] = 0;
            }
            break;
    }

    // Renderiza sprites por cima do fundo
    ppu_sprites_renderizar_linha(linha_atual, buffer_tela_tcl);
}