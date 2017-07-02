#include "libretro.h"
#include "frontend_lib.h"
#include <stdio.h>
#include "allegro5/allegro.h"

/* Number of keys on the game pad */
#define NUM_KEYS 16

/* Allegro global vars */
ALLEGRO_BITMAP *display_bitmap;
ALLEGRO_DISPLAY *display;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_EVENT ret_event;

bool keys_pressed[NUM_KEYS];
bool show_ui = false;
bool ui_interactive = false;

/* These store the game info and savegame.
 * They are both loaded only once. */
char* savegame_buf = NULL;
struct retro_game_info* game_info = NULL;

/* Libretro callbacks */
void video_refresh_cb(const void *data, unsigned width, unsigned height, size_t pitch) {
    if (show_ui) {
        ALLEGRO_LOCKED_REGION *locked_region;
        int lock_flags = ALLEGRO_LOCK_READWRITE;
        /* generate flat 32 bit pixel buffer for testing */
        uint16_t *buffer = (uint16_t *) data;

        /* Locking the bitmap means, we work directly with pixel data.  */

        locked_region = al_lock_bitmap(display_bitmap, ALLEGRO_PIXEL_FORMAT_RGB_565, lock_flags);
        int line; //number of display lines written
        for (line = 0; line < height; line++) {
            uint8_t *ptr = (uint8_t *) locked_region->data + line * locked_region->pitch;
            uint16_t *cptr = (uint16_t *) ptr;
            memcpy(cptr, buffer + line * width, pitch); /* blit pixels into locked backbuffer */
        }
        al_unlock_bitmap(display_bitmap);
        al_flip_display();
    }
}

/*
 * This returns true for supported commands.
 */
bool environ_cb(unsigned cmd, void *data){
    enum retro_pixel_format *fmt = (enum retro_pixel_format *)data;
    if(cmd == RETRO_ENVIRONMENT_SET_PIXEL_FORMAT && *fmt == RETRO_PIXEL_FORMAT_RGB565){
        return true; /* quicknes asks if we support RGB565 */
    }
    return false; /* everything else is irrelevant */
}

/* Audio callbacks. Not implemented. */
void audio_sample_cb (int16_t left, int16_t right) {}

size_t audio_s_batch_cb(const int16_t *data, size_t frames) {
    return 0;
}

void poll_input_cb(void){}

/* Input callback.
 * Returns 1 if the given key (id) is pressed.
 */
int16_t input_state_cb(unsigned port, unsigned device,
                    unsigned index, unsigned id){
    if (keys_pressed[id]) {
        return 1;
    }
    return 0;
}

void nes_init() {
    retro_set_video_refresh(&video_refresh_cb);
    retro_set_environment(&environ_cb);
    retro_set_audio_sample(&audio_sample_cb);
    retro_set_audio_sample_batch(&audio_s_batch_cb);
    retro_set_input_poll(&poll_input_cb);
    retro_set_input_state(&input_state_cb);
    retro_init();
}

bool nes_load_game(const char* path, const char* savegame_path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        goto fail;
    }
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);

    if(game_info){
        free((char*)game_info->data);
        free(game_info);
    }

    game_info = (struct retro_game_info*) malloc(sizeof(struct retro_game_info));
    game_info->path = NULL;
    game_info->meta = NULL;
    game_info->size = (int) fsize;
    game_info->data = malloc(game_info->size);

    if (!fread((char*)game_info->data, fsize, 1, f)) {
        goto fail;
    }

    if (!retro_load_game(game_info)) {
        goto fail;
    }

    fclose(f);

    /* load savegame if path given */
    if (savegame_path) {
        size_t s = retro_serialize_size();
        if (savegame_buf) {
            free(savegame_buf);
        }
        savegame_buf = (char*) malloc(s);
        f = fopen(savegame_path, "rb");
        if (!f) {
            goto fail;
        }
        fseek(f, 0, SEEK_END);
        fsize = ftell(f);
        rewind(f);
        if (!fread(savegame_buf, fsize, 1, f)) {
            goto fail;
        }
        nes_reset_game();
        fclose(f);
    }
    return true;
fail:
    fclose(f);
    return false;
}

void press_key(int key) {
    if (key >= 0 && key < NUM_KEYS){
        keys_pressed[key] ^= 1;
    }
}

bool nes_init_ui(bool interactive) {
    show_ui = true;
    al_init();
    display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    display_bitmap = al_get_backbuffer(display);
    /* clear display_bitmap to nice color */
    al_set_target_bitmap(display_bitmap);
    if (interactive) {
        ui_interactive = true;
        if(!al_install_keyboard()) {
            return false;
        }

        event_queue = al_create_event_queue();
        if(!event_queue) {
            return false;
        }

        al_register_event_source(event_queue, al_get_keyboard_event_source());
    }
    return true;
}

bool nes_reset_game() {
    if (savegame_buf) {
        return retro_unserialize(savegame_buf, retro_serialize_size()); /* save game loaded */
    } else {
        retro_reset(); /* no savegame, just reset the console */
        return true;
    }
}

unsigned char nes_get_ram_byte(unsigned addr) {
    return *(((unsigned char*)retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM)) + addr);
}

bool nes_serialize(char* path) {
    size_t s = retro_serialize_size();
    char* buf = (char*) malloc(s);

    if (!retro_serialize(buf, s)) {
        return false;
    }

    FILE *f = fopen(path, "wb");

    if (!f) {
        return false;
    }

    if (!fwrite(savegame_buf, s, 1, f)) {
        return false;
    }
    fclose(f);
    return true;
}
void nes_run() {
    retro_run();
    if (ui_interactive) {
        if (al_get_next_event(event_queue, &ret_event)) {
            if (ret_event.type == ALLEGRO_EVENT_KEY_DOWN || ret_event.type == ALLEGRO_EVENT_KEY_UP) {
                switch(ret_event.keyboard.keycode) {
                    case ALLEGRO_KEY_ENTER:
                        press_key(RETRO_DEVICE_ID_JOYPAD_START);
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        press_key(RETRO_DEVICE_ID_JOYPAD_RIGHT);
                        break;
                    case ALLEGRO_KEY_LEFT:
                        press_key(RETRO_DEVICE_ID_JOYPAD_LEFT);
                        break;
                    case ALLEGRO_KEY_UP:
                        press_key(RETRO_DEVICE_ID_JOYPAD_A);
                        break;
                }
            }
        }
    }
}
