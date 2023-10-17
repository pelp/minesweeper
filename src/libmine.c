// TODO: Always start on empty field
#include "libmine.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define MIN(a, b) (a < b) ? a : b
#define MAX(a, b) (a > b) ? a : b

void _clear(char*, int, int);
void _random_mine(MSGame_t*, int*, int*);
int _peek(MSGame_t*, int, int, int);
void _step(MSGame_t*);
void _generate(MSGame_t*);
void _diff(char*, char*, int, int);

void MSGame_init(MSGame_t *game, int width, int height, int mines)
{
    game->width = width;
    game->height = height;
    game->mines = mines;
    game->placed_mines = 0;
    game->started = 0;

    if (game->field != NULL) {
        free(game->field);
    }
    if (game->pfield != NULL) {
        free(game->pfield);
    }

    game->field = malloc(sizeof(char) * width * height);
    game->pfield = malloc(sizeof(char) * width * height);

    // Clear field
    _clear(game->field, game->width, game->height);
    _clear(game->pfield, game->width, game->height);
}

void _generate(MSGame_t *game)
{
    // Generate the mines
	int x, y;
    for (int i = 0; i < game->mines; i++)
    {
        _random_mine(game, &x, &y);

    	// Generate distances
		int uy = MIN(y+2, game->height);
		int ly = MAX(y-1, 0);
		int ux = MIN(x+2, game->width);
		int lx = MAX(x-1, 0);
		for (int j = ly; j < uy; j++)
		{
			for (int k = lx; k < ux; k++)
			{
				if (j == y && k == x) continue;
				game->field[j * game->width + k]++;
			}
		}
    }
}

void _clear(char *field, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            field[i * width + j] = COVER_MASK;
        }
    }
}

void _random_mine(MSGame_t *game, int *x, int *y)
{
    *x = rand() % game->width;
    *y = rand() % game->height;
    char *val = &game->field[*y * game->width + *x];
    if ((*val & MINE) == 0)
    {
        *val |= MINE;
        return;
    }
    _random_mine(game, x, y);
}

void MSGame_print(MSGame_t *game, char xray)
{
    // Enable xray mask
    char mask = 0xFF;
    if (xray) mask &= MASK;

    printf("   ");
    for (int j = 0; j < game->width; j++)
    {
        printf("%d", j);
    }
    printf("\n  +");
    for (int j = 0; j < game->width; j++)
    {
        printf("-");
    }
    printf("+\n");
    for (int i = 0; i < game->height; i++)
    {
        printf("%d |", i);
        for (int j = 0; j < game->width; j++)
        {
            char *val = &game->field[i * game->width + j];
            if (*val & FLAG_MASK)
            {
                printf("F");
            }
            else if (*val & mask & COVER_MASK)
            {
                printf("#");
            }
            else if (xray != 3 && *val & MINE)
            {
                printf("*");
            }
            else if ((*val & mask) == 0)
            {
                printf(" ");
            }
            else
            {
                printf("%d", *val & mask);
            }
        }
        printf("|\n");
    }
    printf("  +");
    for (int j = 0; j < game->width; j++)
    {
        printf("-");
    }
    printf("+\n");
}

int _peek(MSGame_t *game, int x, int y, int first)
{
    char *val = &game->field[y * game->width + x];
    if (*val & FLAG_MASK) return 0;
    if (!(*val & COVER_MASK) && !first) return 0;
    char open_number = (first &&
        (((*val & (FLAG_MASK | COVER_MASK)) == 0) &&
        ((*val & MASK) > 0)));
    *val &= MASK | MINE;
    if (*val & MINE) {
        game->started = 2;
        return 1;
    }
    int uy = MIN(y+2, game->height);
    int ly = MAX(y-1, 0);
    int ux = MIN(x+2, game->width);
    int lx = MAX(x-1, 0);
    int lost = 0;
    if (((*val & MASK) == 0) || open_number)
    {
        for (int i = ly; i < uy; i++)
        {
            for (int j = lx; j < ux; j++)
            {
                if (_peek(game, j, i, 0)) {
                    lost = 1;
                }
            }
        }
    }
    return lost;
}

void _step(MSGame_t *game)
{
    memcpy(game->pfield, game->field,
           sizeof(char) * game->width * game->height);
}

int MSGame_peek(MSGame_t *game, int x, int y)
{
    if (game->started == 0)
    {
        // Seed random generator with time
        srand((unsigned) time(NULL));

        int i;
        for (i = 0; i < 1000; i++)
        {
            _generate(game);
            char *val = &game->field[y * game->width + x];
            if ((*val & (MASK | MINE)) == 0) 
            {
                game->started = 1;
                return MSGame_peek(game, x ,y);
            }
            _clear(game->field, game->width, game->height);
        }
        return -1;
    }
    _step(game);
    return _peek(game, x, y, 1);
}

int MSGame_flag(MSGame_t *game, int x, int y)
{
    if (game->started == 0) return 1;
    _step(game);
    char *val = &game->field[y * game->width + x];
    if (!(*val & COVER_MASK))
    {
        return 1;
    }
    if (*val & FLAG_MASK)
    {
        game->placed_mines--;
        *val &= ~FLAG_MASK;
    }
    else
    {
        if (game->placed_mines == game->mines) return 1;
        game->placed_mines++;
        *val |= FLAG_MASK;
    }
    return game->mines - game->placed_mines;
}

int MSGame_check(MSGame_t *game)
{
    if (game->mines != game->placed_mines) return 1;
    for (int i = 0; i < game->height; i++)
    {
        for (int j = 0; j < game->width; j++)
        {
            char *val = &game->field[i * game->width + j];
            if (*val & FLAG_MASK) continue;
            if (*val & COVER_MASK) return 1;
        }
    }
    game->started = 2;
    return 0;
}

void _diff(char *cstate, char *pstate, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            char *val = &cstate[i * width + j];
            char *pval = &pstate[i * width + j];
            if (*val == *pval) continue;
            // printf("%d -> %d @ (%d, %d)\n", *pval, *val, j, i);
            if ((*val & FLAG_MASK) && ((*pval & FLAG_MASK) == 0))
            {
                printf("F %d %d\n", j, i);
                continue;
            }
            if ((*pval & FLAG_MASK) && ((*val & FLAG_MASK) == 0))
            {
                printf("C %d %d\n", j, i);
                continue;
            }
            if ((*pval & COVER_MASK) && ((*val & COVER_MASK) == 0))
            {
                printf("%d %d %d\n", (*val & MINE) ? 9 : *val & MASK, j, i);
            }
        }
    }
}

void MSGame_diff(MSGame_t *game)
{
    _diff(game->field, game->pfield, game->width, game->height);
}

void MSGame_state(MSGame_t *game)
{
    // Setup static blank field
    static char *bfield = NULL;
    if (bfield == NULL)
    {
        bfield = malloc(sizeof(char) * game->width * game->height);
        _clear(bfield, game->width, game->height);
    }

    _diff(game->field, bfield, game->width, game->height);
}
