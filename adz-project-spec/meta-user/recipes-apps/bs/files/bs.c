/*
 * bs.c - original author: Bruce Holloway
 *		salvo option by: Chuck A DeGaul
 * with improved user interface, autoconfiguration and code cleanup
 *
 * SPDX-FileCopyrightText: (C) Eric S. Raymond <esr@thyrsus.com>
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <assert.h>
#include <ctype.h>
#include <curses.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int getcoord(int);

/*
 * Constants for tuning the random-fire algorithm. It prefers moves that
 * diagonal-stripe the board with a stripe separation of srchstep. If
 * no such preferred moves are found, srchstep is decremented.
 */
#define BEGINSTEP 3 /* initial value of srchstep */

/* miscellaneous constants */
#define SHIPTYPES 5
#define OTHER (1 - turn)
#define PLAYER 0
#define COMPUTER 1
#define MARK_HIT 'H'
#define MARK_MISS 'o'
#define CTRLC '\003' /* used as terminate command */
#define FF '\014'    /* used as redraw command */

/* coordinate handling */
#define BWIDTH 10
#define BDEPTH 10

/* display symbols */
#define SHOWHIT '*'
#define SHOWSPLASH ' '
#define IS_SHIP(c) (isupper(c) ? true : false)

/* how to position us on player board */
#define PYBASE 3
#define PXBASE 3
#define PY(y) (PYBASE + (y))
#define PX(x) (PXBASE + (x)*3)
#define pgoto(y, x) (void)move(PY(y), PX(x))

/* how to position us on cpu board */
#define CYBASE 3
#define CXBASE 48
#define CY(y) (CYBASE + (y))
#define CX(x) (CXBASE + (x)*3)
#define CYINV(y) ((y)-CYBASE)
#define CXINV(x) (((x)-CXBASE) / 3)
#define cgoto(y, x) (void)move(CY(y), CX(x))

#define ONBOARD(x, y) (x >= 0 && x < BWIDTH && y >= 0 && y < BDEPTH)

/* other board locations */
#define COLWIDTH 80
#define PROMPTLINE 21              /* prompt line */
#define SYBASE CYBASE + BDEPTH + 3 /* move key diagram */
#define SXBASE 63
#define MYBASE SYBASE - 1 /* diagram caption */
#define MXBASE 64
#define HYBASE SYBASE - 1 /* help area */
#define HXBASE 0

/* this will need to be changed if BWIDTH changes */
static const char numbers[] = "   0  1  2  3  4  5  6  7  8  9";

static const char carrier[] = "Aircraft Carrier";
static const char battle[] = "Battleship";
static const char sub[] = "Submarine";
static const char destroy[] = "Destroyer";
static const char ptboat[] = "PT Boat";

static char name[40];
static const char dftname[] = "stranger";

/* direction constants */
#define E 0
#define SE 1
#define S 2
#define SW 3
#define W 4
#define NW 5
#define N 6
#define NE 7
static const int xincr[8] = {1, 1, 0, -1, -1, -1, 0, 1};
static const int yincr[8] = {0, 1, 1, 1, 0, -1, -1, -1};

/* current ship position and direction */
static int curx = (BWIDTH / 2);
static int cury = (BDEPTH / 2);

typedef struct {
	const char *name;  /* name of the ship type */
	int hits;          /* how many times has this ship been hit? */
	char symbol;       /* symbol for game purposes */
	int length;        /* length of ship */
	char x, y;         /* coordinates of ship start point */
	unsigned char dir; /* direction of `bow' */
	bool placed;       /* has it been placed on the board? */
} ship_t;

static bool checkplace(int b, ship_t *ss, int vis);

static ship_t plyship[SHIPTYPES] = {
    {carrier, 0, 'A', 5}, {battle, 0, 'B', 4}, {destroy, 0, 'D', 3},
    {sub, 0, 'S', 3},     {ptboat, 0, 'P', 2},
};

static ship_t cpuship[SHIPTYPES] = {
    {carrier, 0, 'A', 5}, {battle, 0, 'B', 4}, {destroy, 0, 'D', 3},
    {sub, 0, 'S', 3},     {ptboat, 0, 'P', 2},
};

/* "Hits" board, and main board. */
static char hits[2][BWIDTH][BDEPTH];
static char board[2][BWIDTH][BDEPTH];

static int turn;                   /* 0=player, 1=computer */
static int plywon = 0, cpuwon = 0; /* How many games has each won? */

static bool salvo, blitz, closepack;

#define PR (void)addstr

static void uninitgame(int sig GCC_UNUSED)
/* end the game, either normally or due to signal */
{
	clear();
	(void)refresh();
	(void)resetterm();
	(void)echo();
	(void)endwin();
	exit(EXIT_FAILURE);
}

static void announceopts(void) {
	/* announce which game options are enabled */
	if (salvo || blitz || closepack) {
		(void)printw("Playing optional game (");
		if (salvo) {
			(void)printw("salvo, ");
		} else {
			(void)printw("nosalvo, ");
		}
		if (blitz) {
			(void)printw("blitz ");
		} else {
			(void)printw("noblitz, ");
		}
		if (closepack) {
			(void)printw("closepack)");
		} else {
			(void)printw("noclosepack)");
		}
	} else {
		(void)printw(
		    "Playing standard game (noblitz, nosalvo, noclosepack)");
	}
}

static void intro(void) {
	char *tmpname;

	srand((unsigned)(time(0L) +
	                 getpid())); /* Kick the random number generator */

	(void)signal(SIGINT, uninitgame);
	(void)signal(SIGINT, uninitgame);
	(void)signal(SIGABRT, uninitgame); /* for assert(3) */
	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN) {
		(void)signal(SIGQUIT, uninitgame);
	}

	if ((tmpname = getlogin()) != 0) {
		(void)strcpy(name, tmpname);
		name[0] = toupper(name[0]);
	} else {
		(void)strcpy(name, dftname);
	}

	(void)initscr();
#ifdef KEY_MIN
	keypad(stdscr, true);
#endif /* KEY_MIN */
	(void)saveterm();
	(void)nonl();
	(void)cbreak();
	(void)noecho();

#ifdef PENGUIN
	(void)clear();
	(void)mvaddstr(4, 29, "Welcome to Battleship!");
	// clang-format off
	(void)move(8,0);
	PR("                                                  \\\n");
	PR("                           \\                     \\ \\\n");
	PR("                          \\ \\                   \\ \\ \\_____________\n");
	PR("                         \\ \\ \\_____________      \\ \\/            |\n");
	PR("                          \\ \\/             \\      \\/             |\n");
	PR("                           \\/               \\_____/              |__\n");
	PR("           ________________/                                       |\n");
	PR("           \\  S.S. Penguin                                         |\n");
	PR("            \\                                                     /\n");
	PR("             \\___________________________________________________/\n");
	// clang-format on
	(void)mvaddstr(22, 27, "Hit any key to continue...");
	(void)refresh();
	(void)getch();
#endif /* PENGUIN */

#ifdef A_COLOR
	start_color();

	init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
#endif /* A_COLOR */

#ifdef NCURSES_MOUSE_VERSION
	(void)mousemask(BUTTON1_CLICKED, (mmask_t *)NULL);
#endif /* NCURSES_MOUSE_VERSION*/
}

/* VARARGS1 */
static void prompt(int n, NCURSES_CONST char *f, const char *s) {
	/* print a message at the prompt line */
	(void)move(PROMPTLINE + n, 0);
	(void)clrtoeol();
	(void)printw(f, s);
	(void)refresh();
}

static void error(NCURSES_CONST char *s) {
	(void)move(PROMPTLINE + 2, 0);
	(void)clrtoeol();
	if (s) {
		(void)addstr(s);
		(void)beep();
	}
}

static void placeship(int b, ship_t *ss, int vis) {
	int l;

	for (l = 0; l < ss->length; ++l) {
		int newx = ss->x + l * xincr[ss->dir];
		int newy = ss->y + l * yincr[ss->dir];

		board[b][newx][newy] = ss->symbol;
		if (vis) {
			pgoto(newy, newx);
			(void)addch((chtype)ss->symbol);
		}
	}
	ss->hits = 0;
}

static int rnd(int n) { return (((rand() & 0x7FFF) % n)); }

static void randomplace(int b, ship_t *ss) {
	/* generate a valid random ship placement into px,py */

	do {
		ss->dir = rnd(2) ? E : S;
		ss->x = rnd(BWIDTH - (ss->dir == E ? ss->length : 0));
		ss->y = rnd(BDEPTH - (ss->dir == S ? ss->length : 0));
	} while (!checkplace(b, ss, false));
}

static void initgame(void) {
	int i, j, unplaced;
	ship_t *ss;

	(void)clear();
	(void)mvaddstr(0, 35, "BATTLESHIPS");
	(void)move(PROMPTLINE + 2, 0);
	announceopts();

	memset(board, 0, sizeof(char) * BWIDTH * BDEPTH * 2);
	memset(hits, 0, sizeof(char) * BWIDTH * BDEPTH * 2);
	for (i = 0; i < SHIPTYPES; i++) {
		ss = cpuship + i;

		ss->x = ss->y = ss->dir = ss->hits = 0;
		ss->placed = false;

		ss = plyship + i;

		ss->x = ss->y = ss->dir = ss->hits = 0;
		ss->placed = false;
	}

	/* draw empty boards */
	(void)mvaddstr(PYBASE - 2, PXBASE + 5, "Main Board");
	(void)mvaddstr(PYBASE - 1, PXBASE - 3, numbers);
	for (i = 0; i < BDEPTH; ++i) {
		(void)mvaddch(PYBASE + i, PXBASE - 3, (chtype)(i + 'A'));
#ifdef A_COLOR
		if (has_colors()) {
			attron(COLOR_PAIR(COLOR_BLUE));
		}
#endif /* A_COLOR */
		(void)addch(' ');
		for (j = 0; j < BWIDTH; j++) {
			(void)addstr(" . ");
		}
#ifdef A_COLOR
		attrset(0);
#endif /* A_COLOR */
		(void)addch(' ');
		(void)addch((chtype)(i + 'A'));
	}
	(void)mvaddstr(PYBASE + BDEPTH, PXBASE - 3, numbers);
	(void)mvaddstr(CYBASE - 2, CXBASE + 7, "Hit/Miss Board");
	(void)mvaddstr(CYBASE - 1, CXBASE - 3, numbers);
	for (i = 0; i < BDEPTH; ++i) {
		(void)mvaddch(CYBASE + i, CXBASE - 3, (chtype)(i + 'A'));
#ifdef A_COLOR
		if (has_colors()) {
			attron(COLOR_PAIR(COLOR_BLUE));
		}
#endif /* A_COLOR */
		(void)addch(' ');
		for (j = 0; j < BWIDTH; j++) {
			(void)addstr(" . ");
		}
#ifdef A_COLOR
		attrset(0);
#endif /* A_COLOR */
		(void)addch(' ');
		(void)addch((chtype)(i + 'A'));
	}

	(void)mvaddstr(CYBASE + BDEPTH, CXBASE - 3, numbers);

	(void)mvprintw(
	    HYBASE, HXBASE,
	    "To position your ships: move the cursor to a spot, then");
	(void)mvprintw(
	    HYBASE + 1, HXBASE,
	    "type the first letter of a ship type to select it, then");
	(void)mvprintw(
	    HYBASE + 2, HXBASE,
	    "type a direction ([hjkl] or [4862]), indicating how the");
	(void)mvprintw(
	    HYBASE + 3, HXBASE,
	    "ship should be pointed. You may also type a ship letter");
	(void)mvprintw(
	    HYBASE + 4, HXBASE,
	    "followed by `r' to position it randomly, or type `R' to");
	(void)mvprintw(HYBASE + 5, HXBASE,
	               "place all remaining ships randomly.");

	(void)mvaddstr(MYBASE, MXBASE, "Aiming keys:");
	(void)mvaddstr(SYBASE, SXBASE, "y k u    7 8 9");
	(void)mvaddstr(SYBASE + 1, SXBASE, " \\|/      \\|/ ");
	(void)mvaddstr(SYBASE + 2, SXBASE, "h-+-l    4-+-6");
	(void)mvaddstr(SYBASE + 3, SXBASE, " /|\\      /|\\ ");
	(void)mvaddstr(SYBASE + 4, SXBASE, "b j n    1 2 3");

	/* have the computer place ships */
	for (ss = cpuship; ss < cpuship + SHIPTYPES; ss++) {
		randomplace(COMPUTER, ss);
		placeship(COMPUTER, ss, false);
	}

	ss = (ship_t *)NULL;
	do {
		char c, docked[SHIPTYPES + 2], *cp = docked;

		/* figure which ships still wait to be placed */
		*cp++ = 'R';
		for (i = 0; i < SHIPTYPES; i++) {
			if (!plyship[i].placed) {
				*cp++ = plyship[i].symbol;
			}
		}
		*cp = '\0';

		/* get a command letter */
		prompt(1, "Type one of [%s] to pick a ship.", docked + 1);
		do {
			c = toupper(getcoord(PLAYER));
		} while (!strchr(docked, c));

		if (c == 'R') {
			(void)ungetch('R');
		} else {
			/* map that into the corresponding symbol */
			for (ss = plyship; ss < plyship + SHIPTYPES; ss++) {
				if (ss->symbol == c) {
					break;
				}
			}

			prompt(1, "Type one of [hjklrR] to place your %s.",
			       ss->name);
			pgoto(cury, curx);
		}

		do {
			c = getch();
		} while (!strchr("hjklrR", c) || c == FF);

		if (c == FF) {
			(void)clearok(stdscr, true);
			(void)refresh();
		} else if (c == 'r') {
			prompt(1, "Random-placing your %s", ss->name);
			randomplace(PLAYER, ss);
			placeship(PLAYER, ss, true);
			error((char *)NULL);
			ss->placed = true;
		} else if (c == 'R') {
			prompt(1, "Placing the rest of your fleet at random...",
			       "");
			for (ss = plyship; ss < plyship + SHIPTYPES; ss++) {
				if (!ss->placed) {
					randomplace(PLAYER, ss);
					placeship(PLAYER, ss, true);
					ss->placed = true;
				}
			}
			error((char *)NULL);
		} else if (strchr("hjkl8462", c)) {
			ss->x = curx;
			ss->y = cury;

			switch (c) {
			case 'k':
			case '8':
				ss->dir = N;
				break;
			case 'j':
			case '2':
				ss->dir = S;
				break;
			case 'h':
			case '4':
				ss->dir = W;
				break;
			case 'l':
			case '6':
				ss->dir = E;
				break;
			}

			if (checkplace(PLAYER, ss, true)) {
				placeship(PLAYER, ss, true);
				error((char *)NULL);
				ss->placed = true;
			}
		}

		for (unplaced = i = 0; i < SHIPTYPES; i++) {
			unplaced += !plyship[i].placed;
		}
	} while (unplaced);

	turn = rnd(2);

	(void)mvprintw(
	    HYBASE, HXBASE,
	    "To fire, move the cursor to your chosen aiming point   ");
	(void)mvprintw(
	    HYBASE + 1, HXBASE,
	    "and strike any key other than a motion key.            ");
	(void)mvprintw(
	    HYBASE + 2, HXBASE,
	    "                                                       ");
	(void)mvprintw(
	    HYBASE + 3, HXBASE,
	    "                                                       ");
	(void)mvprintw(
	    HYBASE + 4, HXBASE,
	    "                                                       ");
	(void)mvprintw(
	    HYBASE + 5, HXBASE,
	    "                                                       ");

	(void)prompt(0, "Press any key to start...", "");
	(void)getch();
}

static int getcoord(int atcpu) {
	if (atcpu) {
		cgoto(cury, curx);
	} else {
		pgoto(cury, curx);
	}
	(void)refresh();
	for (;;) {
		int ny, nx, c;

		if (atcpu) {
			(void)mvprintw(CYBASE + BDEPTH + 1, CXBASE + 11,
			               "(%d, %c)", curx, 'A' + cury);
			cgoto(cury, curx);
		} else {
			(void)mvprintw(PYBASE + BDEPTH + 1, PXBASE + 11,
			               "(%d, %c)", curx, 'A' + cury);
			pgoto(cury, curx);
		}

		switch (c = getch()) {
		case 'q':
		case 27:
			uninitgame(0);
		case 'k':
		case '8':
#ifdef KEY_MIN
		case KEY_UP:
#endif /* KEY_MIN */
			ny = cury + BDEPTH - 1;
			nx = curx;
			break;
		case 'j':
		case '2':
#ifdef KEY_MIN
		case KEY_DOWN:
#endif /* KEY_MIN */
			ny = cury + 1;
			nx = curx;
			break;
		case 'h':
		case '4':
#ifdef KEY_MIN
		case KEY_LEFT:
#endif /* KEY_MIN */
			ny = cury;
			nx = curx + BWIDTH - 1;
			break;
		case 'l':
		case '6':
#ifdef KEY_MIN
		case KEY_RIGHT:
#endif /* KEY_MIN */
			ny = cury;
			nx = curx + 1;
			break;
		case 'y':
		case '7':
#ifdef KEY_MIN
		case KEY_A1:
#endif /* KEY_MIN */
			ny = cury + BDEPTH - 1;
			nx = curx + BWIDTH - 1;
			break;
		case 'b':
		case '1':
#ifdef KEY_MIN
		case KEY_C1:
#endif /* KEY_MIN */
			ny = cury + 1;
			nx = curx + BWIDTH - 1;
			break;
		case 'u':
		case '9':
#ifdef KEY_MIN
		case KEY_A3:
#endif /* KEY_MIN */
			ny = cury + BDEPTH - 1;
			nx = curx + 1;
			break;
		case 'n':
		case '3':
#ifdef KEY_MIN
		case KEY_C3:
#endif /* KEY_MIN */
			ny = cury + 1;
			nx = curx + 1;
			break;
		case FF:
			nx = curx;
			ny = cury;
			(void)clearok(stdscr, true);
			(void)refresh();
			break;
#ifdef NCURSES_MOUSE_VERSION
		case KEY_MOUSE: {
			MEVENT myevent;

			getmouse(&myevent);
			if (atcpu && myevent.y >= CY(0) &&
			    myevent.y <= CY(BDEPTH) && myevent.x >= CX(0) &&
			    myevent.x <= CX(BDEPTH)) {
				curx = CXINV(myevent.x);
				cury = CYINV(myevent.y);
				return (' ');
			} else {
				beep();
				continue;
			}
		}
		/* no fall through */
#endif /* NCURSES_MOUSE_VERSION */

		default:
			if (atcpu) {
				(void)mvaddstr(CYBASE + BDEPTH + 1, CXBASE + 11,
				               "      ");
			} else {
				(void)mvaddstr(PYBASE + BDEPTH + 1, PXBASE + 11,
				               "      ");
			}
			return (c);
		}

		curx = nx % BWIDTH;
		cury = ny % BDEPTH;
	}
}

static bool collidecheck(int b, int y, int x) {
	/* is this location on the selected zboard adjacent to a ship? */
	bool collide;

	/* anything on the square */
	if ((collide = IS_SHIP(board[b][x][y])) != false) {
		return (collide);
	}

	/* anything on the neighbors */
	if (!closepack) {
		int i;

		for (i = 0; i < 8; i++) {
			int xend, yend;

			yend = y + yincr[i];
			xend = x + xincr[i];
			if (ONBOARD(xend, yend) &&
			    IS_SHIP(board[b][xend][yend])) {
				collide = true;
				break;
			}
		}
	}
	return (collide);
}

static bool checkplace(int b, ship_t *ss, int vis) {
	int l, xend, yend;

	/* first, check for board edges */
	xend = ss->x + (ss->length - 1) * xincr[ss->dir];
	yend = ss->y + (ss->length - 1) * yincr[ss->dir];
	if (!ONBOARD(xend, yend)) {
		if (vis) {
			switch (rnd(3)) {
			case 0:
				error("Ship is hanging from the edge of the "
				      "world");
				break;
			case 1:
				error("Try fitting it on the board");
				break;
			case 2:
				error("Figure I won't find it if you put it "
				      "there?");
				break;
			}
		}
		return (false);
	}

	for (l = 0; l < ss->length; ++l) {
		if (collidecheck(b, ss->y + l * yincr[ss->dir],
		                 ss->x + l * xincr[ss->dir])) {
			if (vis) {
				switch (rnd(3)) {
				case 0:
					error("There's already a ship there");
					break;
				case 1:
					error("Collision alert!  Aaaaaagh!");
					break;
				case 2:
					error("Er, Admiral, what about the "
					      "other ship?");
					break;
				}
			}
			return (false);
		}
	}
	return (true);
}

static int awinna(void) {
	int i, j;

	for (i = 0; i < 2; ++i) {
		ship_t *ss = (i) ? cpuship : plyship;
		for (j = 0; j < SHIPTYPES; ++j, ++ss) {
			if (ss->length > ss->hits) {
				break;
			}
		}
		if (j == SHIPTYPES) {
			return (OTHER);
		}
	}
	return (-1);
}

static ship_t *hitship(int x, int y) {
	/* register a hit on the targeted ship */
	ship_t *sb, *ss;
	char sym;
	int oldx, oldy;

	getyx(stdscr, oldy, oldx);
	sb = (turn) ? plyship : cpuship;
	if ((sym = board[OTHER][x][y]) == 0) {
		return ((ship_t *)NULL);
	}
	for (ss = sb; ss < sb + SHIPTYPES; ++ss) {
		if (ss->symbol == sym) {
			if (++ss->hits < ss->length) { /* still afloat? */
				return ((ship_t *)NULL);
			} else { /* sunk! */
				int i;

				if (!closepack) {
					int j;

					for (j = -1; j <= 1; j++) {
						int bx =
						    ss->x +
						    j * xincr[(ss->dir + 2) %
						              8];
						int by =
						    ss->y +
						    j * yincr[(ss->dir + 2) %
						              8];

						for (i = -1; i <= ss->length;
						     ++i) {
							int x1, y1;

							x1 = bx +
							     i * xincr[ss->dir];
							y1 = by +
							     i * yincr[ss->dir];
							if (ONBOARD(x1, y1)) {
								hits[turn][x1]
								    [y1] =
								        MARK_MISS;
								if (turn % 2 ==
								    PLAYER) {
									cgoto(
									    y1,
									    x1);
#ifdef A_COLOR
									if (has_colors()) {
										attron(COLOR_PAIR(
										    COLOR_GREEN));
									}
#endif /* A_COLOR */
									(void)addch(
									    MARK_MISS);
#ifdef A_COLOR
									attrset(
									    0);
#endif /* A_COLOR */
								} else {
									pgoto(
									    y1,
									    x1);
									(void)addch(
									    SHOWSPLASH);
								}
							}
						}
					}
				}

				for (i = 0; i < ss->length; ++i) {
					int x1 = ss->x + i * xincr[ss->dir];
					int y1 = ss->y + i * yincr[ss->dir];

					hits[turn][x1][y1] = ss->symbol;
					if (turn % 2 == PLAYER) {
						cgoto(y1, x1);
						(void)addch(
						    (chtype)(ss->symbol));
					} else {
						pgoto(y1, x1);
#ifdef A_COLOR
						if (has_colors()) {
							attron(COLOR_PAIR(
							    COLOR_RED));
						}
#endif /* A_COLOR */
						(void)addch(SHOWHIT);
#ifdef A_COLOR
						attrset(0);
#endif /* A_COLOR */
					}
				}

				(void)move(oldy, oldx);
				return (ss);
			}
		}
	}
	(void)move(oldy, oldx);
	return ((ship_t *)NULL);
}

static int plyturn(void) {
	ship_t *ss;
	bool hit;
	NCURSES_CONST char *m = NULL;

	prompt(1, "Where do you want to shoot? ", "");
	for (;;) {
		(void)getcoord(COMPUTER);
		if (hits[PLAYER][curx][cury]) {
			prompt(1, "You shelled this spot already! Try again.",
			       "");
			beep();
		} else {
			break;
		}
	}
	hit = IS_SHIP(board[COMPUTER][curx][cury]);
	hits[PLAYER][curx][cury] = (hit ? MARK_HIT : MARK_MISS);
	cgoto(cury, curx);
#ifdef A_COLOR
	if (has_colors()) {
		if (hit) {
			attron(COLOR_PAIR(COLOR_RED));
		} else {
			attron(COLOR_PAIR(COLOR_GREEN));
		}
	}
#endif /* A_COLOR */
	(void)addch((chtype)hits[PLAYER][curx][cury]);
#ifdef A_COLOR
	attrset(0);
#endif /* A_COLOR */

	prompt(1, "You %s.", hit ? "scored a hit" : "missed");
	if (hit && (ss = hitship(curx, cury))) {
		switch (rnd(5)) {
		case 0:
			m = " You sank my %s!";
			break;
		case 1:
			m = " I have this sinking feeling about my %s....";
			break;
		case 2:
			m = " My %s has gone to Davy Jones's locker!";
			break;
		case 3:
			m = " Glub, glub -- my %s is headed for the bottom!";
			break;
		case 4:
			m = " You'll pick up survivors from my %s, I hope...!";
			break;
		}
		(void)printw(m, ss->name);
		(void)beep();
		return (awinna() == -1);
	}
	return (hit);
}

static int sgetc(const char *s) {
	const char *s1;

	(void)refresh();
	for (;;) {
		int ch = getch();
		if (islower(ch)) {
			ch = toupper(ch);
		}
		if (ch == CTRLC || ch == 27 || ch == 'Q') {
			uninitgame(0);
		}
		for (s1 = s; *s1 && ch != *s1; ++s1) {
			continue;
		}
		if (*s1) {
			(void)addch((chtype)ch);
			(void)refresh();
			return (ch);
		}
	}
}

static void randomfire(int *px, int *py) {
	/* random-fire routine -- implements simple diagonal-striping strategy
	 */
	static int turncount = 0;
	static int srchstep = BEGINSTEP;
	static int huntoffs; /* Offset on search strategy */
	int ypossible[BWIDTH * BDEPTH], xpossible[BWIDTH * BDEPTH], nposs;
	int ypreferred[BWIDTH * BDEPTH], xpreferred[BWIDTH * BDEPTH], npref;
	int x, y, i;

	if (turncount++ == 0) {
		huntoffs = rnd(srchstep);
	}

	/* first, list all possible moves */
	nposs = npref = 0;
	for (x = 0; x < BWIDTH; x++) {
		for (y = 0; y < BDEPTH; y++) {
			if (!hits[COMPUTER][x][y]) {
				xpossible[nposs] = x;
				ypossible[nposs] = y;
				nposs++;
				if (((x + huntoffs) % srchstep) !=
				    (y % srchstep)) {
					xpreferred[npref] = x;
					ypreferred[npref] = y;
					npref++;
				}
			}
		}
	}

	if (npref) {
		i = rnd(npref);

		*px = xpreferred[i];
		*py = ypreferred[i];
	} else if (nposs) {
		i = rnd(nposs);

		*px = xpossible[i];
		*py = ypossible[i];

		if (srchstep > 1) {
			--srchstep;
		}
	} else {
		error("No moves possible?? Help!");
		exit(EXIT_FAILURE);
		/*NOTREACHED*/
	}
}

#define S_MISS 0
#define S_HIT 1
#define S_SUNK -1

static int cpufire(int x, int y) {
	/* fire away at given location */
	bool hit, sunk;
	ship_t *ss = NULL;

	hits[COMPUTER][x][y] =
	    (hit = (board[PLAYER][x][y])) ? MARK_HIT : MARK_MISS;
	(void)mvprintw(PROMPTLINE, 0, "I shoot at %c%d. I %s!", y + 'A', x,
	               hit ? "hit" : "miss");
	if ((sunk = (hit && (ss = hitship(x, y)))) != 0) {
		(void)printw(" I've sunk your %s", ss->name);
	}
	(void)clrtoeol();

	pgoto(y, x);
#ifdef A_COLOR
	if (has_colors()) {
		if (hit) {
			attron(COLOR_PAIR(COLOR_RED));
		} else {
			attron(COLOR_PAIR(COLOR_GREEN));
		}
	}
#endif /* A_COLOR */
	(void)addch((chtype)(hit ? SHOWHIT : SHOWSPLASH));
#ifdef A_COLOR
	attrset(0);
#endif /* A_COLOR */

	return ((hit ? (sunk ? S_SUNK : S_HIT) : S_MISS) ? true : false);
}

/*
 * This code implements a fairly irregular FSM, so please forgive the rampant
 * unstructuredness below. The five labels are states which need to be held
 * between computer turns.
 */
static bool cputurn(void) {
#define POSSIBLE(x, y) (ONBOARD(x, y) && !hits[COMPUTER][x][y])
#define RANDOM_FIRE 0
#define RANDOM_HIT 1
#define HUNT_DIRECT 2
#define FIRST_PASS 3
#define REVERSE_JUMP 4
#define SECOND_PASS 5
	static int next = RANDOM_FIRE;
	static bool used[4];
	static ship_t ts;
	int navail, x, y, d, n;
	int hit = S_MISS;

	switch (next) {
	case RANDOM_FIRE: /* last shot was random and missed */
	refire:
		randomfire(&x, &y);
		if (!(hit = cpufire(x, y))) {
			next = RANDOM_FIRE;
		} else {
			ts.x = x;
			ts.y = y;
			ts.hits = 1;
			next = (hit == S_SUNK) ? RANDOM_FIRE : RANDOM_HIT;
		}
		break;

	case RANDOM_HIT: /* last shot was random and hit */
		used[E / 2] = used[S / 2] = used[W / 2] = used[N / 2] = false;
		/* FALLTHROUGH */

	case HUNT_DIRECT: /* last shot hit, we're looking for ship's long axis
	                   */
		for (d = navail = 0; d < 4; d++) {
			x = ts.x + xincr[d * 2];
			y = ts.y + yincr[d * 2];
			if (!used[d] && POSSIBLE(x, y)) {
				navail++;
			} else {
				used[d] = true;
			}
		}
		if (navail == 0) /* no valid places for shots adjacent... */ {
			goto refire; /* ...so we must random-fire */
		} else {
			for (d = 0, n = rnd(navail) + 1; n; n--) {
				while (used[d]) {
					d++;
				}
			}

			assert(d <= 4);

			used[d] = false;
			x = ts.x + xincr[d * 2];
			y = ts.y + yincr[d * 2];

			assert(POSSIBLE(x, y));

			if (!(hit = cpufire(x, y))) {
				next = HUNT_DIRECT;
			} else {
				ts.x = x;
				ts.y = y;
				ts.dir = d * 2;
				ts.hits++;
				next =
				    (hit == S_SUNK) ? RANDOM_FIRE : FIRST_PASS;
			}
		}
		break;

	case FIRST_PASS: /* we have a start and a direction now */
		x = ts.x + xincr[ts.dir];
		y = ts.y + yincr[ts.dir];
		if (POSSIBLE(x, y) && (hit = cpufire(x, y))) {
			ts.x = x;
			ts.y = y;
			ts.hits++;
			next = (hit == S_SUNK) ? RANDOM_FIRE : FIRST_PASS;
		} else {
			next = REVERSE_JUMP;
		}
		break;

	case REVERSE_JUMP: /* nail down the ship's other end */
		d = ts.dir + 4;
		x = ts.x + ts.hits * xincr[d];
		y = ts.y + ts.hits * yincr[d];
		if (POSSIBLE(x, y) && (hit = cpufire(x, y))) {
			ts.x = x;
			ts.y = y;
			ts.dir = d;
			ts.hits++;
			next = (hit == S_SUNK) ? RANDOM_FIRE : SECOND_PASS;
		} else {
			next = RANDOM_FIRE;
		}
		break;

	case SECOND_PASS: /* kill squares not caught on first pass */
		x = ts.x + xincr[ts.dir];
		y = ts.y + yincr[ts.dir];
		if (POSSIBLE(x, y) && (hit = cpufire(x, y))) {
			ts.x = x;
			ts.y = y;
			ts.hits++;
			next = (hit == S_SUNK) ? RANDOM_FIRE : SECOND_PASS;
			break;
		} else {
			next = RANDOM_FIRE;
		}
		break;
	}

	/* check for continuation and/or winner */
	if (salvo) {
		(void)refresh();
		(void)sleep(1);
	}
	if (awinna() != -1) {
		return (false);
	}

#ifdef DEBUG
	(void)mvprintw(PROMPTLINE + 2, 0, "New state %d, x=%d, y=%d, d=%d",
	               next, x, y, d);
#endif /* DEBUG */
	return ((hit) ? true : false);
}

static int playagain(void) {
	int j;
	ship_t *ss;

	for (ss = cpuship; ss < cpuship + SHIPTYPES; ss++) {
		for (j = 0; j < ss->length; j++) {
			cgoto(ss->y + j * yincr[ss->dir],
			      ss->x + j * xincr[ss->dir]);
			(void)addch((chtype)ss->symbol);
		}
	}

	if (awinna()) {
		++cpuwon;
	} else {
		++plywon;
	}
	j = 18 + strlen(name);
	if (plywon >= 10) {
		++j;
	}
	if (cpuwon >= 10) {
		++j;
	}
	(void)mvprintw(1, (COLWIDTH - j) / 2, "%s: %d     Computer: %d", name,
	               plywon, cpuwon);

	prompt(2,
	       (awinna()) ? "Want to be humiliated again, %s [yn]? "
	                  : "Going to give me a chance for revenge, %s [yn]? ",
	       name);
	return (sgetc("YN") == 'Y');
}

static void do_options(int c, char *op[]) {
	if (c > 1) {
		register int i;

		for (i = 1; i < c; i++) {
			switch (op[i][0]) {
			default:
			case '?':
				(void)fprintf(stderr,
				              "Usage: bs [-s | -b] [-c]\n");
				(void)fprintf(stderr,
				              "\tWhere the options are:\n");
				(void)fprintf(stderr,
				              "\t-s : play a salvo game\n");
				(void)fprintf(stderr,
				              "\t-b : play a blitz game\n");
				(void)fprintf(stderr,
				              "\t-c : ships may be adjacent\n");
				exit(EXIT_FAILURE);
				break;
			case '-':
				switch (op[i][1]) {
				case 'b':
					blitz = true;
					if (salvo) {
						(void)fprintf(
						    stderr,
						    "Bad Arg: -b and -s are "
						    "mutually exclusive\n");
						exit(EXIT_FAILURE);
					}
					break;
				case 's':
					salvo = true;
					if (blitz) {
						(void)fprintf(
						    stderr,
						    "Bad Arg: -s and -b are "
						    "mutually exclusive\n");
						exit(EXIT_FAILURE);
					}
					break;
				case 'c':
					closepack = true;
					break;
				case 'v':
					printf("bs %s\n", VERSION);
					exit(0);
				default:
					(void)fprintf(stderr,
					              "Bad arg: type \"%s ?\" "
					              "for usage message\n",
					              op[0]);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
}

static int scount(int who) {
	register int i, shots;
	register ship_t *sp;

	if (who) {
		sp = cpuship; /* count cpu shots */
	} else {
		sp = plyship; /* count player shots */
	}

	for (i = 0, shots = 0; i < SHIPTYPES; i++, sp++) {
		if (sp->hits >= sp->length) {
			continue; /* dead ship */
		} else {
			shots++;
		}
	}
	return (shots);
}

int main(int argc, char *argv[]) {
	do_options(argc, argv);

	intro();
	do {
		initgame();
		while (awinna() == -1) {
			if (!blitz) {
				if (!salvo) {
					if (turn) {
						(void)cputurn();
					} else {
						(void)plyturn();
					}
				} else {
					register int i;

					i = scount(turn);
					while (i--) {
						if (turn) {
							if (cputurn() &&
							    awinna() != -1) {
								i = 0;
							}
						} else {
							if (plyturn() &&
							    awinna() != -1) {
								i = 0;
							}
						}
					}
				}
			} else {
				while (turn ? cputurn() : plyturn()) {
					continue;
				}
			}
			turn = OTHER;
		}
	} while (playagain());
	uninitgame(0);
	/*NOTREACHED*/
	return 0;
}

/* bs.c ends here */
