/*
    $Id: main.c,v 1.3 1999/10/13 19:09:33 nik Exp nik $	
    
    tic tac 4 - tic tac 2 in a 4x4 field 
    Copyright (C) 1999 Niklas Olmes <tux.the.penguin@gmx.de>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.  

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
*/
/*
   TODO:  	- human vs CPU
		- CPU vs CPU (perhaps..)
		- resizeable field
		- X11 version
*/

#include <stdio.h>
#include <stdlib.h>

#include <curses.h>

#define BOARDH	26
#define BOARDV	13

#define WIN	1
#define LOOSE	-1

char field[16];
char board[BOARDH*BOARDV]=
//	"1234567890123456789012345"
	"+--1--+--2--+--3--+--4--+\n" // 2
	"|     |     |     |     |\n" // 3
	"|     |     |     |     |\n" // 4
	"+--5--+--6--+--7--+--8--+\n" // 5
	"|     |     |     |     |\n" // 6
	"|     |     |     |     |\n" // 7
	"+--9--+-10--+-11--+-12--+\n" // 8
	"|     |     |     |     |\n" // 9
	"|     |     |     |     |\n" // 10
	"+-13--+-14--+-15--+-16--+\n" // 11
	"|     |     |     |     |\n" // 12
	"|     |     |     |     |\n" // 13 
	"+-----+-----+-----+-----+\n";// 14

char xy[16][2]= {
	1,2,
	7,2,
	13,2,
	19,2,
	1,5,
	7,5,
	13,5,
	19,5,
	1,8,
	7,8,
	13,8,
	19,8,
	1,11,
	7,11,
	13,11,
	19,11
};
				
void init(void)
{
#ifdef ORIG_METHOD
	/* Basic */
	initscr();
	start_color();
	cbreak(); 
	noecho();
	/* Additional */
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	/* Refresh */
	refresh();
#else
	initscr();
	start_color();
	refresh();
	keypad(stdscr,TRUE);
	nocrmode();
	noecho();
	cbreak();
	refresh();
#endif
}

void deinit(void)
{
	 clear();
	 endwin();
}

void colorinit(void)
{
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_WHITE, COLOR_BLUE);
	init_pair(6, COLOR_CYAN, COLOR_BLUE);
	init_pair(7, COLOR_WHITE, COLOR_YELLOW);
	init_pair(8, COLOR_WHITE, COLOR_BLACK);
	init_pair(9, COLOR_YELLOW, COLOR_BLUE);
	init_pair(10, COLOR_RED, COLOR_BLUE);
}

void draw_main(void)
{
	attrset(COLOR_PAIR(7));
	mvaddstr(0,0,"Tic Tac 4 v1.0 - Tic Tac Toe on a 4x4 field");
	attrset(COLOR_PAIR(8));
	mvaddstr(22,0,"Copyright (C) 1999 Niklas Olmes <tux.the.penguin@gmx.de>");
	mvaddstr(23,0,"Licensed under GPL, see file COPYING for details.");
	mvaddstr(0,50,"  ");

}

void draw_field(void)
{
	short i;
	addch('\n');
	addch('\n');
	attrset(COLOR_PAIR(6));
	for ( i=0;i<BOARDV*BOARDH;i++ )	addch( board[i] );
}

void writex(short x, short y)
{
	attrset(COLOR_PAIR(9));
	attron(A_BOLD);
	mvaddch(y,x,'\\');
	mvaddch(y,x+4,'/');
	mvaddch(y+1,x+2,'x');
        mvaddch(y+2,x,'/');
	mvaddch(y+2,x+4,'\\');
}

void writeo(short x, short y)
{
	attrset(COLOR_PAIR(5));
	attron(A_BOLD);
	mvaddch(y,x,'/');
	mvaddch(y,x+1,' ');
	mvaddch(y,x+2,'~');
	mvaddch(y,x+3,' ');
	mvaddch(y,x+4,'\\');
	mvaddch(y+1,x,'|');
	mvaddch(y+1,x+4,'|');
	mvaddch(y+2,x,'\\');
	mvaddch(y+2,x+1,' ');
	mvaddch(y+2,x+2,'_');
	mvaddch(y+2,x+3,' ');
	mvaddch(y+2,x+4,'/');
}

int check(char player)
{
	int i=0;
	/* horizontal ??? */
	for(; i<4; i++)
		if (field[0+i*4]==player && field[1+i*4]==player && field[2+i*4]==player && field[3+i*4]==player)
			return WIN;
	/* vertical ??? */
	for(i=0; i<4; i++)
		if (field[0+i]==player && field[4+i]==player && field[8+i]==player && field[12+i]==player)
			return WIN;
	/* box ??? */
	for(i=0; i<11; i++)
		if (field[0+i]==player && field[1+i]==player && field[4+i]==player && field[5+i]==player)
			return WIN;
	/* diagonal ??? */
	if (field[0]==player && field[5]==player && field[10]==player && field[15]==player)
		return WIN;
	if (field[3]==player && field[6]==player && field[9]==player && field[12]==player)
		return WIN;
	/* special diagonal 3 ??? */
	if (field[2]==player && field[5]==player && field[8]==player)
		return WIN;
	if (field[7]==player && field[10]==player && field[13]==player)
		return WIN;
	if (field[1]==player && field[6]==player && field[11]==player)
		return WIN;
	if (field[4]==player && field[9]==player && field[14]==player)
		return WIN;
	/* ------------ */
	return 0;
}

int mainloop(void)
{
	char s[10], game=0, player='x';
	int n, t1, t2;
	
	while(game==0) {
		attrset(COLOR_PAIR(3));
		attron(A_BOLD);
		if (player=='x')
			mvaddstr(20,0, "-> It's Player's 'x' turn ");
		else {
			attrset(COLOR_PAIR(8));
			attron(A_BOLD);
			mvaddstr(20,3, "It's Player's 'o' turn ");
		}

		refresh();

		attrset(COLOR_PAIR(2));
		attron(A_BOLD);
		t1=getchar();
		mvaddch(18,1,t1);
		refresh();
		t2=getchar();
		mvaddch(18,2,t2);
		refresh();
		
		if(t2=='\r' && t1!=0 )
			n=t1-48;
		if(t1!=0 && t2!='\r' && t2!=0)
			n=(t1-48)*10+(t2-48);
		if(t1=='q')
			exit(0);

		if (n<17 && n>0 && field[n-1]==0) {
			field[n-1]=player;
			if (player=='x')
				writex(xy[n-1][0],xy[n-1][1]);
			else
				writeo(xy[n-1][0],xy[n-1][1]);
		} else {
			goto jump1;
		}

		if (check(player)==WIN) {
			attrset(COLOR_PAIR(9));
			attron(A_BOLD);
			mvaddstr(20, 0, "--> Congratulations! PLAYER '");
			addch(player);
			addstr("' WON. (press any key to quit)");
			refresh();
			getchar();
			return 0;
		}
		
		if (player=='x')
			player='o';
		else
			player='x';
jump1:
		attrset(COLOR_PAIR(2));
		attron(A_BOLD);
		mvaddstr(18,1,"          ");
		refresh();
	}
}
	
void draw_help(void)
{
	attrset(COLOR_PAIR(2));
	attron(A_BOLD);
	mvaddstr(2,45,"Commands:");
	attroff(A_BOLD);
	mvaddstr(3,45,"q <enter>\t\tquit");
	mvaddstr(4,45,"1...16 <enter>\tselect field");
	attron(A_BOLD);
	mvaddstr(6,45,"Rules:");
	attroff(A_BOLD);
	mvaddstr(7,45,"The player who has 4 vertical");
	mvaddstr(8,45,"horizontal or diagonal fields");
	mvaddstr(9,45,"or has the fields 3,6,9; 2,7,12;");
	mvaddstr(10,45,"8,11,14 or 5,10,15 wins the game.");
	mvaddstr(12,45,"One Player has 'o', the other 'x'.");
	mvaddstr(13,45,"\"Player 'x'\" starts and selects the");
	mvaddstr(14,45,"field where to put his 'x' on...");
}
			
int main(void)
{
	atexit(deinit);
	init();
	colorinit();
	draw_main();
	draw_field();
	draw_help();
	refresh();
	mainloop();
	return 0;
}
