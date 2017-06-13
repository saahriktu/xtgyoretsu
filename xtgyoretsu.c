/*
 * XT gyoretsu v0.2
 * 2013 - 2014 (c) Kurashov A.K., Kostyukov D.A. and Yakushev D.O.
 * licensed under GNU GPLv3
 */
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

WINDOW *gboard, *gbwin, *sttswin;
char cm_flag = 1, kuangshan_yougang_flag = 0, hex_flag = 0, pcm_flag = 0;
int pcm_x, pcm_y, gc_x = 0, gc_y = 0, cnt_x = 0, cnt_y = 3;
char c[2];
long int gscore, gls, gcs, sos, gcne;

void
putwinmsg (char *msgptr)
{
  WINDOW *mw;
  mw =
    newwin (3, strlen (msgptr) + 2, LINES / 2 - 1,
	    COLS / 2 - strlen (msgptr) / 2 - 1);
  box (mw, 0, 0);
  mvwaddstr (mw, 1, 1, msgptr);
  wrefresh (mw);
  wgetch (mw);
  werase (mw);
  wrefresh (mw);
  delwin (mw);
}

void
reinit_game ()
{
  int i_i, i_j;
  char i_rv;
  gscore = 0;
  gls = 0;
  gcs = 0;
  gcne = 27;
  if (hex_flag)
    sos = 16;
  else
    sos = 10;
  for (i_j = 3; i_j <= cnt_y; i_j++)
    mvwaddstr (gboard, i_j, 0, "         ");
  if (cm_flag)
    {
      mvwaddstr (gboard, 0, 0, "123456789");
      mvwaddstr (gboard, 1, 0, "111213141");
      mvwaddstr (gboard, 2, 0, "516171819");
    }
  else
    for (i_j = 0; i_j < 3; i_j++)
      for (i_i = 0; i_i < 9; i_i++)
	if (hex_flag)
	  {
	    i_rv = (int) (15.0 * rand () / (RAND_MAX + 1.0));
	    if (i_rv < 9)
	      mvwaddch (gboard, i_j, i_i, 49 + i_rv);
	    else
	      mvwaddch (gboard, i_j, i_i, 88 + i_rv);
	  }
	else
	  mvwaddch (gboard, i_j, i_i,
		    (int) (49.0 + 9.0 * rand () / (RAND_MAX + 1.0)));
}

int
check_space ()
{
  int i_v, rslt = 0;
  if (pcm_y == gc_y && pcm_x == gc_x)
    return 0;
  if (pcm_y == gc_y)
    {
      if (kuangshan_yougang_flag && abs (pcm_x - gc_x) == 8)
	return 1;
      rslt = 1;
      if (abs (pcm_x - gc_x) > 1)
	for (i_v = 1 + (int) fmin (pcm_x, gc_x); i_v < fmax (pcm_x, gc_x);
	     i_v++)
	  if (mvwinch (gboard, pcm_y, i_v) != ' ')
	    rslt = 0;
    }
  if (pcm_x == gc_x)
    {
      rslt = 1;
      if (abs (pcm_y - gc_y) > 1)
	for (i_v = 1 + (int) fmin (pcm_y, gc_y); i_v < fmax (pcm_y, gc_y);
	     i_v++)
	  if (mvwinch (gboard, i_v, pcm_x) != ' ')
	    rslt = 0;
    }
  return rslt;
}

int
white_flag ()
{
  int chk_x = cnt_x, chk_y = cnt_y, i_x = 0, i_y = 0;
  char cd;
  do
    {
      if (--chk_x < 0)
	{
	  chk_x = 8;
	  chk_y--;
	}
    }
  while ((A_CHARTEXT & mvwinch (gboard, chk_y, chk_x)) == ' ');
  if (++chk_x > 8)
    {
      chk_x = 0;
      chk_y++;
    }
  cnt_x = chk_x;
  cnt_y = chk_y;
  do
    {
      if ((cd = A_CHARTEXT & mvwinch (gboard, i_y, i_x)) != ' ')
	{
	  mvwaddch (gboard, cnt_y, cnt_x, cd);
	  if (++cnt_x > 8)
	    {
	      cnt_x = 0;
	      cnt_y++;
	    }
	  if (cnt_y * 9 + cnt_x > 8999)
	    {
	      putwinmsg ("Game over...");
	      reinit_game ();
	      return 1;
	    }
	  gcne += 1;
	}
      if (++i_x > 8)
	{
	  i_x = 0;
	  i_y++;
	}
    }
  while (i_y * 9 + i_x < chk_y * 9 + chk_x);
  return 0;
}

int
win_chk ()
{
  int i_x = 0, i_y = 0, rslt = 1;
  do
    {
      if ((A_CHARTEXT & mvwinch (gboard, i_y, i_x)) != ' ')
	{
	  rslt = 0;
	  break;
	}
      if (++i_x > 8)
	{
	  i_x = 0;
	  i_y++;
	}
    }
  while (i_y * 9 + i_x < cnt_y * 9 + cnt_x);
  return rslt;
}

void
cleaning_linez ()
{
  int i_x, i_y, i_fr, els;
  char cd;
  for (i_y = 0; i_y < cnt_y; i_y++)
    {
      els = 1;
      for (i_x = 0; i_x < 9; i_x++)
	if ((A_CHARTEXT & mvwinch (gboard, i_y, i_x)) != ' ')
	  {
	    els = 0;
	    break;
	  }
      if (els)
	{
	  for (i_fr = i_y; i_fr < cnt_y; i_fr++)
	    for (i_x = 0; i_x < 9; i_x++){
		cd = A_CHARTEXT & mvwinch (gboard, i_fr + 1, i_x);
	        mvwaddch (gboard, i_fr, i_x, cd);
	    }
	  for (i_x = 0; i_x < 9; i_x++)
	      mvwaddch (gboard, cnt_y - 1 + pow(1, cnt_x), i_x, ' ');
	  cnt_y--;
	  gls++;
	  gscore += 10;
	}
    }
}

void
display_status ()
{
  int i_j;
  for (i_j = 8; i_j < 14; i_j++) mvwaddstr (sttswin, i_j, 7, "                ");
  mvwprintw (sttswin, 8, 0, "Score: %d", gscore);
  mvwprintw (sttswin, 9, 0, "Lines sweeped: %d", gls);
  mvwprintw (sttswin, 10, 0, "Cells sweeped: %d", gcs);
  mvwprintw (sttswin, 11, 0, "Not empty lines: %d", cnt_y);
  mvwprintw (sttswin, 12, 0, "Not empty cells: %d", gcne);
  mvwprintw (sttswin, 13, 0, "Strike out sum: %d", sos);
}

char
read_cell (int c_y, int c_x)
{
  char cd = (A_CHARTEXT & mvwinch (gboard, c_y, c_x));
  if (cd < 58)
    return cd - 48;
  else
    return cd - 87;
}

int
main ()
{
  char tty_in;
  int gboard_y = 0, cl = A_NORMAL;
  double ts, te;
  srand (time (0));
  initscr ();
  cbreak ();
  curs_set (0);
  noecho ();
  gboard = newpad (1000, 9);
  gbwin = newwin (LINES, 11, 0, COLS / 2 - 17);
  sttswin = newwin (LINES, 22, 0, COLS / 2 - 6);
  mvwaddstr (sttswin, 0, 0, "- __    __  _______  -");
  mvwaddstr (sttswin, 1, 0, "- \\ \\  / / |__   __| -");
  mvwaddstr (sttswin, 2, 0, "-  \\ \\/ /     | |    -");
  mvwaddstr (sttswin, 3, 0, "-  / /\\Gyoretsu |    -");
  mvwaddstr (sttswin, 4, 0, "- /_/  \\_\\    |_|    -");
  mvwhline (sttswin, 5, 0, ACS_HLINE, 22);
  mvwaddstr (sttswin, 6, 0, "C D");
  mvwhline (sttswin, 7, 0, ACS_HLINE, 22);
  box (gbwin, 0, 0);
  reinit_game ();
  for (;;)
    {
      display_status ();
      wrefresh (sttswin);
      wrefresh (gbwin);
      mvwchgat (gboard, gc_y, gc_x, 1, A_REVERSE, 0, NULL);
      prefresh (gboard, gboard_y, 0, 1, COLS / 2 - 16, LINES - 2,
		COLS / 2 + 3);
      ts = time (0);
      tty_in = wgetch (gbwin);
      te = time (0);
      if (kuangshan_yougang_flag && (te - ts) > 9)
	tty_in = 'q';
      mvwchgat (gboard, gc_y, gc_x, 1, cl, 0, NULL);
      if (cl != A_NORMAL)
	cl = A_NORMAL;
      switch (tty_in)
	{
	case 'h':
	  gc_x--;
	  if (gc_x < 0)
	    gc_x = 0;
	  break;
	case 'j':
	  gc_y++;
	  if (gc_y > 999)
	    gc_y = 999;
	  if (gc_y > gboard_y + LINES - 3)
	    {
	      gboard_y++;
	      if (gboard_y > 1002 - LINES)
		gboard_y = 1002 - LINES;
	    }
	  break;
	case 'k':
	  gc_y--;
	  if (gc_y < 0)
	    gc_y = 0;
	  if (gc_y < gboard_y)
	    {
	      gboard_y--;
	      if (gboard_y < 0)
		gboard_y = 0;
	    }
	  break;
	case 'l':
	  gc_x++;
	  if (gc_x > 8)
	    gc_x = 8;
	  break;
	case 'y': if (gc_y < 1 || gc_x < 1) break;
		  gc_x--;
		  gc_y--;
	  break;
	case 'u': if (gc_y < 1 || gc_x > 7) break;
		  gc_x++;
		  gc_y--;
	  break;
	case 'b': if (gc_x < 1 || gc_y > 998) break;
		  gc_x--;
		  gc_y++;
	  break;
	case 'n': if (gc_x > 7 || gc_y > 998) break;
		  gc_x++;
		  gc_y++;
	  break;
	case 'q':
	  white_flag ();
	  break;
	case 'z':
	  if (pcm_flag)
	    {
	      pcm_flag = 0;
	      mvwchgat (gboard, pcm_y, pcm_x, 1, A_NORMAL, 0, NULL);
	      c[0] = read_cell (pcm_y, pcm_x);
	      c[1] = read_cell (gc_y, gc_x);
	      if ((c[0] == c[1] || (c[0] + c[1]) == sos) && check_space ())
		{
		  mvwaddch (gboard, pcm_y, pcm_x, ' ');
		  mvwaddch (gboard, gc_y, gc_x, ' ');
		  gcne -= 2;
		  gcs += 2;
		  gscore++;
		  if (kuangshan_yougang_flag){
		    if (hex_flag)
		      sos = (int) (2.0 + 14.0 * rand () / (RAND_MAX + 1.0));
		    else
		      sos = (int) (2.0 + 8.0 * rand () / (RAND_MAX + 1.0));
		  }
	      	  cleaning_linez ();
		}
	      if (win_chk ())
		{
		  putwinmsg ("The end!");
		  reinit_game ();
		}
	    }
	  else
	    {
	      pcm_flag = 1;
	      cl = A_BOLD;
	      pcm_y = gc_y;
	      pcm_x = gc_x;
	    }
	  break;
	case 'R':
	  cm_flag = 1 - cm_flag;
	  if (cm_flag){
	    hex_flag = 0;
	    mvwaddch (sttswin, 6, 0, 'C');
	    mvwaddch (sttswin, 6, 2, 'D');
	  }
	  else
	    mvwaddch (sttswin, 6, 0, 'R');
	  reinit_game ();
	  break;
	case 'K':
	  kuangshan_yougang_flag = 1 - kuangshan_yougang_flag;
	  if (kuangshan_yougang_flag)
	    mvwaddch (sttswin, 6, 1, 'K');
	  else
	    mvwaddch (sttswin, 6, 1, ' ');
	  reinit_game ();
	  break;
	case 'H':
	  if (cm_flag) break;
	  hex_flag = 1 - hex_flag;
	  if (hex_flag)
	    mvwaddch (sttswin, 6, 2, 'H');
	  else
	    mvwaddch (sttswin, 6, 2, 'D');
	  reinit_game ();
	}
      if (tty_in == 'Q')
	break;
    }
  endwin ();
  return 0;
}
