#include <ncurses.h>
#include <string.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>


int CreateArray(char **array,char *message,int w)
{
    int c =0;
    char buffer[500];
    snprintf(buffer, sizeof(buffer), "%s", message);

    char *token = strtok(buffer, "~");
    char *last = NULL;

    while (token != NULL) {
        array[c] = malloc(strlen(token) + 1);
        strcpy(array[c],token);
        last = token;
        token = strtok(NULL, "~");
        c++;
    }
    for(int i=0;i<c;i++){
        if((w-2)<strlen(array[i])){
            clear();
            mvprintw(0,(COLS-strlen("THE STRING '' IS TOO LONG")-strlen(array[i]))/2,"THE STRING '%s' IS TOO LONG",array[i]);
            getch();
            for (int j = 0; j <= i; j++)
                free(array[j]);
            return 0;
        }
    }
    return c;
}
void msgbox(int h,int w,char *title,char *message)
{   
    int x,y;
    x = (COLS-w)/2;
    y = (LINES-h)/2;

    char *array[h];
    int c = CreateArray(array,message,w);
    if (!c){
        return;
    }
    WINDOW *win = newwin(h,w,y,x);
    box(win,0,0);
    mvwprintw(win,0,((w)-strlen(title))/2,title);

    

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
    int x,y;
    x = (COLS-w)/2;
    y = (LINES-h)/2;

    char *array[h];
    int c = CreateArray(array,message,w);
    if (!c){
        return;
    }

    WINDOW *win = newwin(h,w,y,x);
    box(win,0,0);


    mvwprintw(win,0,(w-strlen(title))/2,title);

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
bool yesno(int h,int w,char *title,char *message,char *yes,char *no)
{
    int x,y,d;
    d = 0;
    x = (COLS-w)/2;
    y = (LINES-h)/2;

    char *array[h];
    int c = CreateArray(array,message,w);
    if (!c){
        return false;
    }

    WINDOW *win = newwin(h,w,y,x);
    box(win,0,0);
    mvwprintw(win,0,((w)-strlen(title))/2,title);
    // Prints the generated array on multiple lines
    for (int i = 0; i < c; i++) {
    mvwprintw(win, i + 2,(w - strlen(array[i])) / 2,"%s", array[i]);
    }   

    keypad(win,true);

    mvwprintw(win,h-2,w-(8+strlen(no)),no);
    wattron(win,A_REVERSE);
    mvwprintw(win,h-2,5+strlen(yes),yes);
    wattroff(win,A_REVERSE);

    bool yn = true;

    while (d!=10){
        d = wgetch(win);
        if (d == KEY_RIGHT){
            mvwprintw(win,h-2,5+strlen(yes),yes); 
            wattron(win,A_REVERSE);
            mvwprintw(win,h-2,w-(8+strlen(no)),no);
            wattroff(win,A_REVERSE);
            yn=false;
        }
        else if (d == KEY_LEFT){
            mvwprintw(win,h-2,w-(8+strlen(no)),no);
            wattron(win,A_REVERSE);
            mvwprintw(win,h-2,5+strlen(yes),yes); 
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
void inputbox(int h,int w,char *title,char *message,char out[200])
{
    int x,y;
    x = (COLS-w)/2;
    y = (LINES-h)/2;
    static char str[50];
    char *array[h];
    int c = CreateArray(array,message,w);
    if (!c){
        return;
    }

    WINDOW *win = newwin(h,w,y,x);
    WINDOW *input = newwin(3,w-6,y+(h-4),x+3);
    box(win,0,0);
    box(input,0,0);

    mvwprintw(win,0,(w-strlen(title))/2,title);
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
    wgetnstr(input,out,w-8);
    // disables the display of user input and returns the string
    noecho();
}
int menu(int h, int w, char *title, char *options)
{
    int x, y;
    int c = 0;

    x = (COLS - w) / 2;
    y = (LINES - h) / 2;

    WINDOW *win = newwin(h, w, y, x);
    keypad(win, TRUE);

    box(win, 0, 0);
    mvwprintw(win, 0, (w - strlen(title)) / 2, "%s", title);
    wrefresh(win);

    /* Parse options into array  */
    char buffer[2500];
    snprintf(buffer, sizeof(buffer), "%s", options);

    char array[300][300];   // supports up to 300 items
    char *token = strtok(buffer, "~");

    while (token && c < 300) {
        strncpy(array[c], token, 300);
        array[c][300] = '\0';
        c++;
        token = strtok(NULL, "~");
    }

    /* Scrolling state  */
    int highlight = 0;
    int offset = 0;
    int visible_rows = h - 3;   // space inside window

    int ch;
    while (1) {
        /* Redraw window */
        werase(win);
        box(win, 0, 0);
        mvwprintw(win,h-2,(w-strlen("press [ ENTER ] to continue"))/2,"press [ ENTER ] to continue");
        mvwprintw(win, 0, (w - strlen(title)) / 2, "%s", title);

        for (int i = 0; i < visible_rows; i++) {
            int index = offset + i;
            if (index >= c)
                break;

            if (index == highlight) {
                wattron(win, A_REVERSE);
                mvwprintw(win, i+1, (w-strlen(array[index]))/2, "%s", array[index]);
                wattroff(win, A_REVERSE);
            } else {
                mvwprintw(win, i+1, (w-strlen(array[index]))/2, "%s", array[index]);
            }
        }

        wrefresh(win);

        /* Input */
        ch = wgetch(win);

        if (ch == KEY_DOWN && highlight < c - 1) {
            highlight++;
            if (highlight >= offset + visible_rows)
                offset++;
        }
        else if (ch == KEY_UP && highlight > 0) {
            highlight--;
            if (highlight < offset)
                offset--;
        }
        else if (ch == 10) {  // ENTER
            return highlight;
        }
    }

    delwin(win);
}
void configc(char *location,char **result)
{
    FILE *fptr;
    
    strcat(location,"config");
    fptr = fopen(location,"w");
    if (fptr == NULL){
        return;
    }
    fclose(fptr);
}
int configr(char *location,char **result)
{
    FILE *fptr;
    char line[100];
    int d=0;
    strcat(location,"config.conf");

    fptr = fopen(location,"r");
    if(fptr == NULL) {
        clear();
        mvprintw(LINES/2,(COLS-25)/2,"Not able to open the file.");
        getch();
        return 0;
    }
    else;

    while(fgets(line, 100, fptr)){
        result[d]=strdup(line);
        d++;
    }
    fclose(fptr);
    return d;
}
int Commands(char **result, int km)
{
    FILE *fptr;
    char line[100];
    int i = 0;

    if (km == 1)
        fptr = popen("lsblk -dn -o NAME,SIZE", "r");
    else if (km == 2)
        fptr = popen("localectl list-keymaps", "r");


    if (!fptr)
        return 0;

    while (fgets(line, sizeof(line), fptr)) {
        line[strcspn(line, "\n")] = '\0';
        result[i] = strdup(line);
        i++;
    }

    pclose(fptr);
    return i;
}
void array_to_string(char **array,char *str,int lim)
{
    char *p = str;
    for (int i=0;i<lim;i++){
        size_t len = strlen(array[i]);
        if (len > 0 && array[i][len - 1] == '\n') {
            len--;
        }
        memcpy(p, array[i], len);
        p += len;

        if (i < lim - 1) {
            *p++ = '~';
        }
    }
    *p = '\0';
}
int fileread(char *location,char *name,char **result)
{
    FILE *fptr;
    char line[300];
    int d=0;
    strcat(location,name);

    fptr = fopen(location,"r");
    if(fptr == NULL) {
        clear();
        mvprintw(LINES/2,(COLS-25)/2,"Not able to open the file.");
        getch();
        return 0;
    }
    else;

    while(fgets(line, 100, fptr)){
        result[d]=strdup(line);
        d++;
    }
    fclose(fptr);
    return d;
}




/*
    The Current List of needed variables:
    - Drives
    - Partitioning Scheme
    - keymap
    - Formatting and mounting scheme
    - Applications to install
*/

int main(void)
{
    // necesarry variables
    char *keymap="uk";
    char *drive;
    char partitions[3][200]={"500M","4G",""};
    char format[3][200]={"fat32","swap","ext4"};
    char mounting[2][200]={"/mnt/boot/efi","/mnt"};
    char Cust_commands[3][200];

    // reusable variables
    char buff[250]="Your input is: ";
    char command[1000];
    char *result[300];
    char *message = calloc(1, 20000);
    int index=0;
    int selection=0;

    initscr();
    noecho();
    raw();
    // Network checking
    while (system(": >/dev/tcp/8.8.8.8/53")){
        clear();
        msgbox(10,55,"[ Networking ]","You are not connected to the internet~Please connect on the next screen~ ~IF YOU HAVE CONNECTED ALREADY~MOVE TO THE NEXT SCREEN AND WAIT");
        refresh();
        getch();
        system("nmtui");
    }
    // Set the keymap
    index=Commands(result,2);
    array_to_string(result,message,index);
    index=menu(40,60,"[ Keymap ]",message);
    strcpy(command,"loadkeys ");
    strcat(command,result[index]);
    system(command);


    clear();
    refresh();
    // Main Menu
    while(selection!=6){
        selection=menu(15,60,"[ Arch Installation ]","Drives selection and Partitioning~Formatting(For Advanced Users)~Mounting(For Advanced Users)~Additional Applications to install~Configurations~Run Program~[ EXIT ]");
        if (selection==0){
            // Drive selection
            index=Commands(result,1);
            array_to_string(result,message,index);
            strcat(message,"~[ EXIT ]");
            selection=menu(6,29,"[ Drive Selection ]",message);
            if(!(selection==index)){
                // Partitioning selection
                drive=result[selection];
                index=menu(8,95,"[ Partitioning Scheme ]","Simple - Will remove all the data on the drive and install Arch with a basic partition~Advanced - You will need to decide on the size of the partitions your self~Custom command - Add a command to be run at the end of the partitioning section~[ EXIT ]");
                
                if (index==1){
                    timed_msgbox(12,72,"[ INFORMATION ]","You have decided to go to the advanced option~You will now create 3 partitions These partitions will be as follows:~boot,swap and root~These partitions are necesary to the function of your computer.",3);
                    clear();
                    if((yesno(8,85,"[ Partition Size ]","You are now deciding the size of the partition you are creating~This partition will be the the [boot] parition so it is suggested to select M~The M and G stand for Megabytes and Gigabytes.","[ M ]","[ G ]"))){
                        inputbox(11,50,"[ Partition Size ]","You have selected Megabytes~Please input the size of the [boot] partition~make sure you only input numbers~when the program runs it will check the inputs~and will reset if you don't input correctly",partitions[0]);
                        strcat(partitions[0],"M ");
                    }
                    else{
                        inputbox(11,50,"[ Partition Size ]","You have selected Gigabytes~Please input the size of the [boot] partition~make sure you only input numbers~when the program runs it will check the inputs~and will reset if you don't input correctly",partitions[0]);
                        strcat(partitions[0],"G ");
                    }
                    clear();

                    if((yesno(8,85,"[ Partition Size ]","You are now deciding the size of the partition you are creating~This partition will be the the mount parition so it is suggested to select M~The M and G stand for Megabytes and Gigabytes.","[ M ]","[ G ]"))){
                        inputbox(11,50,"[ Partition Size ]","You have selected Megabytes~Please input the size of the mount partition~make sure you only input numbers~when the program runs it will check the inputs~and will reset if you don't input correctly",partitions[0]);
                        strcat(partitions[1],"M ");
                    }
                    else{
                        inputbox(11,50,"[ Partition Size ]","You have selected Gigabytes~Please input the size of the mount partition~make sure you only input numbers~when the program runs it will check the inputs~and will reset if you don't input correctly",partitions[0]);
                        strcat(partitions[1],"G ");
                    }
                    clear();

                    inputbox(13,55,"[ Partition Size ]","Please input the size of the root partition~This will be in Gigabytes~make sure you only input numbers~when the program runs it will check the inputs~and will reset if you don't input correctly~IF YOU WANT TO USE THE WHOLE DRIVE~FOR THE INSTALL LEAVE THE TEXT BOX EMPTY",partitions[0]);
                    strcat(partitions[2],"G ");
                }
                else if (index==2){
                    inputbox(10,55,"[ Custom Command ]","Please enter the linux command that is to~be run after the partitioning section",Cust_commands[0]);
                }
            }
        }
        else if (selection==2){
            index=menu(8,50,"[ Formatting ]","Simple - will make the partitions F32 and ext4 for boot and root respectivly~Advanced");
            
        }
    }
    endwin();
    return 0;
}