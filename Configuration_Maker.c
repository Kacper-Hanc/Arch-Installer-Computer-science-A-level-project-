#include <ncurses.h>
#include <string.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STR 200

/*
    Define all my variables into Config
*/

typedef struct
{
    char keymap[30];
    char partitions[3][MAX_STR];
    char format[2][MAX_STR];
    char mounting[2][MAX_STR];
    char Cust_commands[4][MAX_STR];
    char Applications[MAX_STR];
    int grub;
} Config;

/*
    LOGIC FUNCTIONS
*/
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
void configc(char *location, Config *cfg)
{
    FILE *fptr;
    char config[200];
    strcpy(config,location);
    strcat(config,"config.conf");
    fptr = fopen(config,"w");

    if (fptr == NULL) return;

    fprintf(fptr, "keymap:\"%s\"\n", cfg->keymap);
    fprintf(fptr, "partition{\n - \"%s\"\n - \"%s\"\n - \"%s\"\n}\n", cfg->partitions[0], cfg->partitions[1], cfg->partitions[2]);
    fprintf(fptr, "format{\n - \"%s\"\n - \"%s\"\n}\n", cfg->format[0], cfg->format[1]);
    fprintf(fptr, "mount{\n - \"%s\"\n - \"%s\"\n}\n", cfg->mounting[0], cfg->mounting[1]);
    fprintf(fptr, "custom commands{\n - \"%s\"\n - \"%s\"\n - \"%s\"\n - \"%s\"\n}\n", cfg->Cust_commands[0], cfg->Cust_commands[1], cfg->Cust_commands[2], cfg->Cust_commands[3]);
    fprintf(fptr, "applications:\"%s\"\n", cfg->Applications);
    fprintf(fptr, "grub:\"%s\"", cfg->grub ? "truez" : "false");
    fclose(fptr);
}
void configr(char *location, Config *cfg)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return;
    }
    if (sscanf(line, "keymap:\"%255[^\"]\"", cfg->name) == 1)
            continue;
    char line[512];
    int current_array = 0;

}

/*
    TUI FUNCTIONS
*/
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

    mvwprintw(win,h-2,w-(8+(2*strlen(no))),no);
    wattron(win,A_REVERSE);
    mvwprintw(win,h-2,5+strlen(yes),yes);
    wattroff(win,A_REVERSE);

    bool yn = true;

    while (d!=10){
        d = wgetch(win);
        if (d == KEY_RIGHT){
            mvwprintw(win,h-2,5+strlen(yes),yes); 
            wattron(win,A_REVERSE);
            mvwprintw(win,h-2,w-(8+(2*strlen(no))),no);
            wattroff(win,A_REVERSE);
            yn=false;
        }
        else if (d == KEY_LEFT){
            mvwprintw(win,h-2,w-(8+(2*strlen(no))),no);
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

int main(void)
{
    Config cfg = {
        .partitions = {"500M","4G",""},
        .format = {"fat32","ext4"},
        .mounting = {"/mnt/boot/efi","/mnt"},
        .Cust_commands = {"(NULL)","(NULL)","(NULL)","(NULL)"},
        .Applications = "base linux linux-firmware sof-firmware base-devel grub efibootmgr",
        .grub = 1
    };

    // reusable commands
    char *result[300];
    char message[20000] = {0};
    char *drive;
    char command[256];
    int index=0;
    int selection=0;

    initscr();
    noecho();
    raw();

    /*
        Network Check
    */
    while (system(": >/dev/tcp/8.8.8.8/53")){
        clear();
        msgbox(10,55,"[ Networking ]","You are not connected to the internet~Please connect on the next screen~ ~IF YOU HAVE CONNECTED ALREADY~MOVE TO THE NEXT SCREEN AND WAIT");
        system("nmtui");
    }

    /*
        Keymap
    */
    timed_msgbox(6,10,"[ Keymap ]","Select a keymap which is similar to your keyboard on the next screen",1)
    index = Commands(result,2);
    array_to_string(result,message,index);
    index = menu(40,60,"[ Keymap ]",message);
    strcpy(command,"loadkeys ");
    strcat(command,result[index]);
    system(command);
    strcpy(cfg.keymap,result[index]);
    clear();
    refresh();
    /*
        Drive Selection
    */
    index = Commands(result,1);
    array_to_string(result,message,index);
    strcat(message,"~[ EXIT ]");
    index = menu(6,29,"[ Drive Selection ]",message);
    drive = result[index];

    timed_msgbox(10,85,"[ Important ]","Welcome to this Arch installation script.~This script can only so much~Follow the suggestions and read the UI otherwise the installation may not work.~This script is not responsible if the installation goes astray.~This will be your final chance to exit the program.",2);
    /*
        Main Menu
    */
    while(selection != 6){
        selection = menu(15,60,"[ Arch Installation ]","Partitioning - How to split the drive for the OS~Formatting(For Advanced Users)~Mounting(For Advanced Users)~Additional Applications to install~Configurations~Run Program~[ EXIT ]");
        // Partitioning
        if(selection == 0){
            index = menu(8,95,"[ Partitioning Scheme ]","Simple - Will remove all the data on the drive and install Arch with a basic partition~Advanced - You will need to decide on the size of the partitions your self~Custom command - Add a command to be run at the end of the partitioning section~[ EXIT ]");
            if(index == 1){
                timed_msgbox(12,72,"[ INFORMATION ]","You have decided to go to the advanced option~You will now create 3 partitions These partitions will be as follows:~boot,swap and root~These partitions are necesary to the function of your computer.",3
                );

                // Boot partition size
                if(yesno(8,85,"[ Partition Size ]","You are now deciding the size of the partition you are creating~This partition will be the the [boot] parition so it is suggested to select M~The M and G stand for Megabytes and Gigabytes.","[ M ]","[ G ]"))
                {
                    inputbox(11,50,"[ Partition Size ]","You have selected Megabytes~Please input the size of the [boot] partition",cfg.partitions[0]);
                    strcat(cfg.partitions[0]," M");
                } else {
                    inputbox(11,50,"[ Partition Size ]","You have selected Gigabytes~Please input the size of the [boot] partition",cfg.partitions[0]);
                    strcat(cfg.partitions[0]," G");
                }

                // Mount partition size
                if(yesno(8,85,"[ Partition Size ]","You are now deciding the size of the partition you are creating~This partition will be the the mount parition so it is suggested to select M~The M and G stand for Megabytes and Gigabytes.","[ M ]","[ G ]"))
                {
                    inputbox(11,50,"[ Partition Size ]","Megabytes",cfg.partitions[1]);
                    strcat(cfg.partitions[1]," M");
                } else {
                    inputbox(11,50,"[ Partition Size ]","Gigabytes",cfg.partitions[1]);
                    strcat(cfg.partitions[1]," G");
                }
                // Root partition size
                inputbox(13,55,"[ Partition Size ]","Please input the size of the root partition~This will be in Gigabytes~Leave empty to use whole drive",cfg.partitions[2]);
                if(strcmp(cfg.partitions[2],"")){
                    strcat(cfg.partitions[2]," G");
                }
            }
            else if(index == 2){
                inputbox(10,55,"[ Custom Command ]","Please enter the linux command that is to be run after the partitioning section",cfg.Cust_commands[0]);
            }
        }
        // Formatting
        else if (selection==1){timed_msgbox(10,85,"[ Important ]","Welcome to this Arch installation script.~This script can only so much~Follow the suggestions and read the UI otherwise the installation may not work.~This script is not responsible if the installation goes astray.",2);
    
            timed_msgbox(7,90,"[ IMPORTANT ]","THIS SECTION IS SUGGESTED TO BE KEPT THE SAME UNLESS YOU KNOW WHAT YOU ARE DOING~This option usually doesn't provide much use unless you have a odd partitioning scheme.",0);
            index=menu(8,90,"[ Formatting ]","Simple - will make the partitions F32 and ext4 for boot and root respectivly as default~Advanced-Select a custom format for the 2 partitions~Custom command - Add a command to be run at the end of the formating section~[ EXIT ]");
            if (index==1){
                timed_msgbox(10,82,"[ INFORMATION ]","You have decided to go to the advanced option~This options means you will select a format for the boot and root respectivly*~ ~ ~ ~*(in that order)",3);
                if ((yesno(12,90,"[ yesno ]","The 'Select' option will be to select be between the 2 options F32 and ext4 for the 2 partitions~and the 'Type' option is to type the command to excecute for formatting~(ONLY SELECT THE TYPE IF YOU KNOW WHAT YOUR DOING)","[ Select ]","[ Type ]"))){
                    if(yesno(12,70,"[ Format selection ]","You have 2 options to select from F32 or ext4 for the formatting the boot partition~fat 32 is default","[ F32 ]","[ ext4 ]")){
                        strcpy(cfg.format[0],"fat32");
                    }
                    else{
                        strcpy(cfg.format[0],"ext4");
                    }
                    if(yesno(12,70,"[ Format selection ]","You have 2 options to select from F32 or ext4 for the formatting the root partition~ext4 is default","[ F32 ]","[ ext4 ]")){
                        strcpy(cfg.format[1],"fat32");
                    }
                    else{
                        strcpy(cfg.format[1],"ext4");
                    }
                }
                else{
                    inputbox(8,55,"","",cfg.format[0]);
                    inputbox(8,55,"","",cfg.format[1]);
                }
            }
            else if (index==2){
                inputbox(10,55,"[ Custom Command ]","Please enter the linux command that is to~be run after the partitioning section",cfg.Cust_commands[1]);
            }
        }
        // Mounting
        else if (selection==2){
            //Informs the user of the dangers of modifying this section
            timed_msgbox(7,90,"[ IMPORTANT ]","This section is another very important section which~shouldn't be touched unless you know what you are modifying",2);
            index=menu(6,90,"[ Mounting ]","Simple- Mounts the boot partition to /mnt/boot/efi/ and root partition to /mnt~Advanced - Allows you to modify the mounting location of the paritions~Custom command - Add a command to be run at the end of the mounting section~[ EXIT ]");
            if (index==1){
                //Tells the user what to write in this section
                timed_msgbox(8,90,"[ INFORMATION ]","You are going to type the mountpoint of the boot then the root partition.~The boot partition should be inside the root partition.~if the mountpoints are modified then you will more than likely~fix things in the bootloader (grub or the one of your choice)",3);
                inputbox(10,85,"[ Mounting point ]","This is the mountpoint for the boot partition~make sure that you have the '/' before the mountpoint~i.e /mnt/boot/efi <-- the default",cfg.mounting[0]);
                inputbox(10,85,"[ Mounting point ]","This is the mountpoint for the root partition~make sure that you have the '/' before the mountpoint~i.e /mnt <-- the default",cfg.mounting[1]);
            }
            else if (index==2){
                inputbox(10,55,"[ Custom Command ]","Please enter the linux command that is to~be run after the mounting section",cfg.Cust_commands[2]);
            }
        }
        // Additional applications
        else if (selection==3){
            cfg.grub=yesno(8,80,"[ Bootloader ]","Do you want to use grub as your bootloader~If you select no then you need to install your own bootloader and modify it~If you are unsure then go with yes","[ Yes ]","[ No ]");
            inputbox(9,130,"[ Applications ]","Enter the applications you want to install. Each application should have a space between themselves.~For example to install firefox and network manager you would input 'firefox networkmanager'~It is suggested to have a network interface like the networkmanager application",message);
            if(yesno(6,50,"[ Custom Command ]","Do you want to add a Custom command.~To the end of this section","[ Yes ]","[ No ]")){
                inputbox(10,75,"[ Custom Command ]","Please enter the linux command that is to~be run after the applications have been installed (finilisation)",cfg.Cust_commands[2]);
            }
        }
        // Configurations
        else if(selection == 4){
            configc("Configs/", &cfg);
        }

        // Running the program
        else if(selection == 5){
            // Run installation logic
        }
    }

    endwin();
    return 0;
}
