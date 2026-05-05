
#include <nds.h>
#include <stdio.h>

// Handle dual screens
PrintConsole topScreen;
PrintConsole bottomScreen;
Keyboard _keyboard;

// player data
char playerName[256];
int playerType = -1; // 0 = speed, 1 = fly, 2 = power

void OnKeyPressed(int key) {
   if(key > 0){
      iprintf("%c", key);
   }

}

u32 WaitForKeyPress()
{
   //keyboardHide();
   iprintf("\n*\n\n");
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();

        u32 keys = keysDown();

        if (keys != 0)
            return keys;
    }
}

// Assumes 256 chars limit
void printLn(char* line)
{
   consoleSelect(&topScreen);
   //consoleClear();
   iprintf("%s", line);
   WaitForKeyPress();
}

void printLnf(const char* fmt, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, 256, fmt, args);
    va_end(args);

    iprintf("%s", buffer);

    WaitForKeyPress();
}

void readLine(char* _input)
{
   consoleSelect(&topScreen);
   iprintf(">");
   memset(_input, 0, 256);
   //fflush(stdin);
   //swiWaitForVBlank();
   iscanf(" %255[^\n]", _input);
   consoleSelect(&topScreen);
   //keyboardHide();
}

// warning: this will clear the console
int WaitForMenuChoice(const char* prompt, const char* options[], int count)
{
   //keyboardHide();
   int selected = 0;
   consoleSelect(&topScreen);
   iprintf("%s\n\n", prompt);
   consoleSelect(&bottomScreen);
   while (1)
   {
      swiWaitForVBlank();
      scanKeys();

      consoleClear();

      u32 keys = keysDown();
      if (keys & KEY_DOWN) {
         selected++;
         if (selected >= count) selected = 0; // loop around
      }
      if (keys & KEY_UP)
      {
         selected--;
         if (selected < 0 ) selected = count - 1; // loop around
      }
      if (keys & KEY_A)
      {
         consoleSelect(&bottomScreen);
         consoleClear();
         consoleSelect(&topScreen);
         return selected;
      }

      
      //iprintf("%s\n\n", prompt);
      // go to fixed cursor position
      //iprintf("\x1b[3;0H"); // move cursor (row 3, column 0)
      //consoleSelect(&bottomScreen);
      for (int i = 0; i < count; i++)
      {
         if (i == selected)
            iprintf("> %s\n", options[i]);
         else
            iprintf("  %s\n", options[i]);
      }
   }
   consoleSelect(&bottomScreen);
   consoleClear();
   consoleSelect(&topScreen);
}

int WaitForMenuChoiceYesNo(const char* prompt)
{
   const char* _choices[] = {
    "Yes",
    "No",
   };
   return WaitForMenuChoice(prompt, _choices, 2);
}

int WaitForChoice(const char* prompt, const char* options[], int count)
{
   int selected = 0;
   char _inputLine[256];
   while (1)
   {
      //swiWaitForVBlank();
      //scanKeys();
      consoleClear();
      iprintf("%s\n\n", prompt);
      for (int i = 0; i < count; i++)
      {
         iprintf("%d %s\n", i, options[i]);
      }
      iprintf("\nEnter choice (0-%d): ", count - 1);
      readLine(_inputLine);
      int choice = atoi(_inputLine);  // convert string → int

      if (choice >= 0 && choice < count)
      {
         return choice; // valid selection
      }

      iprintf("\nInvalid, try again\n");
      WaitForKeyPress();
   }
}


void BeginGame()
{
   consoleClear();

   iprintf("You wake up in your house (press A to continue)\n");
   WaitForKeyPress();
   consoleClear();
   iprintf("It is currently MORNING\n");
   iprintf("You are wearing NOTHING\n");
   WaitForKeyPress();
   iprintf("\nIt is a school day, but you have about 1 hour before you have to leave\n\n");
   iprintf("You may want to look around before leaving, you never know what may happen or what may come in handy\n\n");
   WaitForKeyPress();

   char lastInput[256];
   

   while (1)
   {
      readLine(lastInput);
      iprintf("You said: %s\n", lastInput);

      if (strcmp(lastInput, "repeat") == 0)
      {
         consoleClear();
         iprintf("It is currently MORNING\n");
         iprintf("You are wearing NOTHING\n");
         iprintf("\nIt is a school day, but you have about 1 hour before you have to leave\n\n");
         iprintf("You may want to look around before leaving, you never know what may happen or what may come in handy\n\n");
      }
      memset(lastInput, 0, sizeof(lastInput));
      //WaitForKeyPress();
   }
}

void DoIntro()
{
while (true) // Ask for name
   {
      // Clear player information
      memset(playerName, 0, sizeof(playerName));
      consoleSelect(&topScreen);
      iprintf("What is your name?\n");
      readLine(playerName);
      //iscanf(" %[^\n]", myName);
      //printLn("\nYour name is %s\n", myName)

      consoleClear();
      iprintf("\nYour name is %s\n\n", playerName);
      iprintf("Is this ok? (A for yes, B for no)\n");
      // wait for button press
      u32 key = WaitForKeyPress();
      if (key & KEY_A) { // A pressed
         break;
      }
   }
   iprintf("Nice!!! %s\n", playerName);
   WaitForKeyPress();
   const char* choices[] = {
   "Speed",
   "Fly",
   "Power"
   };
   playerType = WaitForMenuChoice(
   "Now choose your type",
   choices,
   3
   );

   iprintf("You chose: %s\n", choices[playerType]);
   WaitForKeyPress();
   consoleClear();

   char msg[256];
   snprintf(msg, sizeof(msg), "Your name is %s and your type is %s\nIs this ok?",
   playerName, choices[playerType]);

   int startGame = WaitForMenuChoiceYesNo(msg);
   if (startGame == 0)
   {
      // Start game
      BeginGame();
   }
   else {
      DoIntro();
   }
}


int main(void)  {

   videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);

	consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
	consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 29, 0, false, true);

   consoleSelect(&topScreen);
   iprintf("This goes to TOP screen\n");
   WaitForKeyPress();
   

   consoleSelect(&bottomScreen);
   iprintf("This goes to BOTTOM screen\n");
   WaitForKeyPress();
   
   Keyboard *kbd = 	keyboardInit(&_keyboard, 1,BgType_Text4bpp, BgSize_T_256x256, 20, 4, false, true); // hold pointer to keyboard
   kbd->OnKeyPressed = OnKeyPressed; // register callback for on key press
   

   iprintf("bingus test\n");
   WaitForKeyPress();
   consoleSelect(&bottomScreen);
   consoleClear();

   consoleSelect(&topScreen);
   consoleClear();
   printLn("Welcome to Sonic RPG (press A to continue)\n");
   printLn("You can be anyone and do anything in this game\n");
   printLn("Well within reason anyway\n");
   printLn("Also you can play with others on a local online network if you like\n");
   printLn("Anyway lets get you registered as an player\n");

   consoleClear();
   DoIntro();


   return 0;
}
