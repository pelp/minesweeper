// Structure of the data
//
// C = Covered
// F = Flag
//
// | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
// | ------| C | F |  Surrounding  |
//

#define MINE 9
#define MASK 0xF
#define FLAG_MASK 0x10
#define COVER_MASK 0x20

typedef struct {
    char *field;
    char *pfield;
    int width, height;
    int mines;
    int placed_mines;
    int plays;
    char started;
} MSGame_t;

void MSGame_init(MSGame_t*, int, int, int);
int MSGame_flag(MSGame_t*, int, int);
int MSGame_peek(MSGame_t*, int, int);
void MSGame_print(MSGame_t*, char);
void MSGame_diff(MSGame_t*);
void MSGame_state(MSGame_t*);
int MSGame_check(MSGame_t*);
