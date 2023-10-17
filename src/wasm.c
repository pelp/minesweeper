#include "libmine.h"
#include <stdio.h>
#include <time.h>
#include <emscripten/emscripten.h>

#define WASM_FLAG 10
#define WASM_COVER 11
#define WASM_MINE 9

EMSCRIPTEN_KEEPALIVE MSGame_t game;
EMSCRIPTEN_KEEPALIVE time_t end_time, start_time;
EMSCRIPTEN_KEEPALIVE char *interface = NULL;

EMSCRIPTEN_KEEPALIVE void start(int mines, int width, int height)
{
    start_time = time(NULL);
    end_time = start_time;
    MSGame_init(&game, width, height, mines);
}

EMSCRIPTEN_KEEPALIVE int peek(int x, int y)
{
    if (game.started == 0)
    {
        start_time = time(NULL);
        end_time = start_time;
    }
    if (MSGame_peek(&game, x, y)) return 1;
    return MSGame_check(&game) ? 0 : 2;
}

EMSCRIPTEN_KEEPALIVE int flag(int x, int y)
{
    if (MSGame_flag(&game, x, y)) return 1;
    return MSGame_check(&game) ? 0 : 2;
}

EMSCRIPTEN_KEEPALIVE char * state()
{
    // Allocate interface buffer
    if (interface == NULL)
    {
        interface = malloc(sizeof(char) * game.width * game.height);
    }

    // Filter the data
    for (int i = 0; i < game.width * game.height; i++)
    {
        if (game.field[i] & FLAG_MASK)
        {
            interface[i] = WASM_FLAG;
        }
        else if (game.field[i] & COVER_MASK)
        {
            interface[i] = WASM_COVER;
        }
        else if (game.field[i] & MINE)
        {
            interface[i] = WASM_MINE;
        }
        else
        {
            interface[i] = game.field[i];
        }
    }
    return interface;
}

EMSCRIPTEN_KEEPALIVE void info()
{
    printf("MINES: %d\nWIDTH: %d\nHEIGHT: %d\n", game.mines, game.width, game.height);
}

EMSCRIPTEN_KEEPALIVE int flags()
{
    return game.mines - game.placed_mines;
}

EMSCRIPTEN_KEEPALIVE int get_time()
{
    if (game.started == 1) end_time = time(NULL);
    return end_time - start_time;
}
