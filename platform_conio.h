#ifndef __PLATFORM_CONIO_H
#define __PLATFORM_CONIO_H
/*
 utility functions for the command-line console, usable in both Windows and Linux/Unix
 author: mvaganov@hotmail.com
 license: MIT (http://opensource.org/licenses/MIT). TL;DR - this is free software, I won't fix it for you!
 */

/** move the cursor to the given location in the console */
void platform_move(long row, long col);
/** true (non-zero) if a key is pressed */
long platform_kbhit();
/** return what key is currently pressed as a character, or -1 if not pressed. special keys are different on windows/linux */
long platform_getchar();
/** set the color of the command line cursor. linux gets more colors than windows. ignore negative values (use current color) */
void platform_setColor(long foreground, long background);
/** pause the thread for the given number of milliseconds */
void platform_sleep(long ms);
/** how many milliseconds since first use of this API */
long long platform_upTimeMS();
/** wait for any key to be pressed (thread blocking call) */
inline void platform_waitForAnyKey() { while (!platform_kbhit()) { platform_sleep(1); } }

/*
 // example console application using this library:
 #include "platform_conio.h"
 #include <string.h>         // for strlen
 
 /// fills a rectangle in the console with the given character
 void fillRect(int row, int col, int width, int height, char character) {
	for(int y = row; y < row+height; y++) {
 platform_move(y, col);
 for(int x = col; x < col+width; x++) {
 putchar(character);
 }
	}
 }
 
 int main(int argc, char * argv[])
 {
	int userInput;
	const int frameDelay = 100; // how many milliseconds for each frame of animation
	long long now, whenToStopWaiting; // timing variables
 
	// character animation \ - / | \ - / | \ - / | \ - / |
	const char * animation = "\\-/|";
	int numframes = strlen(animation);
	int frameIndex = 0;
 
	// color shifting for blinking messate
	int colors[] = { 8, 7, 15, 7 };
	int numColors = sizeof(colors) / sizeof(colors[0]);
	int colorIndex = 0;
	do
	{
 // animation
 platform_setColor(7, 0);
 platform_move(3, 10);
 putchar(animation[frameIndex++]);
 frameIndex %= numframes;
 
 // blinking message
 platform_move(3, 12);
 platform_setColor(colors[colorIndex++], 0);
 printf("Press \'a\' to continue ");
 colorIndex %= numColors;
 
 fflush(stdout); // makes sure that whatever was printed will show up in the console
 
 // wait the specified time, or until there is a keyboard hit (interruptable throttle code)
 now = platform_upTimeMS();
 whenToStopWaiting = now + frameDelay;
 while (platform_upTimeMS() < whenToStopWaiting && !platform_kbhit()) { platform_sleep(1); }
 
 userInput = platform_getchar(); // platform_getchar() returns -1 if no key is pressed
	} while (userInput != 'a');
	// prep to make the console look more usable after the program is done
	platform_setColor(-1, -1);    // reset colors to original
	fillRect(0, 0, 80, 10, ' ');  // clear the top 10 lines (80 columns per line)
	platform_move(0,0);           // move the cursor back to back to the start
	return 0;
 }
 */

#include <stdio.h>		// printf and putchar

#ifdef _WIN32
// how to do console utility stuff for Windows

// escape sequence for arrow keys
#define PLATFORM_KEY_UP 'H\340'
#define PLATFORM_KEY_LEFT 'K\340'
#define PLATFORM_KEY_RIGHT 'M\340'
#define PLATFORM_KEY_DOWN 'P\340'

#define NOMINMAX // keeps Windows from defining "min" and "max"
#include <windows.h>	// move cursor, change color, sleep
#include <conio.h>		// non-blocking input
#include <time.h>		// clock()

inline HANDLE * __stdOutputHandle() {
    static HANDLE g_h = 0;
    return &g_h;
}

/** keep track of the old terminal settings */
inline WORD * __oldAttributes() {
    static WORD oldAttributes;
    return &oldAttributes;
}

inline void __platform_release()
{
    if (*__stdOutputHandle() != 0) {
        platform_setColor(*__oldAttributes() & 0xf, *__oldAttributes() & 0xf0);
        *__stdOutputHandle() = 0;
    }
}

inline void __platform_init()
{
    if (*__stdOutputHandle() == 0) {
        *__stdOutputHandle() = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo;
        GetConsoleScreenBufferInfo(*__stdOutputHandle(), &lpConsoleScreenBufferInfo);
        *__oldAttributes() = lpConsoleScreenBufferInfo.wAttributes;
        atexit(__platform_release);
    }
}

inline void platform_move(long row, long col)
{
    if (col < 0) col = 0;
    if (row < 0) row = 0;
    COORD p = { (short)col, (short)row };
    __platform_init();
    SetConsoleCursorPosition(*__stdOutputHandle(), p);
}

inline long platform_kbhit()
{
    __platform_init();
    return _kbhit() != 0;
}

inline long platform_getchar()
{
    long input;
    if (!platform_kbhit()) return -1;
    input = _getch();
    switch ((char)input){
        case '\0': case '\340':
            if (_kbhit()) {
                long nextByte = _getch();
                input |= (nextByte & 0xff) << 8;
            }
    }
    return input;
}

inline void platform_setColor(long foreground, long background)
{
    __platform_init();
    if (foreground < 0){ foreground = (*__oldAttributes()) & 0xf; }
    if (background < 0){ background = (*__oldAttributes() & 0xf0) >> 4; }
    SetConsoleTextAttribute(*__stdOutputHandle(), (foreground & 0xf) | ((background & 0xf) << 4));
}

inline void platform_sleep(long ms)
{
    Sleep(ms);
}

inline long long platform_upTimeMS()
{
    return clock();
}

#else // #ifdef _WIN32
// how to do console utility stuff for *NIX

// escape sequence for arrow keys
#define PLATFORM_KEY_UP 'A[\033'
#define PLATFORM_KEY_DOWN 'B[\033'
#define PLATFORM_KEY_RIGHT 'C[\033'
#define PLATFORM_KEY_LEFT 'D[\033'

#include <unistd.h>		// sleep
#include <sys/select.h>	// select, fd_set (for raw, low-level access to input)
#include <sys/time.h>	// for wall-clock timer (as opposed to clock cycle timer)
#include <sys/ioctl.h>	// ioctl
#include <termios.h>	// terminal i/o settings
#include <stdlib.h>		// atexit

// using ANSI TTY console features to control color and cursor position by default.
// http://en.wikipedia.org/wiki/ANSI_escape_code#graphics

/** keep track of the old terminal settings */
inline termios * __oldTerminalIOSettings() {
    static termios oldTerminalIOSettings;
    return &oldTerminalIOSettings;
}

inline long * __initialized() {
    static long initted = 0;
    return &initted;
}

/** Linux keeps track of time this way. clock() returns CPU cycles, not time. */
inline timeval * __g_startTime() {
    static timeval g_startTime = { 0, 0 };
    return &g_startTime;
}

/** input check during kbhit */
inline fd_set * __g_fds() {
    static fd_set g_fds;
    return &g_fds;
}

inline void __platform_release()
{
    if (*__initialized() != 0) {
        tcsetattr(STDIN_FILENO, TCSANOW, __oldTerminalIOSettings());
        platform_setColor(-1, -1); // set to not-intense-white
        *__initialized() = 0;
    }
}

inline void __platform__init()
{
    if (*__initialized() == 0) {
        *__initialized() = 1;
        // make getch read right at the key press, without echoing
        tcgetattr(STDIN_FILENO, __oldTerminalIOSettings());
        termios currentTerminalIOSettings = *__oldTerminalIOSettings();
        currentTerminalIOSettings.c_lflag &= ~(ICANON | ECHO);	// don't wait for enter, don't print
        tcsetattr(STDIN_FILENO, TCSANOW, &currentTerminalIOSettings);
        FD_ZERO(__g_fds());	// initialize the struct that checks for input
        gettimeofday(__g_startTime(), NULL);	// start the timer
        atexit(__platform_release);
    }
}

inline long platform_kbhit()
{
    static timeval g_tv_zero = { 0, 0 };
    long result;
    __platform__init();
    // check the hardware input stream if there is data waiting
    FD_SET(STDIN_FILENO, __g_fds());
    result = select(STDIN_FILENO + 1, __g_fds(), NULL, NULL, &g_tv_zero);
    // specifically, check for data to be read
    return result && (FD_ISSET(0, __g_fds()));
}

inline long platform_getchar() // if multiple keys are pressed simultaneously, 4-bytes worth of entered keys will output
{
    long buffer = 0;
    if (!platform_kbhit()) return -1;
    read(STDIN_FILENO, (char *)&buffer, 1); // read only one byte
    switch (buffer) {
        case '\033': // if it is an escape sequence, read some more...
            read(STDIN_FILENO, ((char *)&buffer) + 1, 1);
            switch (((char *)&buffer)[1]) {
                case '[': // possibly arrow keys
                    read(STDIN_FILENO, ((char *)&buffer) + 2, 1);
                    break;
            }
            break;
    }
    return buffer;
}

inline void platform_move(long row, long col)
{
    if (col < 0) col = 0;
    if (row < 0) row = 0;
    __platform__init();
    printf("\033[%d;%df", (int)row + 1, (int)col + 1);	// move cursor, without ncurses
}

inline void platform_setColor(long foreground, long background)
{
    __platform__init();
    // colorRGB and colorGRAY usable for TTY (unix/linux) expanded console color
    if (foreground >= 0)
        printf("\033[38;5;%dm", (int)foreground);
    else
        printf("\033[39m");// default foreground color
    if (background >= 0)
        printf("\033[48;5;%dm", (int)background);
    else
        printf("\033[49m");// default background color
}

inline void platform_sleep(long a_ms)
{
    long seconds = a_ms / 1000;
    a_ms -= seconds * 1000;
    timespec time = { seconds, a_ms * 1000000 }; // 1 millisecond = 1,000,000 Nanoseconds
    nanosleep(&time, NULL);
}

inline long long platform_upTimeMS()
{
    static timeval now;
    static time_t seconds, useconds, ms;
    __platform__init();
    gettimeofday(&now, NULL);
    seconds = now.tv_sec - __g_startTime()->tv_sec;
    useconds = now.tv_usec - __g_startTime()->tv_usec;
    ms = seconds * 1000 + useconds / 1000;
    return ms;
}

#endif // #ifdef _WIN32 #else

#endif // __PLATFORM_CONIO_H