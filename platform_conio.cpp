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

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>	// move cursor, change color, sleep
#include <conio.h>		// non-blocking input
#include <time.h>		// clock()

HANDLE __WINDOWS_CLI = 0;
/** keep track of the old terminal settings */
WORD oldAttributes;

void release()
{
    platform_setColor(7, 0);
    __WINDOWS_CLI = 0;
    putchar('\n');
}

void init()
{
    if (__WINDOWS_CLI == NULL) {
        __WINDOWS_CLI = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo;
        GetConsoleScreenBufferInfo(__WINDOWS_CLI, &lpConsoleScreenBufferInfo);
        oldAttributes = lpConsoleScreenBufferInfo.wAttributes;
        atexit(release);
    }
}

void platform_move(int row, int col)
{
    COORD p = { (short)col, (short)row };
    init();
    SetConsoleCursorPosition(__WINDOWS_CLI, p);
}

bool platform_kbhit()
{
    init();
    return _kbhit() != 0;
}

int platform_getchar()
{
    int input;
    if (!platform_kbhit()) return -1;
    input = _getch();
    switch ((char)input){
        case '\0': case '\340':
            if (_kbhit()) {
                int nextByte = _getch();
                input |= (nextByte & 0xff) << 8;
            }
    }
    return input;
}

void platform_setColor(int foreground, int background)
{
    init();
    if (foreground < 0){ foreground = oldAttributes & 0xf; }
    if (background < 0){ background = (oldAttributes & 0xf0) >> 4; }
    SetConsoleTextAttribute(__WINDOWS_CLI, (foreground & 0xf) | ((background & 0xf) << 4));
}

void platform_sleep(int ms)
{
    Sleep(ms);
}

long long platform_upTimeMS()
{
    return clock();
}

#else
#include <unistd.h>		// sleep
#include <sys/select.h>	// select, fd_set (for raw, low-level access to input)
#include <sys/time.h>	// for wall-clock timer (as opposed to clock cycle timer)
#include <sys/ioctl.h>	// ioctl
#include <termios.h>	// terminal i/o settings
#include <stdlib.h>

// using ANSI console features to control color and cursor position by default.
// http://en.wikipedia.org/wiki/ANSI_escape_code#graphics

// experimented with ncurses. the TTY ANSI console is plenty powerful already...
// http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html

/** keep track of the old terminal settings */
termios oldTerminalIOSettings;
/** Linux keeps track of time this way. clock() returns CPU cycles, not time. */
timeval g_startTime = { 0, 0 };
/** how long to wait while checking kbhit */
timeval g_tv = { 0, 0 };
/** input check during kbhit */
fd_set g_fds;

void release()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalIOSettings);
    platform_setColor(7, 0);
    putchar('\n');
}

void init()
{
    if (g_startTime.tv_sec == 0) {
        // make getch read right at the key press, without echoing
        tcgetattr(STDIN_FILENO, &oldTerminalIOSettings);
        termios currentTerminalIOSettings = oldTerminalIOSettings;
        currentTerminalIOSettings.c_lflag &= ~(ICANON | ECHO);	// don't wait for enter, don't print
        tcsetattr(STDIN_FILENO, TCSANOW, &currentTerminalIOSettings);
        FD_ZERO(&g_fds);	// initialize the struct that checks for input
        gettimeofday(&g_startTime, NULL);	// start the timer
        atexit(release);
    }
}

bool platform_kbhit()
{
    int result;
    init();
    // check the hardware input stream if there is data waiting
    FD_SET(STDIN_FILENO, &g_fds);
    result = select(STDIN_FILENO + 1, &g_fds, NULL, NULL, &g_tv);
    // specifically, check for data to be read
    return result && (FD_ISSET(0, &g_fds));
}

int platform_getchar()
{
    int buffer;
    init();
    read(STDIN_FILENO, (char *)&buffer, sizeof(buffer));
    return buffer;
}

void platform_move(int row, int col)
{
    init();
    printf("\033[%d;%df", row + 1, col + 1);	// move cursor, without ncurses
}


void platform_setColor(int foreground, int background)
{
    init();
    // colorRGB and colorGRAY usable for TTY (unix/linux) expanded console color
    if (foreground >= 0)
        printf("\033[38;5;%dm", foreground);
    else
        printf("\033[39m");// default foreground color
    if (background >= 0)
        printf("\033[48;5;%dm", background);
    else
        printf("\033[49m");// default background color
}

void platform_sleep(int a_ms)
{
    static timeval endTime, startTime;
    static time_t seconds, useconds, ms;
    init();
    gettimeofday(&startTime, NULL);
    ms = 0;
    while (ms < a_ms)
    {
        usleep(128);	// sleeps "microseconds worth" of CPU-instructions
        gettimeofday(&endTime, NULL);					// but we must calculate
        seconds = endTime.tv_sec - startTime.tv_sec;	// how much time was
        useconds = endTime.tv_usec - startTime.tv_usec;	// really spent
        ms = seconds * 1000 + useconds / 1000;
    }
}

long long platform_upTimeMS()
{
    static timeval now;
    static time_t seconds, useconds, ms;
    init();
    gettimeofday(&now, NULL);
    seconds = now.tv_sec - g_startTime.tv_sec;
    useconds = now.tv_usec - g_startTime.tv_usec;
    ms = seconds * 1000 + useconds / 1000;
    return ms;
}

#endif