//
// Created by rg on 6/11/17.
//

#include <stdio.h>
#include "libretro.h"
#include "frontend_lib.h"

/*
 * Play Mario with mouse and keyboard.
 */
int main(int argc, char** argv) {
    int i = 0;
    nes_init();
    nes_init_ui(true);
    bool k = nes_load_game("/home/rg/mario_bros.nes", "/home/rg/mario_bros.nes-savegame-right");
    for (;;){
        nes_run();
        if (!(++i % 1000)){
            printf("Will reset the game now :P.\nScore: %d\n",
                   (nes_get_ram_byte(0x6D) << 8) +  nes_get_ram_byte(0x86));
            nes_reset_game();
        }
    }
    return 0;
}