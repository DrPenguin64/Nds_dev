
#include <nds.h>
#include <stdio.h>

// Handle dual screens
PrintConsole topScreen;
PrintConsole bottomScreen;
Keyboard _keyboard;

// player data
const char* typeGuide[] = {
   "Speed",
   "Fly",
   "Power"
   };
char playerName[256];
int playerType = -1; // 0 = speed, 1 = fly, 2 = power
char timeOfDay[15] = "MORNING";
char clothing[15] = "NOTHING";
char inventory[15] = "NOTHING";

// parsing things
const char* fillerWords[] = { "the", "a", "an", "on", "at", "to", "from", "in", "with", "for" };

//ItemID inventory[10];

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
   //iprintf("\n");
   //consoleSelect(&topScreen);
   while(keysHeld()) { scanKeys(); swiWaitForVBlank(); }
   //keyboardHide();
}

// warning: this will clear the console
int WaitForMenuChoice(const char* prompt, const char* options[], int count)
{
   keyboardHide();
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

void giveStatusInfo()
{
   iprintf("You are %s the %s-type %s\n", playerName, typeGuide[playerType], "hedgehog");
   iprintf("It is currently %s\n", timeOfDay);
   iprintf("You are wearing %s\n", clothing);
   iprintf("You have %s\n", inventory);
}

void handleKill(char* target)
{
   if (target == NULL) iprintf("Kill who, with what?\n");
   else if (strcmp(target, "self") == 0 || strcmp(target, "myself") == 0 || strcmp(target, "you") == 0 ) iprintf("Not in my christian minecraft server.\n");
}

void handleHelp(char* target)
{
   if (target == NULL || strcmp(target, "") == 0 || strcmp(target, "me") == 0) iprintf("And why should I?\n");
   else iprintf("That person isn't here.\n");
}

void handleFuck(char* target)
{
   iprintf("Not in my christian minecraft server, you won't.\n");
}

void handleAsk(char* target)
{
   if (target == NULL) iprintf("Ask who?\n");
   else iprintf("That person isn't here.\n");
}

void parseCommand(char* input) {
    char* verb = strtok(input, " ");
    char* noun = strtok(NULL, " "); // Get the second word

    if (verb == NULL) return;
   
    if (strcmp(verb, "go") == 0) {
        //handleMove(noun);
    }
    else if (strcmp(verb, "ask") == 0)
    {
      handleAsk(noun);
    }
    else if (strcmp(verb, "fuck") == 0 || strcmp(verb, "sex") == 0 )
    {
      handleFuck(noun);
    }
    else if (strcmp(verb, "kill") == 0 || strcmp(verb, "attack") == 0 || strcmp(verb, "fight") == 0)
    {
      iprintf("USING what?\n");
    }
    else if (strcmp(verb, "flee") == 0 || strcmp(verb, "run") == 0)
    {
      iprintf("To where?\n");
    }
    else if (strcmp(verb, "help") == 0)
    {
      handleHelp(noun);
    }
    else if (strcmp(verb, "suicide") == 0)
    {
      iprintf("By means of what?\n");
    }
    else if (strcmp(verb, "look") == 0) {
        //iprintf("%s\n", WorldMap[currentRoom].description);
    }
    else if (strcmp(verb, "take") == 0) {
        // handleTake(noun);
    }
    else {
        iprintf("I don't know how to '%s'.\n", verb);
    }
}

void BeginGame()
{
   consoleClear();
   iprintf("----------------------------------\n");
   iprintf("You wake up in your house (press A to continue)\n");
   WaitForKeyPress();
   consoleClear();
   giveStatusInfo();
   WaitForKeyPress();
   iprintf("\nIt is a school day, but you have about 1 hour before you have to leave\n\n");
   iprintf("You may want to look around before leaving, you never know what may happen or what may come in handy\n\n");
   WaitForKeyPress();

   char lastInput[256];
   

   while (1)
   {
      readLine(lastInput);
      iprintf("You said: %s\n", lastInput);
      parseCommand(lastInput);
      /*
      if (strcmp(lastInput, "repeat") == 0)
      {
         consoleClear();
         giveStatusInfo();
         iprintf("\nIt is a school day, but you have about 1 hour before you have to leave\n\n");
         iprintf("You may want to look around before leaving, you never know what may happen or what may come in handy\n\n");
      }
      else if (strcmp(lastInput, "help") == 0)
      {
         iprintf("Tough luck buddy.\n");
      }
      else if (strcmp(lastInput, "suicide") == 0)
      {
         iprintf("How? Be specific.\n");
      }
      */
      memset(lastInput, 0, sizeof(lastInput));
      WaitForKeyPress();
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
      iprintf("Is this ok? \n(A for yes, B for no)\n");
      // wait for button press
      u32 key = WaitForKeyPress();
      if (key & KEY_A) { // A pressed
         break;
      }
   }
   consoleClear();
   iprintf("%s? Nice!!!\n", playerName);
   //WaitForKeyPress();
   
   const char* choices[] = {
   "Speed",
   "Fly",
   "Power"
   };
   playerType = WaitForMenuChoice(
   "Now choose your type!",
   choices,
   3
   );

   iprintf("You chose: %s\n", choices[playerType]);
   WaitForKeyPress();
   consoleClear();

   char msg[256];
   snprintf(msg, sizeof(msg), "Your name is %s and your type is %s\n------\nIs this ok?",
   playerName, choices[playerType]);

   int startGame = WaitForMenuChoiceYesNo(msg);
   if (startGame == 0)
   {
      // Start game
      iprintf("GAME START!\n");
      WaitForKeyPress();
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

   /*
   consoleSelect(&topScreen);
   iprintf("This goes to TOP screen\n");
   WaitForKeyPress();
   

   consoleSelect(&bottomScreen);
   iprintf("This goes to BOTTOM screen\n");
   WaitForKeyPress();
   */
   
   Keyboard *kbd = 	keyboardInit(&_keyboard, 1,BgType_Text4bpp, BgSize_T_256x256, 20, 4, false, true); // hold pointer to keyboard
   kbd->scrollSpeed = 0;
   kbd->OnKeyPressed = OnKeyPressed; // register callback for on key press
   

   iprintf("bingus test\n");
   WaitForKeyPress();
   consoleSelect(&bottomScreen);
   consoleClear();

   consoleSelect(&topScreen);
   consoleClear();
   printLn("Welcome to Sonic RPG (press A to continue)\n");
   printLn("You can be anyone and do \nanything in this game\n");
   printLn("Well within reason anyway\n");
   printLn("Also you can play with others \non a local online network if you\nlike\n");
   printLn("Anyway lets get you registered\nas an player\n");

   consoleClear();
   DoIntro();


   return 0;
}
