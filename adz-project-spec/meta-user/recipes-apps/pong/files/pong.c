// Copyright 2020 Alexandru-Sergiu Marton <brown121407@member.fsf.org>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <ncurses.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 60
#define HEIGHT 20
#define PAD_SIZE 4
#define BALL "O"
#define PAD "#"
#define BLUE 1
#define RED 2
#define YELLOW 3
#define BALL_ATTRS A_BOLD | COLOR_PAIR(YELLOW)
#define PAD_P1_ATTRS A_BOLD | COLOR_PAIR(BLUE)
#define PAD_P2_ATTRS A_BOLD | COLOR_PAIR(RED)
#define PI 3.1415926535
#define DELTA 25000
#define UP -1
#define DOWN 1

enum game_state {
	GS_OK,
	GS_WIN_P1,
	GS_WIN_P2,
} gs = GS_OK;

struct vec2_d {
	double x, y;
};

struct vec2_i {
	int x, y;
};

struct ball {
	struct vec2_d pos;
	struct vec2_d dir;
	double angle;
} ball;

WINDOW* screen = NULL;
struct vec2_i p1_pad, p2_pad;
double velocity = 0.70;

int between(int low, int high, int x) {
	return x >= low && x <= high;
}

void set_ball_angle(double radians) {
	ball.dir.x = cos(radians);
	ball.dir.y = sin(radians);
	ball.angle = radians;
}

void draw_pad(int p) {
	struct vec2_i *pad = NULL;
	pad = (p == 1 ? &p1_pad : &p2_pad);

	p == 1 ? wattron(screen, PAD_P1_ATTRS) : wattron(screen, PAD_P2_ATTRS);
	for (int i = 0; i < PAD_SIZE; i++) {
		mvwprintw(screen, pad->y + i, pad->x, "%s", PAD);
	}
	p == 1 ? wattroff(screen, PAD_P1_ATTRS) : wattroff(screen, PAD_P2_ATTRS);
}

void move_pad(int p, int dir) {
	struct vec2_i *pad = NULL;
	pad = (p == 1 ? &p1_pad : &p2_pad);

	if (dir == UP)
		mvwprintw(screen, pad->y + PAD_SIZE - 1, pad->x, " ");
	else
		mvwprintw(screen, pad->y, pad->x, " ");

	pad->y += dir;
	if (pad->y <= 0)
		pad->y = 1;
	else if (pad->y >= HEIGHT - PAD_SIZE - 1)
		pad->y = HEIGHT - PAD_SIZE - 1;

	draw_pad(p);
}

void move_ball() {
	int collided_pad = FALSE, collided_wall = FALSE;
	struct vec2_d old_pos = ball.pos;

	ball.pos.x += ball.dir.x * velocity;
	ball.pos.y -= ball.dir.y * velocity;

	if ((int)ball.pos.y <= 0) {
		collided_wall = TRUE;
		ball.pos.y = 1.0;
	} else if ((int)ball.pos.y >= HEIGHT - 1) {
		collided_wall = TRUE;
		ball.pos.y = HEIGHT - 2;
	}
	
	if (collided_wall) {
		set_ball_angle(0 - ball.angle);
		gs = GS_OK;
		goto draw;
	}
	
	if (((int)ball.pos.x == p1_pad.x && between(p1_pad.y, p1_pad.y + PAD_SIZE - 1, ball.pos.y))) {
		collided_pad = TRUE;
		ball.pos.x = p1_pad.x + 1.0;
	} else if ((int)ball.pos.x == p2_pad.x && between(p2_pad.y, p2_pad.y + PAD_SIZE - 1, ball.pos.y)) {
		collided_pad = TRUE;
		ball.pos.x = p2_pad.x - 1.0;
	}

	if (collided_pad) {
		// Generate a random double between -0.45 and 0.15
		double r = ((double) rand() / RAND_MAX) * 0.6f - 0.45f;
		printf("r=%f\n", r);
		set_ball_angle(PI - ball.angle + r);
		gs = GS_OK;
		goto draw;
	}

	if ((int)ball.pos.x <= 0) {
		gs = GS_WIN_P2;
		return;
	} else if ((int)ball.pos.x >= WIDTH - 1) {
		gs = GS_WIN_P1;
		return;
	} else {
		gs = GS_OK;
	}

draw:
	mvwprintw(screen, (int)old_pos.y, (int)old_pos.x, " ");
	wattron(screen, BALL_ATTRS);
	mvwprintw(screen, (int)ball.pos.y, (int)ball.pos.x, "%s", BALL);
	wattroff(screen, BALL_ATTRS);
}

void run() {
	int should_run = TRUE, key;

	draw_pad(1);
	draw_pad(2);

	while (should_run) {
		key = wgetch(screen);
		switch (key) {
			case 'w': case 'W':
				move_pad(1, UP);
				break;
			case 's': case 'S':
				move_pad(1, DOWN);
				break;
			case KEY_UP:
				move_pad(2, UP);
				break;
			case KEY_DOWN:
				move_pad(2, DOWN);
				break;
			case 'q': case 'Q':
				should_run = FALSE;
				break;
		}
		move_ball();
		
		box(screen, 0, 0);
		wrefresh(screen);

		if (gs != GS_OK)
			should_run = false;

		usleep(DELTA);
	}
}

void setup() {
	srand(time(NULL));
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(FALSE);
	start_color();
	init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(RED, COLOR_RED, COLOR_BLACK);
	init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
	
	refresh();
	
	int max_x, max_y, screen_x, screen_y;
	getmaxyx(stdscr, max_y, max_x);

	screen_x = (max_x - WIDTH) / 2;
	screen_y = (max_y - HEIGHT) / 2;

	screen = newwin(HEIGHT, WIDTH, screen_y, screen_x);
	box(screen, 0, 0);
	nodelay(screen, TRUE);
	keypad(screen, TRUE);
	wrefresh(screen);

	p1_pad.x = 1.;
	p1_pad.y = 1.;
	p2_pad.x = WIDTH - 2.;
	p2_pad.y = 1.;
	ball.pos.x = (double)(WIDTH / 2);
	ball.pos.y = (double)(HEIGHT / 2);
	set_ball_angle(PI/3);
}

void cleanup() {
	endwin();
}

int main() {
	setup();
	run();
	cleanup();

	if (gs == GS_WIN_P1)
		puts("Player 1 wins!\n");
	else if (gs == GS_WIN_P2)
		puts("Player 2 wins!\n");
	else
		puts("Game ended.\n");
	return 0;
}
