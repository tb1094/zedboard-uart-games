#
#    $Id: Makefile,v 1.4 1999/10/13 19:12:04 nik Exp nik $	
#    
#    tic tac 4 - tic tac 2 in a 4x4 field
#    Copyright (C) 1999 Niklas Olmes <tux.the.penguin@gmx.de>
#    
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#    
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.  
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
#

CC=	gcc
CFLAGS=	-O6

PROG=	tictac4
OBJS=	main.o
LIBS=	-lncurses

all: clean $(PROG)

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(PROG) $(LDFLAGS)

$(OBJS):
	$(CC) $(CFLAGS) -c $*.c -o $@

clean:
	rm -f $(PROG) *.o

