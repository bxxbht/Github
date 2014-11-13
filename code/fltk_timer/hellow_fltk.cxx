//
// "$Id: hello.cxx,v 1.4.2.3.2.1 2002/01/01 15:11:33 easysw Exp $"
//
// Hello, World! program for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2002 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

#include <FL/Fl.H>
#include <sys/time.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>


long int TIMEOUT[100];

bool			bKeystroke=false;//是否打开按键声音

void send_msg(int type, int vol, int flag){
}

void PrintTimeout(Fl_Widget*, void*) {
  printf("Print Timeout\n");
  for(int i = 0;i<100;i++)
  {
    printf(" %ld ",TIMEOUT[i]);
    if(i%30 == 0)
    {
        printf("\n");
    }
  }
  printf("\n");
}


void recTimeout(void *)
{
  Fl::repeat_timeout(0.01,recTimeout);
  static int index = 0;
  static struct timeval prevclock;
  struct timeval newclock;
  gettimeofday(&newclock, NULL);
  TIMEOUT[index] = (newclock.tv_sec - prevclock.tv_sec) * 1000000 +  (newclock.tv_usec - prevclock.tv_usec);
  prevclock.tv_sec = newclock.tv_sec;
  prevclock.tv_usec = newclock.tv_usec;
  index++;
  if(index >= 100)
  {
	index = 0;
	static int timer =0;
	timer++;
   	if(timer > 5)
	{
		timer = 0;
        	PrintTimeout(NULL,NULL);
	}
  }
}

int main(int argc, char **argv) {
  Fl_Window *window = new Fl_Window(300,180);
  Fl_Button *but = new Fl_Button(20,40,260,100,"Hello, World!");
  but->labelfont(FL_BOLD+FL_ITALIC);
  but->labelsize(36);
  but->labeltype(FL_SHADOW_LABEL);
  but->callback(PrintTimeout);
  but->take_focus();
  window->end();
  window->show(argc, argv);
  Fl::add_timeout(0.5, recTimeout);

  struct sched_param param;
  int maxpri;
      maxpri = sched_get_priority_max(SCHED_FIFO);//»ñÈ¡×î´óÖµ
      if(maxpri == -1) 
      {
            perror("sched_get_priority_max() failed");
            exit(1);
      }
      param.sched_priority = maxpri;
      if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) //设置优先级

     {
            perror("sched_setscheduler() failed");
            exit(1);
     } 
  return Fl::run();
}


//
// End of "$Id: hello.cxx,v 1.4.2.3.2.1 2002/01/01 15:11:33 easysw Exp $".
//
