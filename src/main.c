#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libmine.h"

int step(MSGame_t*);
int get_settings(int *, int *, int *);

int main(int argc, char *argv[])
{
    MSGame_t game;
    // Get settings from user
    int mines, width, height;
    printf("READY\n");
    if (get_settings(&mines, &width, &height))
    {
        printf("NVS\n");
        return 1;
    }

    MSGame_init(&game, width, height, mines);
    printf("OK\n");
    // MSGame_print(&game, 0);
    int running = 1;
    while (running) {
        fflush(stdout);
        int res = step(&game);
        if (res == 1)
        {
            printf("NVM\n");
        }
        else if (res == -1)
        {
            printf("L\n");
            running = 0;
        }
        else if (res == 0)
        {
            if (MSGame_check(&game) == 0)
            {
                printf("W\n");
                running = 0;
            }
        }
        else if (res == -2)
        {
            printf("FAILED GENERATING GAME\n");
            return 1;
        }
        printf("END\n");
    }
    return 0;
}

int step(MSGame_t *game)
{
    char buf[32];
    fgets(buf, 32, stdin);
    int len = strlen(buf);
    if (len == 1) return 0;
    buf[len-1] = 0;

    char *token = strtok(buf, " ");
    if (strcmp(token, "P") == 0)
    {
        // Getting x coordinate
        token = strtok(NULL, " ");
        if (token == NULL) return 1;
        int x = atoi(token);

        // Getting y coordinate
        token = strtok(NULL, " ");
        if (token == NULL) return 1;
        int y = atoi(token);

        int res = MSGame_peek(game, x, y);
        // MSGame_print(game, 0);
        MSGame_diff(game);
        if (res == -1) return -2;
        return (res ? -1 : 0);
    }
    else if (strcmp(token, "F") == 0)
    {
        // Getting x coordinate
        token = strtok(NULL, " ");
        if (token == NULL) return 1;
        int x = atoi(token);

        // Getting y coordinate
        token = strtok(NULL, " ");
        if (token == NULL) return 1;
        int y = atoi(token);

        int flags = MSGame_flag(game, x, y);
        if (flags == -1) return 1;
        // MSGame_print(game, 0);
        MSGame_diff(game);
        // printf("Flags left: %d\n", flags);
        return 0;
    }
    else if (strcmp(token, "S") == 0)
    {
        MSGame_state(game);
        return 0;
    }
    else if (strcmp(token, "Q") == 0)
    {
        return -1;
    }
    else if (strcmp(token, "PRINT") == 0)
    {
        MSGame_print(game, 0);
        printf("Flags left: %d\n", game->mines - game->placed_mines);
        return 0;
    }
    return 1;
}

int get_settings(int *mines, int *width, int *height)
{
    char buf[32];
    fgets(buf, 32, stdin);
    int len = strlen(buf);
    if (len == 1) return 1;
    buf[len-1] = 0;

    char *token = strtok(buf, " ");
    // First is n-mines
    *mines = atoi(token);
    if (*mines < 1) return 1;

    // Get width
    token = strtok(NULL, " ");
    if (token == NULL) return 1;
    *width = atoi(token);
    if (*width < 1) return 1;

    // Get height
    token = strtok(NULL, " ");
    if (token == NULL) return 1;
    *height = atoi(token);
    if (*height < 1) return 1;
    return 0;
}
