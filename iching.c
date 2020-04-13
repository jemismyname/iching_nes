
/*
NES/neslib port of 'iching.c' from 
https://hexadecimal.uoregon.edu/ching/

Set the screen background color and palette colors.
Then write a message to the nametable.
Finally, turn on the PPU to display video.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


#include "neslib.h"

// link the pattern table into CHR ROM
//#link "chr_generic.s"


void cast(char *hexagram)
{
	int sum, i;

	sum = 2458;
	//while ((c = getchar()) != EOF) sum += c;
	srand(sum);
	for (i = 0; i < 6; i++) {
		sum = 2 + ((rand() >> 7) & 1);
		sum += 2 + ((rand() >> 7) & 1);
		sum += 2 + ((rand() >> 7) & 1);
		hexagram[i] = '0' + sum;
	}
}

int changing(char *hexagram)
{
	int i;

	for (i = 0; i < 6; i++) if (hexagram[i] == '6' || hexagram[i] == '9') return 1;
	return 0;
}

void change(char *hexagram)
{
	int i;

	for (i = 0; i < 6; i++) {
		if (hexagram[i] == '6') hexagram[i] = '7';
		if (hexagram[i] == '9') hexagram[i] = '8';
	}
}


int number(char *hexagram)
{
	int i, h = 0;
	static char hnum[64] = {
		  2, 24,  7, 19, 15, 36, 46, 11,
		 16, 51, 40, 54, 62, 55, 32, 34,
		  8,  3, 29, 60, 39, 63, 48,  5,
		 45, 17, 47, 58, 31, 49, 28, 43,
		 23, 27,  4, 41, 52, 22, 18, 26,
		 35, 21, 64, 38, 56, 30, 50, 14,
		 20, 42, 59, 61, 53, 37, 57,  9,
		 12, 25,  6, 10, 33, 13, 44,  1
	};

	for (i = 0; i < 6; i++) if (hexagram[i] == '7' || hexagram[i] == '9') h += 1 << i;
	return hnum[h];
}
void print(char *hexagram, int ntc, int * ntr)
{
  
	//FILE *text;
	//char line[80], top[8];
  char top[8];
	int i = 6, echo = 0;
/*
	if ((text = fopen(ICHING_PATH, "r")) == 0) {
		perror(ICHING_PATH);
		exit(1);
	}
*/
  //ntr=0;
vram_adr(NTADR_A(ntc,*ntr));
  
	sprintf(top, "%i  ", number(hexagram));
  vram_write(top, 3);
  //vram_write(hexagram, 6);
  
  for(i;i>-1;i--){
    vram_adr(NTADR_A(ntc,*ntr+=2));
    if(hexagram[i] == '7') vram_write("--------  ",10);
    if(hexagram[i] == '8') vram_write("---  ---  ",10);
    if(hexagram[i] == '9') vram_write("--------* ",10);
    if(hexagram[i] == '6') vram_write("---  ---* ",10);
    
  }
  //vram_write(hexagram, 8);
  /*
	fputs("\n", stdout);
	while (fgets(line, sizeof(line), text) != 0) {
		if (*line == '\f') *line = '\n';
		if (i == -3) {
			if (strstr(line, top) != 0 && strstr(line, " / ") != 0) echo = 1, i++;
		}
		else if (i == -2) {
			if (strstr(line, "The Lines") != 0)
				if (!changing(hexagram)) break;
				else i++;
		}
		else {
			if (*line == '\n') i++;
			if (i < 6) echo = (hexagram[i] == '6' || hexagram[i] == '9');
			else if (memcmp(hexagram, "666666", 6) && memcmp(hexagram, "999999", 6) || i > 6) break;
		}
		if (echo) fputs(line, stdout);
	}
	fclose(text);
        */
}

// main function, run after console reset
void main(void) {
  int i;
  int * ntr;//nametable row
  int ntc; //nametable col
  char hexagram[6];
  cast(hexagram);
  *ntr=6;
  ntc=6;
  i=6;
  
  // set palette colorsprint(hexagram);

  pal_col(0,0x23);	// set screen to dark blue
  pal_col(1,0x14);	// fuchsia
  pal_col(2,0x20);	// grey
  pal_col(3,0x30);	// white

  // write text to name table
  //vram_adr(NTADR_A(*ntr,*ntc));		// set address
  print(hexagram,ntc, ntr);
  //vram_adr(NTADR_A(2,*ntr++));
  if (changing(hexagram)) {
    //vram_adr(NTADR_A(*ntc,*ntr));
    ntc+=12;
    *ntr=6;
    change(hexagram);
     print(hexagram,ntc,ntr);
  }
  //vram_write("HELLO, WORLD!", 13);	// write bytes to video RAM

  // enable PPU rendering (turn on screen)
  ppu_on_all();

  // infinite loop
  while (1) ;
}
