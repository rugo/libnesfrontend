//
// Created by rg on 6/11/17.
//

#ifndef DATFRONTEND_DATLIB_H
#define DATFRONTEND_DATLIB_H

/**
 * Initializes the emu.
 * This must be called before any other call!
 */
void nes_init();

/**
 * Loads a NES game and save game from disc.
 * This loads a NES game and savegame
 * into RAM AND KEEPS IT THERE. No other function
 * will load data from disc. The savegame is optional.
 * @param path Path to the NES ROM.
 * @param savegame_path Path to the savegame. This can be NULL.
 * @return true on success.
 */
bool nes_load_game(const char* path, const char* savegame_path);

/**
 * This only needs to be called if visualization with allegro
 * is wished for.
 * @param interactive If this is true, Mario can be controlled
 * with the keyboard (arrow keys).
 * @return true on success
 */
bool nes_init_ui(bool interactive);

/**
 * Run the game for one frame.
 */
void nes_run();

/**
 * Press a key in the game.
 * The key is considered pressed until the function is called
 * again with the same key.
 * @param key RETRO_DEVICE_ID_JOYPAD_* as defined in libretro.h
 */
void press_key(int key);

/**
 * Write the current state as savegame to disk.
 * Errno is set in case file is not writeable.
 * @param path Path to write the savegame to.
 * @return true on success.
 */
bool nes_serialize(char* path);

/**
 * This resets the current game.
 * If a savegame is loaded (via nes_load_game)
 * the game is reverted to this savegame.
 * Otherwise the game is just restarted.
 * @return true on success.
 */
bool nes_reset_game();

/**
 * Read a byte from the NES RAM.
 * @param addr Address to get.
 * @return byte value on RAM address addr
 */
unsigned char nes_get_ram_byte(unsigned addr);

/**
 * Screen dimensions.
 */
#define DISPLAY_WIDTH 256
#define DISPLAY_HEIGHT 240

#endif //DATFRONTEND_DATLIB_H
