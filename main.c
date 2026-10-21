/* Copyright (c) 2026 oticadiotica-sys. Todos os direitos reservados. Uso pessoal apenas. */

/* =========================================================================
   GBA_OS - Sistema Operacional em Metal Puro para TCL L5 (5033TP)
   Módulo: main.c (Loop Principal e Sincronização do Sistema)
   ========================================================================= */

#include "cpu_arm.h"
#include "cpu_thumb.h"
#include "cpu_memoria.h"
#include "ppu.h"
#include "gba_timers.h"
#include "gba_sintetizador.h"
#include "power.h"
#include <stdint.h>

// Buffer de vídeo temporário para compor a imagem de saída para o ecrã do telemóvel (240x160 RGB)
static uint8_t buffer_tela_tcl[240 * 160 * 3];

// Função fictícia que representa a inicialização dos drivers de baixo nível do TCL L5
extern void tcl_l5_inicializar_hardware(void);
extern void tcl_l5_enviar_buffer_tela(uint8_t* buffer);

int main(void) {
    // 1. Inicializa o hardware físico do telemóvel (Drivers de ecrã, som e botões)
    tcl_l5_inicializar_hardware();

    // 2. Inicializa todos os subsistemas emulados que você criou
    cpu_arm_reset();          // Zera registradores e aponta para 0x08000000
    ppu_init();               // Zera os registradores de vídeo
    gba_timers_init();        // Configura os 4 temporizadores virtuais
    gba_sintetizador_init();  // Limpa o buffer FIFO de som
    power_init();             // Inicializa o sistema de gerenciamento de energia

    // Loop principal de execução Bare-Metal
    while (1) {
        // O GBA renderiza 228 linhas por frame (160 ativas + 68 de V-Blank)
        for (int linha = 0; linha < 228; linha++) {
            // 1. Atualiza a linha atual e renderiza os gráficos se estiver no período ativo
            ppu_renderizar_linha(linha, buffer_tela_tcl);

            // 2. Executa um bloco de instruções da CPU equivalente ao tempo de uma linha de varrimento
            uint32_t ciclos_da_linha = 0;
            while (ciclos_da_linha < 1232) {
                if (cpu.cpsr & (1 << 5)) {
                    uint16_t instr_thumb = gba_memoria_ler8(cpu.registradores[15]) |
                                          (gba_memoria_ler8(cpu.registradores[15] + 1) << 8);

                    cpu_thumb_executar_instrucao(instr_thumb);
                    ciclos_da_linha += 2;
                } else {
                    uint32_t instr_arm = gba_memoria_ler8(cpu.registradores[15]) |
                                        (gba_memoria_ler8(cpu.registradores[15] + 1) << 8) |
                                        (gba_memoria_ler8(cpu.registradores[15] + 2) << 16) |
                                        (gba_memoria_ler8(cpu.registradores[15] + 3) << 24);

                    cpu_arm_executar_instrucao(instr_arm);
                    ciclos_da_linha += 4;
                }

                gba_timers_atualizar(ciclos_da_linha);
                gba_sintetizador_processar();
            }
        }

        power_atualizar();
        tcl_l5_enviar_buffer_tela(buffer_tela_tcl);
    }

    return 0;
}