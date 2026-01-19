#include <ncurses.h>   // Core ncurses functions (windows, input, drawing)
#include <string.h>    // For string handling (strncpy)
#include <unistd.h>    // For sleep()

/* -----------------------------
   Message box (OK)
   ----------------------------- */
void msgbox(const char *title, const char *message, const int *width, const int *height)
{
    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, starty, startx);
    box(win, 0, 0);

    mvwprintw(win, 0, 2, " %s ", title);
    mvwprintw(win, 2, 2, "%s", message);
    mvwprintw(win, height - 2, (width - 6) / 2, "[ OK ]");

    wrefresh(win);
    wgetch(win);

    delwin(win);
}

/* -----------------------------
   Timed message box
   ----------------------------- */
void timed_msgbox(const char *title, const char *message, int seconds)
{
    int height = 7;
    int width = 50;

    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, starty, startx);
    box(win, 0, 0);

    mvwprintw(win, 0, 2, " %s ", title);
    mvwprintw(win, 2, 2, "%s", message);

    for (int i = seconds; i > 0; i--) {
        mvwprintw(win, 4, 2, "Closing in %d seconds... ", i);
        wrefresh(win);
        sleep(1);
    }

    delwin(win);
}

/* -----------------------------
   Yes / No dialog
   Returns 1 = Yes, 0 = No
   ----------------------------- */
int yesno(const char *title, const char *question)
{
    int choice = 0;

    int height = 7;
    int width = 50;

    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, starty, startx);
    keypad(win, TRUE);
    box(win, 0, 0);

    mvwprintw(win, 0, 2, " %s ", title);

    while (1) {
        mvwprintw(win, 2, 2, "%s", question);

        if (choice == 0) {
            wattron(win, A_REVERSE);
            mvwprintw(win, 4, 14, " Yes ");
            wattroff(win, A_REVERSE);
            mvwprintw(win, 4, 25, " No ");
        } else {
            mvwprintw(win, 4, 14, " Yes ");
            wattron(win, A_REVERSE);
            mvwprintw(win, 4, 25, " No ");
            wattroff(win, A_REVERSE);
        }

        wrefresh(win);

        int ch = wgetch(win);
        if (ch == KEY_LEFT || ch == KEY_RIGHT)
            choice = !choice;
        else if (ch == '\n')
            break;
    }

    delwin(win);
    return choice == 0;
}

/* -----------------------------
   Input box
   ----------------------------- */
void inputbox(const char *title, const char *prompt, char *output, int maxlen)
{
    int height = 7;
    int width = 50;

    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, starty, startx);
    box(win, 0, 0);

    mvwprintw(win, 0, 2, " %s ", title);
    mvwprintw(win, 2, 2, "%s", prompt);

    echo();
    curs_set(1);

    mvwgetnstr(win, 4, 2, output, maxlen - 1);

    noecho();
    curs_set(0);

    delwin(win);
}

/* -----------------------------
   Simple menu
   Returns selected index
   ----------------------------- */
int menu(const char *title, const char *items[], int count)
{
    int selected = 0;

    int height = count + 4;
    int width = 40;

    int starty = (LINES - height) / 2;
    int startx = (COLS - width) / 2;

    WINDOW *win = newwin(height, width, starty, startx);
    keypad(win, TRUE);
    box(win, 0, 0);

    mvwprintw(win, 0, 2, " %s ", title);

    while (1) {
        for (int i = 0; i < count; i++) {
            if (i == selected)
                wattron(win, A_REVERSE);

            mvwprintw(win, i + 2, 2, "%s", items[i]);

            if (i == selected)
                wattroff(win, A_REVERSE);
        }

        wrefresh(win);

        int ch = wgetch(win);
        if (ch == KEY_UP)
            selected = (selected - 1 + count) % count;
        else if (ch == KEY_DOWN)
            selected = (selected + 1) % count;
        else if (ch == '\n') {
            delwin(win);
            return selected;
        }
    }
}

/* -----------------------------
   main()
   ----------------------------- */
int main(void)
{
    /* Initialize ncurses */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    /* Variables to store dialog input */
    char username[32] = {0};
    char hostname[64] = {0};
    int install_choice;
    int confirm_install;

    /* Menu input */
    const char *install_menu[] = {
        "Minimal Install",
        "Full Install",
        "Exit"
    };

    install_choice = menu("Installation Type", install_menu, 3);

    if (install_choice == 2) {
        endwin();
        return 0;
    }

    /* Input boxes */
    inputbox("User Setup", "Enter username:", username, sizeof(username));
    inputbox("System Setup", "Enter hostname:", hostname, sizeof(hostname));

    /* Yes / No dialog */
    confirm_install = yesno(
        "Confirmation",
        "Proceed with installation?"
    );

    if (!confirm_install) {
        msgbox("Aborted", "Installation cancelled by user.");
        endwin();
        return 0;
    }

    /* Timed message */
    timed_msgbox(
        "Installing",
        "Please wait while installation starts...",
        3
    );

    /* Final message */
    char summary[128];
    snprintf(
        summary,
        sizeof(summary),
        "User: %s\nHost: %s\nMode: %s",
        username,
        hostname,
        install_menu[install_choice]
    );

    msgbox("Summary", summary);

    /* Cleanup */
    endwin();
    return 0;
}
