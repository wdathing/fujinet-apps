/*
  Graphics functionality
*/

#include "coco_bool.h"

#include "hires.h"
#include "text.h"
//#include<peekpoke.h>
//#include<string.h>
#include "../platform-specific/graphics.h"
#include "../misc.h"
#include "cardgame.h"
#include "../../../../../bgraph-0.1.8/Primitive.h"  /* bgraph */

#define BOTTOM 175
#define RED_VAL_1 ROP_OR(0b11010100)
#define RED_VAL_2 ROP_OR(0b10101010)


#define USER_HAND_ROW       149
#define DISPLAYED_CARD_COL  14
#define DISPLAYED_CARD_ROW  48

#define CARDS_PER_ROW  15

#define OVERLAY_HEIGHT 30  /* A card on top of another one must be drawn this many pixels lower, so enough of the bottom card shows. */

#define STATUS_COL 6
#define STATUS_ROW 1  // in characters

#define DECK_COL 21  // in bytes of pixels
#define DECK_ROW 8  // in pixels

#define EVENT_ROW 13

#define REQUESTED_SUIT_COL 19  // in characters
#define REQUESTED_SUIT_ROW  9  // in characters

#define QUIT_GAME 0
#define NEXT_PLAYER_PLAYS 1
#define NEXT_PLAYER_PASSES 2
#define PLAY_NEW_GAME 3


char blah = 'c';
bool always_render_full_cards = 0;
Primitive_setPixelFuncPtr setPixelFuncPtr;
extern void *screenBuffer;

unsigned char colorMode=0;

unsigned char cycleNextColor() {
  ++colorMode;
  if (colorMode>1)
    colorMode=0;
  return colorMode;
}

void setColorMode(unsigned char mode) {
  colorMode = mode;
}

void enableDoubleBuffer() {
  //  POKE(0xc055,1);
}

void disableDoubleBuffer() {
  // POKE(0xc054,1);
}

void drawTextAt(unsigned char x, unsigned char y, char*s) {
#ifndef DISABLE_GRAPHICS  
  moveCursor(x,y);
#endif  
  printf("%s", s);
}

void clearStatusBar() {
  //unsigned char i;
  //hires_Mask(0,175,40,17,0xa900);
  //for (i=0;i<40;i++) {
   // hires_putc(i,175,ROP_CPY,' ');
   // hires_putc(i,183,ROP_CPY,' ');
 // }
  drawTextAt(0, HEIGHT-2, "                                ");
  drawTextAt(0, HEIGHT-1, "                                ");
}

void drawBuffer() {
//   memcpy((void*)0x4000,(void*)0x2000,0x2000);
}

void drawStatusTextAt(unsigned char x, char* s) {
  drawTextAt(x, HEIGHT-1, s);
}

void drawStatusText(char* s) {
  static char* comma;
  clearStatusBar();
  if (strlen(s)>WIDTH) {
    comma = s;
    while (*comma++!=',');
    comma[0]=0;
    comma++;
    drawTextAt(0, HEIGHT-2, s);
    drawTextAt(0, HEIGHT-1, comma);
  } else {
    drawStatusTextAt(0, s);
  }
  
}

void drawStatusTimer() {
//  hires_putcc(38,BOTTOM+5,ROP_CPY, 0x2829);
}
  

void drawText(unsigned char x, unsigned char y, char* s) {
  //drawTextAt(x,y*8-4, s);
  drawTextAt(x,y, s);
}


void drawChip(unsigned char x, unsigned char y) {
  //hires_putc(x-1,y*8-4,0xa955 - (0x2B*(x%2)), '1');
  //hires_putc(x,y*8-4,ROP_OR(0x55), 'O');
  // Solve gfx later once layout is finalized
  //hires_putc(x,y*8-4,ROP_OR(0x55), 'O');
//  hires_putc(x,y*8-3,ROP_CPY, 0x22);
  
  //hires_putc(x,y,ROP_CPY, 0x22);
  drawCharAtAddressWithoutMovingCursor(0x2a, screenBuffer + (((word) y) << 8) + x, TRUE);
}


// Call to clear the screen to an empty table
void resetScreen() { 
     pcls(0);

    // Draw the deck as a face-down card.
    //
#ifndef DISABLE_GRAPHICS    
    moveCursor(DECK_COL, DECK_ROW >> 3);
    drawFaceDownCard(DECK_ROW + PIXEL_ROWS_PER_TEXT_ROW, DECK_COL);
#endif    

//    basicHelpShown = FALSE;
}

void drawCardAt(unsigned char x, unsigned char y, unsigned char partial, const char* s, bool isHidden) {
  static unsigned char cardgameVal, cardgameSuit;
  if (!isHidden)
  {
    //printf("string is %x-%x\n", (unsigned int)s[0], (unsigned int)s[1]);
    // Card suit
    switch (s[1]) {
      case 'H': case 'h' : cardgameSuit=0; break;
      case 'D': case 'd' : cardgameSuit=2; break;
      case 'C': case 'c' : cardgameSuit=3; break;
      case 'S': case 's' : cardgameSuit=1; break;
      case '?':            isHidden = true; 
      default:             cardgameSuit=4; break;
    }

    // Card value
    switch (s[0]) {
      case 'T': case 't': cardgameVal=10; break;
      case 'J': case 'j': cardgameVal=11;  break;
      case 'Q': case 'q': cardgameVal=12;  break;
      case 'K': case 'k': cardgameVal=13;  break;
      case 'A': case 'a': cardgameVal=1;  break;
      case '?':           isHidden = true; break;
      default:
        cardgameVal=(s[0]-'0'); 
    }
  }


#ifndef DISABLE_GRAPHICS
  if (isHidden)
  {
    drawFaceDownCard(x, y);
    printf("-");  
  }
  else
  {
    drawCompiledCard(cardgameVal, cardgameSuit, x, y);
    printf("+");
  }
printf("%d,%d\n", x,y);
#endif


  
#if 0  
  // Card top
  //hires_putcc(x,y,ROP_O,0x0506); 
  hires_Mask(x,y+7,1,1, ROP_CONST(0b01111100)); 
  hires_Mask(x+1,y+7,1,1, ROP_CONST(0b00111111)); 

  // Card value
  hires_putc(x,y+=8, colorMode && red ? RED_VAL_1 :ROP_CPY ,val);
  hires_putc(x+1,y, colorMode && red ? RED_VAL_2 : ROP_CPY,++val);
  
  // Card middle
  hires_putcc(x,y+=8,ROP_CPY,mid);
 
  // Suit
  hires_putc(x,y+=8,colorMode && red ? RED_VAL_1 :ROP_CPY ,suit);
  hires_putc(x+1,y,colorMode && red ? RED_VAL_2 : ROP_CPY,++suit);

  // Card bottom
  //hires_putcc(x,y+=8,ROP_CPY,0x0708); 
  hires_Mask(x,y+=8,1,1, ROP_CONST(0b01111100)); 
  hires_Mask(x+1,y,1,1, ROP_CONST(0b00111111)); 
#endif
 
}

void drawCard(unsigned char x, unsigned char y, unsigned char partial, const char* s, bool isHidden) {
  y*=8;
  y-=7;
  drawCardAt(x, y, partial, s, isHidden);
}

void drawLine(unsigned char x, unsigned char y, unsigned char w) {
//   hires_Mask(x,y*8-3,w,2, 0xa9ff); 
  unsigned char x1; 
  unsigned char x2;
  unsigned char y1;
  unsigned char y2;

  x++;
  y++;
  x1= x *8 - 4; 
  x2= x1 + w * 8;
  y1= y * 8 - 4;
  y2= y1; // + h*8;

  Primitive_line( x1, y1, x2, y2, 1, Primitive_setPixelPmode4, screenBuffer);
}

void hideLine(unsigned char x, unsigned char y, unsigned char w) {
//   hires_Mask(x,y*8-3,w,2, 0xa900); 
}

void drawBox(unsigned char x, unsigned char y, unsigned char w, unsigned char h) {
  unsigned char x1; 
  unsigned char x2;
  unsigned char y1;
  unsigned char y2;

  x++;
  y++;
  h++;
  x1= x *8 - 4; 
  x2= x1 + w * 8;
  y1= y * 8 - 4;
  y2= y1 + 1 + h*8;

  Primitive_rectangle( x1, y1, x2, y2, 1, Primitive_setPixelPmode4, screenBuffer);

#if 0  
  y=y*8-4;

  // Top Corners
  hires_putc(x,y,ROP_CPY, 0x3b);hires_putc(x+w+1,y,ROP_CPY, 0x3c);
  
  // Accents if height > 1
  if (h>1) {
    hires_putc(x+1,y+8,ROP_CPY, 1);
  }

  // Top/bottom lines
  for(i=x+w;i>x;--i) {
    hires_putc(i,y,ROP_CPY, 0x40);
    hires_putc(i,y+(h+1)*8,ROP_CPY, 0x40);
  }


  
  // Sides
  for(i=0;i<h;++i) {
    y+=8;
    hires_putc(x,y,ROP_CPY, 0x3f);
    hires_putc(x+w+1,y,ROP_CPY, 0x3f);
  }
  
    // Accents if height > 1
  if (h>1) {
    hires_putc(x+w,y,ROP_CPY, 4);
  }

  y+=8;
  // Bottom Corners
  hires_putc(x,y,ROP_CPY, 0x3d);hires_putc(x+w+1,y,ROP_CPY, 0x3e);
#endif  

}

void drawBorder() {
  drawCardAt(1,0,FULL_CARD, "as", 0);
  drawCardAt(37,0,FULL_CARD, "ah", 0);
  drawCardAt(1,134,FULL_CARD, "ad", 0);
  drawCardAt(37,134,FULL_CARD, "ac", 0);
}

void drawLogo() {
  static unsigned char i;
  i=4;
  drawText(15,++i, "           ");
  drawText(15,++i, " FUJI  NET ");
  drawText(15,++i, "           ");
  drawText(15,++i, "5 CARD STUD");
  drawText(15,++i, "           ");
}


void resetGraphics() {
 closeCardGame();
}

void initGraphics() {
  hires_Init();
// enableDoubleBuffer();
  setPixelFuncPtr = Primitive_setPixelPmode4;

}

void waitvsync() {
  static uint16_t i;
  // Aproximate a jiffy for the timer countdown
//  for ( i=0;i<630;i++);
}
