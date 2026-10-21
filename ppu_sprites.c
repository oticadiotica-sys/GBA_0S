#include "ppu_sprites.h"
#include <stdint.h>

static uint8_t oam_memoria_interna[1024];
extern unsigned char vram_do_gba[0x18000];

void ppu_sprites_renderizar_linha(int linha_atual, uint8_t* buffer_tela_tcl) {
    GBA_Sprite_OAM_t* tabela_sprites = (GBA_Sprite_OAM_t*)oam_memoria_interna;

    for (int i = 0; i < 128; i++) {
        GBA_Sprite_OAM_t* sprite = &tabela_sprites[i];
        uint8_t sprite_y = sprite->attribute0 & 0xFF;
        uint8_t modo_objeto = (sprite->attribute0 >> 8) & 0x3;

        if (modo_objeto == 2) continue;

        uint16_t sprite_x = sprite->attribute1 & 0x1FF;
        uint16_t tile_index = sprite->attribute2 & 0x3FF;

        int altura_sprite = 8;
        int largura_sprite = 8;

        if (linha_atual >= sprite_y && linha_atual < (sprite_y + altura_sprite)) {
            int linha_local = linha_atual - sprite_y;

            for (int x_local = 0; x_local < largura_sprite; x_local++) {
                int tela_x = sprite_x + x_local;

                if (tela_x >= 240) continue;

                uint32_t vram_offset = (tile_index * 32) + (linha_local * 4) + (x_local / 2);

                if (vram_offset < 0x18000) {
                    uint8_t par_de_pixels = vram_do_gba[vram_offset];
                    uint8_t index_cor = (x_local % 2 == 0) ? (par_de_pixels & 0xF) : (par_de_pixels >> 4);

                    if (index_cor != 0) {
                        uint32_t pixel_destino = (linha_atual * 240 + tela_x) * 3;
                        buffer_tela_tcl[pixel_destino]     = index_cor * 16;
                        buffer_tela_tcl[pixel_destino + 1] = index_cor * 8;
                        buffer_tela_tcl[pixel_destino + 2] = index_cor * 4;
                    }
                }
            }
        }
    }
}