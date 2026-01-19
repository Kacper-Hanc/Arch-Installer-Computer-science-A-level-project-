#include <ncurses.h>
#include <string.h>
#include <unistd.h> 
#include <stdio.h>

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

void inputbox(int h,int w,char *title,char *message)
{
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
    wmove(input,1,1);
    refresh();
    wrefresh(win);
    wrefresh(input);

    keypad(input,true);
    int k=wgetch(input);
}

int main(void)
{
    initscr();  
    noecho();
    //raw();

    char messages[3][50]= {"This is the first line","Second Line", "Third Line"};
    char array[1][50] = {"message"};
   
    msgbox(8,40,"[  ]","Lorem ipsum dolor sit amet, consectetu-er adipiscing elit. Aenean commodo lig-ula eget dolor. Aenean massa. Cum soci-is natoque penatibus et magnis dis par-turient montes, nascetur ridiculus mus-. Donec qu"/*"This is the very very-long piece of text-spanning multiple lines")*/);
    getch();
    timed_msgbox(7,60,"[  ]","This is a timed textbox with multi line-text that is also centered",3);
    inputbox(8,30,"[ TITLE ]","INPUT INTO BOX");
    clear();
    refresh();
    bool t=yesno(8,40,"[  ]","This is the very very-long piece of text-spanning multiple lines");
    clear();
    mvprintw(0,0,"The value is %d",t);
    getch();
    endwin();
    
    return 0;
}
