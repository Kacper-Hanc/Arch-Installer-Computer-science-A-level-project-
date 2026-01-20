#include <ncurses.h>
#include <string.h>
#include <unistd.h> 
#include <stdio.h>

#define MAKE_MSG(buf, fmt, ...) \
    snprintf(buf, sizeof(buf), fmt, __VA_ARGS__)

void msgbox(int h,int w,char *title,char *message)
{   
    int x,y,c;
    c = 0;
    x = (COLS-w)/2;
    y = (LINES-h)/2;

    char array[h][50];

    WINDOW *win = newwin(h,w,y,x);
    box(win,0,0);
    mvwprintw(win,0,((w)-strlen(title))/2,title);

    char buffer[200];
    snprintf(buffer, sizeof(buffer), "%s", message);

    char *token = strtok(buffer, "-");
    char *last = NULL;

    while (token != NULL) {
        strcpy(array[c],token);
        last = token;
        token = strtok(NULL, "-");
        c++;
    }


    // Prints the generated array on multiple lines
    for (int i = 0; i < c; i++) {
    mvwprintw(win, i + 2,(w - strlen(array[i])) / 2,"%s", array[i]);
    }   
    //Reverses the colour of the terminal to highlight the OK button 
    wattron(win, A_REVERSE);
    mvwprintw(win, h - 2, (w - 6) / 2, "[ OK ]");
    wattron(win, A_REVERSE);
    wmove(win,h-2,(w-6)/2);
    refresh();
    wrefresh(win);
}

void timed_msgbox(int h,int w,char *title, char *message,int t)
{
    int x,y,c;
    c = 0;
    x = (COLS-w)/2;
    y = (LINES-h)/2;

    char array[h][50];

    WINDOW *win = newwin(h,w,y,x);
    box(win,0,0);


    mvwprintw(win,0,(w-strlen(title))/2,title);
    // Create a buffer for the string literal to be modified
    char buffer[200];
    strncpy(buffer, message, sizeof(buffer));
    buffer[sizeof(buffer)-1] = '\0';
    // Splits the string int an array The delimitter/ splitting point being at each '-' character
    char* token = strtok(buffer, "-");
    while (token != nullptr){
        strcpy(array[c],token);
        token = strtok(nullptr, "-");
        c++;
    }
    // Prints the generated array on multiple lines
    for (int i = 0; i < c; i++) {
    mvwprintw(win, i + 2,(w - strlen(array[i])) / 2,"%s", array[i]);
    } 

    wattron(win, A_REVERSE);

    for (int i = t; i > 0; i--) {
        mvwprintw(win, h-2, (w-strlen("Closing in %d seconds"))/2, "Closing in %d seconds", i);
        wmove(win,h-2,(w-6)/2);
        wrefresh(win);
        sleep(1);
    }
    flushinp();
    mvwprintw(win, h-2, (w-strlen("Click a button to continue"))/2, "Click a button to continue");
    wrefresh(win);
    getch();
    wattron(win, A_REVERSE);


    clear();
    refresh();
}

bool yesno(int h,int w,char *title,char *message){
    int x,y,c,d;
    c,d = 0;
    x = (COLS-w)/2;
    y = (LINES-h)/2;

    char array[h][50];

    WINDOW *win = newwin(h,w,y,x);
    box(win,0,0);
    mvwprintw(win,0,((w)-strlen(title))/2,title);
    // Create a buffer for the string literal to be modified
    char buffer[200];
    strncpy(buffer, message, sizeof(buffer));
    buffer[sizeof(buffer)-1] = '\0';
    // Splits the string int an array The delimitter/ splitting point being at each '-' character
    char* token = strtok(buffer, "-");
    while (token != nullptr){
        strcpy(array[c],token);
        token = strtok(nullptr, "-");
        c++;
    }
    // Prints the generated array on multiple lines
    for (int i = 0; i < c; i++) {
    mvwprintw(win, i + 2,(w - strlen(array[i])) / 2,"%s", array[i]);
    }   

    keypad(win,true);

    mvwprintw(win,h-2,w-(8+strlen("NO")),"NO");
    wattron(win,A_REVERSE);
    mvwprintw(win,h-2,5+strlen("YES"),"YES");
    wattroff(win,A_REVERSE);

    bool yn = true;

    while (d!=10){
        d = wgetch(win);
        if (d == KEY_RIGHT){
            mvwprintw(win,h-2,5+strlen("YES"),"YES"); 
            wattron(win,A_REVERSE);
            mvwprintw(win,h-2,w-(8+strlen("NO")),"NO");
            wattroff(win,A_REVERSE);
            yn=false;
        }
        else if (d == KEY_LEFT){
            mvwprintw(win,h-2,w-(8+strlen("NO")),"NO");
            wattron(win,A_REVERSE);
            mvwprintw(win,h-2,5+strlen("YES"),"YES"); 
            wattroff(win,A_REVERSE);
            yn=true;
        }
        refresh();
        wrefresh(win);
    }
    refresh();
    wrefresh(win);
    return yn;
}

void inputbox(int h,int w,char *title,char *message,char out[50])
{
    int x,y,c;
    c = 0;
    x = (COLS-w)/2;
    y = (LINES-h)/2;
    static char str[50];
    char array[h][50];
    WINDOW *win = newwin(h,w,y,x);
    WINDOW *input = newwin(3,w-6,y+(h-4),x+3);
    box(win,0,0);
    box(input,0,0);

    mvwprintw(win,0,(w-strlen(title))/2,title);
    // Create a buffer for the string literal to be modified
    char buffer[200];
    strncpy(buffer, message, sizeof(buffer));
    buffer[sizeof(buffer)-1] = '\0';
    // Splits the string int an array The delimitter/ splitting point being at each '-' character
    char* token = strtok(buffer, "-");
    while (token != nullptr){
        strcpy(array[c],token);
        token = strtok(nullptr, "-");
        c++;
    }
    // Prints the generated array on multiple lines
    for (int i = 0; i < c; i++) {
    mvwprintw(win, i + 2,(w - strlen(array[i])) / 2,"%s", array[i]);
    } 
    // move to the input box and enable user input display
    wmove(input,1,1);
    echo();
    // Refreshes the screen
    refresh();
    wrefresh(win);
    wrefresh(input);
    // gets input from the user limiting to the size of the input box
    // disables the display of user input and returns the string
    wgetnstr(input,out,w-8);
    noecho();
}
void menu(int h,int w,char *title,char *options,char out[50]){
    int x,y,c;
    c = 0;
    x = (COLS-w)/2;
    y = (LINES-h)/2;
    char array[h][50];
    WINDOW *win = newwin(h,w,y,x);
    WINDOW *input = newwin(3,w-6,y+(h-4),x+3);
    box(win,0,0);
    box(input,0,0);

    mvwprintw(win,0,(w-strlen(title))/2,title);
    char buffer[200];
    snprintf(buffer, sizeof(buffer), "%s", options);

    char *token = strtok(buffer, "-");
    char *last = NULL;

    while (token != NULL) {
        strcpy(array[c],token);
        last = token;
        token = strtok(NULL, "-");
        c++;
    }

    keypad(win,true);
    int kp;
    int highlight=0;

    while (1){
        // Prints the generated array on multiple lines
        for (int i = 0; i < c; i++) {
            if (i==highlight){
                wattron(win,A_REVERSE);
                mvwprintw(win, i + 2,1,"%s", array[i]);
                wattroff(win,A_REVERSE);
            }
            else{
                mvwprintw(win, i + 2,1,"%s", array[i]);
            }
        }
    refresh();
    kp = wgetch(win);
    if (kp == KEY_DOWN && highlight<c-1){
        highlight++;
    }
    else if (kp == KEY_UP && highlight>0){
        highlight--;
    }
    else if (kp == 10){
            strncpy(out, array[highlight], 50);
            out[50] = '\0';
            return;
    }
    }
}

int main(void)
{
    return 0;
}
