// <.h>
/** move the cursor to the given location in the console */
void platform_move(int row, int col);
/** true if a key is pressed */
bool platform_kbhit();
/** return what key is currently pressed as a character, or -1 if not pressed. special keys are different on windows/linux */
int platform_getchar();
/** set the color of the command line cursor. linux gets more colors than windows. ignore negative values (use current color) */
void platform_setColor(int foreground, int background);
/** pause the thread for the given number of milliseconds */
void platform_sleep(int ms);
/** how many milliseconds since first use of this API */
long long platform_upTimeMS();

#ifndef PLATFORM_KEY
#define PLATFORM_KEY
#ifdef _WIN32
#define PLATFORM_KEY_UP 'H\340'
#define PLATFORM_KEY_LEFT 'K\340'
#define PLATFORM_KEY_RIGHT 'M\340'
#define PLATFORM_KEY_DOWN 'P\340'
#else
#define PLATFORM_KEY_UP 'A[\033'
#define PLATFORM_KEY_DOWN 'B[\033'
#define PLATFORM_KEY_RIGHT 'C[\033'
#define PLATFORM_KEY_LEFT 'D[\033'
#endif
#endif
// </.h>