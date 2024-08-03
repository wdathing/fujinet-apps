#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <stdlib.h>
#include <conio.h>
#endif /* _CMOC_VERSION_ */
#include "stateclient.h"
#include "screens.h"
#include "misc.h"
#include "gamelogic.h"

#include "platform-specific/graphics.h"
#include "platform-specific/input.h"
#include "platform-specific/util.h"
#include "platform-specific/sound.h"
#include <string.h>

#define PLAYER_NAME_MAX 8

/// @brief Convenience function to draw text centered at row Y
void centerText(unsigned char y, char * text) {
  drawText(WIDTH/2-strlen(text)/2, y, text);
}

/// @brief Convenience function to draw text centered at row Y in alternate color
void centerTextAlt(unsigned char y, char * text) {
  drawTextAlt(WIDTH/2-strlen(text)/2, y, text);
}

/// @brief Convenience function to draw status text centered
void centerStatusText(char * text) {
  clearStatusBar();
  drawStatusTextAt((WIDTH-strlen(text))>>1,text);
}

/// @brief Convenience function to reset screen and draw border
void resetScreenWithBorder() {
  resetScreen();
  
  // Draw dice corners
  drawDie(0,0,"1", 0);
  //drawDie(0,HEIGHT/2-2,"2", 0);
  drawDie(0,HEIGHT-3,"3", 0);

  drawDie(WIDTH-3,0,"2", 0);
  //drawDie(WIDTH-3,HEIGHT/2-2,"5", 0);
  drawDie(WIDTH-3,HEIGHT-3,"4", 0);
}

/// @brief Shows information about the game
void showHelpScreen() {
  static unsigned char y;
  
  centerStatusText("loading instructions..");
  

  // This COULD be retrieved from the server, especially if
  // this client were game agnostic.
  resetScreenWithBorder();
  

  drawTextAlt(10,1,"how to play fujitzee");
  drawLine(10,2,20);
  y=3;
  
  y++;drawText(5,y, "players take turns rolling five");
  y++;drawText(5,y, "dice over 13 rounds to fill out");
  y++;drawText(5,y, "their score sheet.");

  y+=2;drawText(5,y, "the player with the highest");
  y++;drawText(5,y, "score wins the game!");

  y+=3;
  centerTextAlt(y, "turn details");

  y+=2;drawText(5,y, "1. re-roll dice up to 2 times");
  y++;drawText(5,y, "2. choose an entry to score");

  y+=3;
  centerTextAlt(y, "upper bonus");

  y+=2;drawText(5,y, "if upper total is 63 or higher,");
  y++;drawText(5,y, "you score a bonus 35 points.");

  centerStatusText("press any key to continue");

  

  clearCommonInput();
  cgetc();
  resetScreen();
  
}

/// @brief Action called in Welcome Screen to check if a server name is stored in an app key
void welcomeActionVerifyServerDetails() {
  // Read server endpoint into server and query
  read_appkey(AK_LOBBY_CREATOR_ID,  AK_LOBBY_APP_ID, AK_LOBBY_KEY_SERVER, tempBuffer);
  
  i=strlen(tempBuffer);
  if (i) {
  
    // Split "server?query" into separate variables  
    while(--i) {
      if (tempBuffer[i]=='?') {
        strcpy(query, tempBuffer+i);
        tempBuffer[i]=0;
        strcpy(serverEndpoint, tempBuffer);
        break;
      }
    }
  }
}




bool inputFieldCycle(uint8_t x, uint8_t y, uint8_t max, uint8_t* buffer) {
  static uint8_t done, curx, lastY;
  
  // Initialize first call to input box
  //i=strlen(buffer);
  if (done == 1 || lastY != y) {
    done=0;
    lastY=y;
    curx = strlen(buffer);
    drawTextAlt(x,y, buffer);
    drawTextCursor(x+curx,y);
  }
   // curx=i;
   // done=0;
 // }




  // Process any waiting keystrokes
  if (kbhit()) {
    done=0;
    
    inputKey = cgetc();

    // Debugging - See what key was pressed
    // itoa(inputKey, tempBuffer, 10);drawText(0,0, tempBuffer);

    if (inputKey == KEY_RETURN && curx>1) {
      done=1;
      // remove cursor
      drawText(x+1+i,y," ");
    } else if (inputKey == KEY_BACKSPACE && curx>0) {
      buffer[--curx]=0;
      drawText(x+1+curx,y," ");
    } else if (
      curx < max && ((curx>0 && inputKey == KEY_SPACEBAR) || (inputKey>= 48 && inputKey <=57) || (inputKey>= 65 && inputKey <=90) || (inputKey>= 97 && inputKey <=122))    // 0-9 A-Z a-z
    ) {
      
      if (inputKey>=65 && inputKey<=90)
        inputKey+=32;

      buffer[curx]=inputKey;
      buffer[++curx]=0;
    }

    drawTextAlt(x,y, buffer);
    drawTextCursor(x+curx,y);

    return done==1;
  }

  return false;
  
}

void drawLogo()
{
  centerText(4, "fujitzee!");
}

void showPlayerNameScreen() {
  
  resetScreenWithBorder();
  drawLogo();

  
  

  drawText(13,13, "enter your name:");
  drawBox(15,16,PLAYER_NAME_MAX+1,1);
  //drawText(16,17, playerName);
  //i=strlen(playerName);
  
  clearCommonInput();
  //while (inputKey != KEY_RETURN || i<2) {
  while (!inputFieldCycle(16, 17, PLAYER_NAME_MAX, playerName)) ;
  
  
  for (y=13;y<19;++y)
    drawText(13,y, "                 ");
  
  

  write_appkey(AK_LOBBY_CREATOR_ID,  AK_LOBBY_APP_ID, AK_LOBBY_KEY_USERNAME, playerName);
}

/// @brief Action called in Welcome Screen to verify player has a name
void welcomeActionVerifyPlayerName() {
  // Read player's name from app key
  read_appkey(AK_LOBBY_CREATOR_ID,  AK_LOBBY_APP_ID, AK_LOBBY_KEY_USERNAME, tempBuffer);

  tempBuffer[12]=0;
  strcpy(playerName,tempBuffer);

  // Capture username if player didn't come in from the lobby
  if (strlen(playerName) == 0)
    showPlayerNameScreen();
}

/// @brief Shows the Welcome Screen with Logo. Asks player's name
void showWelcomeScreen() {
  resetScreenWithBorder();
  drawLogo();
  
  welcomeActionVerifyPlayerName();
  welcomeActionVerifyServerDetails();

  strcpy(tempBuffer, "welcome ");
  strcat(tempBuffer, playerName);
  centerText(13,tempBuffer);  
  
  pause(45);

  // If first run, show the help screen
  if (prefs[PREF_HELP]!=2) {
    prefs[PREF_HELP]=2;
    savePrefs();
    showHelpScreen();
  } 
  pause(30);
} 

/// @brief Action in Table Selection Screen, joins the selected server
void tableActionJoinServer() {
  // Reset the game state
  clearGameState();
  
  strcat(query, "&player=");
  strcat(query, playerName);
  
  // Replace space with + for player name
  i=strlen(query);
  while(--i)
    if (query[i]==' ')
      query[i]='+';
  
}

/// @brief Shows a screen to select a table to join
void showTableSelectionScreen() {
  static uint8_t shownChip;
  static unsigned char tableIndex=0;
  
  // An empty query means a table needs to be selected
  while (strlen(query)==0) {

    // Show the status immediately before retrival
    centerStatusText("refreshing game list..");
    //drawStatusTimer();
    

    resetScreenWithBorder();
      
    centerText(3, "choose a game to join");
    drawText(6,6, "game");
    drawText(WIDTH-13,6, "players");
    drawLine(6,7,WIDTH-12);

    
    waitvsync();
  
    if (apiCall("tables", false)) {
   
      updateState(true);
      if (tableCount>0) {
        for(i=0;i<tableCount;++i) {
          drawTextAlt(6,8+i*2, state.tables[i].name);
          drawTextAlt(WIDTH-6-strlen(state.tables[i].players), 8+i*2, state.tables[i].players);
          if (state.tables[i].players[0]>'0') {
            drawText(WIDTH-6-strlen(state.tables[i].players)-2, 8+i*2, "*");
          }
        }
      } else {
        centerTextAlt(12, "sorry, no servers are available");
      }

      //drawStatusText("r>efresh   h+elp  c:olor   n+ame   q+uit");
      centerStatusText("Refresh Help Color Name Quit");
      
      
      
      shownChip=0;

      clearCommonInput();
      while (!inputTrigger || !tableCount) {
        readCommonInput();
       
        if (inputKey == 'h' || inputKey == 'H') {
          showHelpScreen();
          break;
        } else if (inputKey == 'r' || inputKey =='R') {
          break;
        } else if (inputKey == 'c' || inputKey =='C') {
          prefs[PREF_COLOR] = cycleNextColor()+1;
          savePrefs();
          break;
         } else if (inputKey == 'n' || inputKey =='N') {
          showPlayerNameScreen();
          break;
        } else if (inputKey == 'q' || inputKey =='Q') {
          quit();
        } /*else if (inputKey != 0) {
          itoa(inputKey, tempBuffer, 10);
          drawStatusText(tempBuffer);
        }*/
        
        if (!shownChip || (tableCount>0 && inputDirY)) {

          drawText(4,8+tableIndex*2," ");
          tableIndex+=inputDirY;
          if (tableIndex==255) 
            tableIndex=tableCount-1;
          else if (tableIndex>=tableCount)
            tableIndex=0;

          drawChip(4,8+tableIndex*2);

          soundCursor();
          shownChip=1;
        }
      }
      
      
      
      if (inputTrigger) {
        soundSelectMove();

        // Clear screen and write server name
        resetScreenWithBorder();
        clearStatusBar();
        centerText(15, state.tables[tableIndex].name);
        
        strcpy(query, "?table=");
        strcat(query, state.tables[tableIndex].table);
        strcpy(tempBuffer, serverEndpoint);
        strcat(tempBuffer, query);

        //  Update server app key in case of reboot 
        write_appkey(AK_LOBBY_CREATOR_ID,  AK_LOBBY_APP_ID, AK_LOBBY_KEY_SERVER, tempBuffer);

      }
    }
  }
  
  centerText(17, "connecting to server");
  
  
  progressAnim(19);
  
  tableActionJoinServer();
}

/// @brief Shows main game play screen (table and cards)
void showGameScreen() {
  // checkIfSpectatorStatusChanged();
  // checkIfPlayerCountChanged();
  
  // // Animate chips to pot before drawing the new state
  // animateChipsToPotOnRoundEnd();
  
  // checkFinalFlip();

  // resetScreen();
  // resetStateIfNewGame();

  // drawPot();

  // if (playerCount>1) {
  //   drawNamePurse();
  //   drawBets();
  //   drawCards(false);
  // }

  // drawGameStatus();
  // 
  // highlightActivePlayer();

  prevRound = state.round;
}

/// @brief shows in-game menu
void showInGameMenuScreen() {
  i=1;
  while (i) {
    
    resetScreenWithBorder();
    
    
    x = WIDTH/2-8;
    y = HEIGHT/2-5;
      
    drawBox(x-3,y-3,21,12);
    drawTextAlt(x,y,    "  Q: quit table");
    drawTextAlt(x,y+=2, "  H: how to play"); 
    drawTextAlt(x,y+=2, "  C: color toggle");
    drawTextAlt(x,y+=2, "ESC: keep playing"); 
    

    clearCommonInput();
    i=1;
    while (i==1) {
      readCommonInput();
      switch (inputKey) {
        case 'c':
        case 'C':
            prefs[PREF_COLOR] = cycleNextColor()+1;
            savePrefs();
            i=2;
            break;
        case 'h':
        case 'H':
          showHelpScreen();
        case KEY_ESCAPE:
        case KEY_ESCAPE_ALT:
          i=0;
          break;
        case 'q':
        case 'Q':
          resetScreenWithBorder();
          centerText(10, "please wait");
          

          // Inform server player is leaving
          apiCall("leave", false);
          progressAnim(12);
          
          //  Clear server app key in case of reboot 
          write_appkey(AK_LOBBY_CREATOR_ID,  AK_LOBBY_APP_ID, AK_LOBBY_KEY_SERVER, "");

          // Clear query so a new table will be selected
          strcpy(query,"");
          showTableSelectionScreen();
          return;
      }
    }
  }

  // Show game screen again before returning
  resetScreen();
  
  showGameScreen();
}

