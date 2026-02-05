#include <ncurses.h>
#include <string.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define MAX_STR 200
#define AVG_Width 70
#define AVG_Height 10
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
int Open_Dir(char **result, const char *location)
{
    DIR *dir = opendir(location);
    struct dirent *ent;
    if (!dir)
        return 0;
    size_t i = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;
        i++;
    }

    rewinddir(dir);

    // allocate memory dynamically
    *result = malloc(i * sizeof(char *));
    if (!*result) {
        closedir(dir);
        return 0;
    }
    i=0;
    //  pass through the output to gather all the name
    while ((ent = readdir(dir)) != NULL) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;
        result[i++] = strdup(ent->d_name);
    }

    closedir(dir);
    // Sort the result into a alphabetical list
    int c,j;
    char s[100];
    for(c=0;c<i;c++){
        for(j=c+1;j<i;j++){
            if(strcmp(result[c],result[j])>0){
                strcpy(s,result[c]);
                strcpy(result[c],result[j]);
                strcpy(result[j],s);
            }
        }
    }
    // return the index +1 so 0 is that the file is empty
    return i;
}

int Commands(char **result, int km, char *location)
{
    FILE *fptr = NULL;
    char buff[MAX_STR]="";
    char line[20000];
    int i = 0;

    if (km == 1)
        fptr = popen("lsblk -dn -o NAME,SIZE", "r");
    else if (km == 2)
        fptr = popen("localectl list-keymaps", "r");
    else if (km == 3)
        fptr = popen("awk 'NR>18 { s=$1; for(j=2;j<=NF;j++) s=s\".\"$j; print NR-18, s}' /etc/locale.gen","r");
    
    if (!fptr) return 0;
    while (fgets(line, sizeof(line), fptr) && i < 500) {
        line[strcspn(line, "\n")] = '\0';
        result[i++] = strdup(line);
    }

    pclose(fptr);
    return i;
}

void array_to_string(char **array, char *str, int lim)
{
    char *p = str;

    for (int i = 0; i < lim; i++) {
        size_t len = strlen(array[i]);

        if (len > 0 && array[i][len - 1] == '\n')
            len--;

        memcpy(p, array[i], len);
        p += len;

        if (i < lim - 1)
            *p++ = '~';
    }

    *p = '\0';
}

void configc(char *location, Config *cfg)
{
    FILE *fptr;
    char config[200];
    strcpy(config,location);
    if(!(config[strlen(config)-1]=='/')){
        strcat(config,"/");
    }
    strcat(config,"config.conf");
    fptr = fopen(config,"w");

    if (fptr == NULL) return;

    fprintf(fptr, "keymap:\"%s\"\n", cfg->keymap);
    fprintf(fptr, "partition{\n - \"%s\"\n - \"%s\"\n - \"%s\"\n}\n", cfg->partitions[0], cfg->partitions[1], cfg->partitions[2]);
    fprintf(fptr, "format{\n - \"%s\"\n - \"%s\"\n}\n", cfg->format[0], cfg->format[1]);
    fprintf(fptr, "mount{\n - \"%s\"\n - \"%s\"\n}\n", cfg->mounting[0], cfg->mounting[1]);
    fprintf(fptr, "custom commands{\n - \"%s\"\n - \"%s\"\n - \"%s\"\n - \"%s\"\n}\n",
        cfg->Cust_commands[0],cfg->Cust_commands[1],cfg->Cust_commands[2],cfg->Cust_commands[3]);
    fprintf(fptr, "applications:\"%s\"\n", cfg->Applications);
    fprintf(fptr, "grub:\"%s\"", cfg->grub ? "true" : "false");
    fclose(fptr);
}
void configr(char *location, Config *cfg)
{   
    char filename[MAX_STR];
    strcpy(filename,location);
    strcat(filename,"config.conf");
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return;
    }
    
    int plen = 0, flen = 0, mlen = 0, clen = 0;
    char buff[15];
    char line[200];
    int current_array = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        // Singular variables
        if (sscanf(line, "keymap:\"%30[^\"]\"", cfg->keymap) == 1)
                continue;
        if (sscanf(line, "applications:\"%199[^\"]\"", cfg->Applications) == 1)
                continue;
        if (sscanf(line, "grub:\"%15[^\"]\"", buff) == 1){
            continue;
            if(buff=="true") cfg->grub=1;
            else cfg->grub=0;
        }

        // Arrays
        if (strstr(line, "partition{")) {
            current_array = 1;
            continue;
        }
        if (strstr(line, "format{")) {
            current_array = 2;
            continue;
        }
        if (strstr(line, "mount{")) {
            current_array = 3;
            continue;
        }
        if (strstr(line, "custom commands{")) {
            current_array = 4;
            continue;
        }

        // End of an Array
        if (strchr(line, '}')) {
            current_array = 0;
            continue;
        }

        if (current_array == 1 && plen < 3) {
            if (sscanf(line, " - \"%199[^\"]\"", cfg->partitions[plen]) == 1)plen++;
        }
        if (current_array == 2 && flen < 2) {
            if (sscanf(line, " - \"%199[^\"]\"", cfg->format[flen]) == 1)flen++;
        }
        
        if (current_array == 3 && mlen < 2) {
            if (sscanf(line, " - \"%199[^\"]\"", cfg->mounting[mlen]) == 1)mlen++;
        }
        if (current_array == 4 && clen < 4) {
            if (sscanf(line, " - \"%199[^\"]\"", cfg->Cust_commands[clen]) == 1)clen++;
        }

    }
    fclose(fp);
}
void Makedir(char *location)
{
    char buffer[MAX_STR]="mkdir -p ";
    strcat(buffer,location);
    system(buffer);

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
    char buffer[20000];
    snprintf(buffer, sizeof(buffer), "%s", options);

    char array[500][300];   // supports up to 300 items
    char *token = strtok(buffer, "~");

    while (token && c < 500) {
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
    char *result[500];
    char message[20000] = {0};
    char command[256];
    char buffer[200];
    int index=0;
    int selection=0;

    char *drive;
    char partname[15];
    char *hostname;
    char *area;
    char location[MAX_STR]="/usr/share/zoneinfo/";

    initscr();
    noecho();
    //raw();

    /*
        Network Check
    */
    while ((system(": >/dev/tcp/8.8.8.8/53"))){
        clear();
        msgbox(10,55,"[ Networking ]","You are not connected to the internet~Please connect on the next screen~" 
            "~IF YOU HAVE CONNECTED ALREADY~MOVE TO THE NEXT SCREEN AND WAIT");
        getch();
        system("nmtui");
    }

    /*
        Keymap
    */
    refresh();
    timed_msgbox(6,80,"[ Keymap ]","Select a keymap which is similar to your keyboard on the next screen",0);
    
    selection = Commands(result,2,"");
    array_to_string(result,message,selection);
    index = menu(40,60,"[ Keymap ]",message);
    strcpy(command,"loadkeys ");
    strcat(command,result[index]);
    system(command);
    strcpy(cfg.keymap,result[index]);
    for(int i=0;i<selection;i++)free(result[i]);
    clear();
    refresh();

    timed_msgbox(10,85,"[ Important ]","Welcome to this Arch installation script.~This script can only so much~Follow the suggestions and read the UI otherwise the"
        " installation may not work.~This script is not responsible if the installation goes astray.~This will be your final chance to exit the program.",2);
    /*
        Main Menu
    */
    while(selection != 6){
        selection = menu(15,60,"[ Arch Installation ]","Partitioning - How to split the drive for the OS~Formatting(For Advanced Users)~Mounting(For Advanced Users)"
            "~Additional Applications to install~Configurations~Run Program~[ EXIT ]");
        // Partitioning
        if(selection == 0){
            index = menu(8,95,"[ Partitioning Scheme ]","Simple - Will remove all the data on the drive and install Arch with a basic partition~Advanced - You will need to"
                " decide on the size of the partitions your self~Custom command - Add a command to be run at the end of the partitioning section~[ EXIT ]");
            if(index == 1){
                timed_msgbox(12,72,"[ INFORMATION ]","You have decided to go to the advanced option~You will now create 3 partitions These partitions will be as follows:"
                    "~boot,swap and root~These partitions are necesary to the function of your computer.",3
                );

                // Boot partition size
                if(yesno(8,85,"[ Partition Size ]","You are now deciding the size of the partition you are creating~This partition will be the the [boot] parition so it is"
                    " suggested to select M~The M and G stand for Megabytes and Gigabytes.","[ M ]","[ G ]"))
                {
                    inputbox(11,50,"[ Partition Size ]","You have selected Megabytes~Please input the size of the [boot] partition",cfg.partitions[0]);
                    strcat(cfg.partitions[0]," M");
                } else {
                    inputbox(11,50,"[ Partition Size ]","You have selected Gigabytes~Please input the size of the [boot] partition",cfg.partitions[0]);
                    strcat(cfg.partitions[0]," G");
                }

                // Mount partition size
                if(yesno(8,85,"[ Partition Size ]","You are now deciding the size of the partition you are creating~This partition will be the the mount parition so it is"
                    " suggested to select M~The M and G stand for Megabytes and Gigabytes.","[ M ]","[ G ]"))
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
        else if (selection==1){
            timed_msgbox(10,85,"[ Important ]","Welcome to this Arch installation script.~This script can only so much"
                "~Follow the suggestions and read the UI otherwise the installation may not work.~This script is not responsible if the installation goes astray.",2);
    
            timed_msgbox(7,90,"[ IMPORTANT ]","THIS SECTION IS SUGGESTED TO BE KEPT THE SAME UNLESS YOU KNOW WHAT YOU ARE DOING~This option usually doesn't provide much "
                "use unless you have a odd partitioning scheme.",0);
            index=menu(8,90,"[ Formatting ]","Simple - will make the partitions F32 and ext4 for boot and root respectivly as default~Advanced-Select a custom format for "
                "the 2 partitions~Custom command - Add a command to be run at the end of the formating section~[ EXIT ]");
            if (index==1){
                timed_msgbox(10,82,"[ INFORMATION ]","You have decided to go to the advanced option~This options means you will select a format for the boot and root "
                    "respectivly*~ ~ ~ ~*(in that order)",3);
                if ((yesno(12,90,"[ yesno ]","The 'Select' option will be to select be between the 2 options F32 and ext4 for the 2 partitions~and the 'Type' option is "
                    "to type the command to excecute for formatting~(ONLY SELECT THE TYPE IF YOU KNOW WHAT YOUR DOING)","[ Select ]","[ Type ]"))){
                    if(yesno(12,70,"[ Format selection ]","You have 2 options to select from F32 or ext4 for the formatting the boot partition~fat 32 is default",
                        "[ F32 ]","[ ext4 ]")){
                        strcpy(cfg.format[0],"fat32");
                    }
                    else{
                        strcpy(cfg.format[0],"ext4");
                    }
                    if(yesno(12,70,"[ Format selection ]","You have 2 options to select from F32 or ext4 for the formatting the root partition~ext4 is default",
                        "[ F32 ]","[ ext4 ]")){
                        strcpy(cfg.format[1],"fat32");
                    }
                    else{
                        strcpy(cfg.format[1],"ext4");
                    }
                }
                else{
                    msgbox(8,20,"[ Information ]","To create a format you need to use one of the commands from linux to format the partition~ie 'mkfs.fat -F 32 '"
                        " that command would format the partition to fat 32");
                    inputbox(8,55,"[ Custom format ]","Please input the command to format the boot partition",cfg.format[0]);
                    inputbox(8,55,"[ Custom format ]","Please input the command to format the root partition",cfg.format[1]);
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
            index=menu(6,90,"[ Mounting ]","Simple- Mounts the boot partition to /mnt/boot/efi/ and root partition to /mnt~Advanced - Allows you to modify the mounting "
                "location of the paritions~Custom command - Add a command to be run at the end of the mounting section~[ EXIT ]");
            if (index==1){
                //Tells the user what to write in this section
                timed_msgbox(8,90,"[ INFORMATION ]","You are going to type the mountpoint of the boot then the root partition.~The boot partition should be inside the "
                    "root partition.~if the mountpoints are modified then you will more than likely~fix things in the bootloader (grub or the one of your choice)",3);
                inputbox(10,85,"[ Mounting point ]","This is the mountpoint for the boot partition~make sure that you have the '/'"
                    " before the mountpoint~i.e /mnt/boot/efi <-- the default",cfg.mounting[0]);
                inputbox(10,85,"[ Mounting point ]","This is the mountpoint for the root partition~make sure that you have the '/'"
                    " before the mountpoint~i.e /mnt <-- the default",cfg.mounting[1]);
            }
            else if (index==2){
                inputbox(10,55,"[ Custom Command ]","Please enter the linux command that is to~be run after the mounting section",cfg.Cust_commands[2]);
            }
        }
        // Additional applications
        else if (selection==3){
            cfg.grub=yesno(8,80,"[ Bootloader ]","Do you want to use grub as your bootloader~If you select no then you need to install your own bootloader and modify it~"
                "If you are unsure then go with yes","[ Yes ]","[ No ]");
            inputbox(9,130,"[ Applications ]","Enter the applications you want to install. Each application should have a space between themselves.~For example to install"
                " firefox and network manager you would input 'firefox networkmanager'~It is suggested to have a network interface like the networkmanager application",
                message);
            if(yesno(6,50,"[ Custom Command ]","Do you want to add a Custom command.~To the end of this section","[ Yes ]","[ No ]")){
                inputbox(10,75,"[ Custom Command ]","Please enter the linux command that is to~be run after the applications have been installed (finilisation)",
                    cfg.Cust_commands[2]);
            }
        }
        // Configurations
        else if(selection == 4){
            msgbox(12,50,"[  ]","");
            index=menu(10,40,"[ Configurations ]","Save Configuration~Load Configuration~Add link to configuation~[ EXIT ]");
            if(index==0){
                inputbox(10,90,"[ Location ]","Enter the location where the file will be located~(The software will scan the directory 'Configs/')~"
                    "Don't Save all your configs to Configs/~save them in seperate folders ie 'Configs/folder/ <-- Make sure to have a / afterwards",message);
                Makedir(message);
                configc(message,&cfg);
            }
            else if(index==1){
                selection = Open_Dir(result,"Configs/");
                if(selection){
                    array_to_string(result,message,selection);
                    index = menu(12,30,"[ Configurations ]",message);
                    sprintf(message,"Partitioning(Boot:'%s',Swap:'%s',Root:'%s')~Formating(Boot:'%s',Root:'%s')~Mounting(Boot:'%s',Root:'%s')~Applications:'%s'~"
                        "Custom Commands-~(Partitioning:'%s',Formatting:'%s',Mounting:'%s',Applications:'%s')~Grub:",
                        cfg.partitions[0],cfg.partitions[1],cfg.partitions[2],cfg.format[0],cfg.format[1],cfg.mounting[0],cfg.mounting[1],cfg.Applications,
                        cfg.Cust_commands[0],cfg.Cust_commands[1],cfg.Cust_commands[2],cfg.Cust_commands[3]);
                    if(cfg.grub) strcat(message,"True");
                    else strcat(message,"False");
                    menu(12,90,"[ Selected Configuration options ]",message);
                }
                else timed_msgbox(7,30,"[ FILE DOESN'T EXIST]","There are no configs~In your Configs folder",2);
            }
            else if(index==2){
                inputbox(AVG_Height,90,"[ GITHUB ]","Enter a link to a github link of a configuration.~For those who want to have a github of their config "
                    "~you can have shell scripts or other programs which are called in the custom commands",message);
                
                inputbox(AVG_Height,90,"[ GITHUB ]","Enter the name of the configuration",buffer);
                sprintf(command,"Configs/%s",buffer);
                Makedir(command);
                sprintf(command," git clone %s Configs/%s",message,buffer);
                system(command);
                clear();
                refresh();
            }
        }

        // Running the program
        else if(selection == 5){
            raw();
            timed_msgbox(6,40,"","TO EMERGENCY STOP THE PROGRAM PRESS CTRL+C",2);

            /*
                Drive Selection
            */
            index = Commands(result,1,"");
            array_to_string(result,message,index);
            index = menu(6,29,"[ Drive Selection ]",message);
            drive = result[index];

            // Drive check ( some partitions are in the form 'Drive_name'x where the x is a number
            // and other paritions are in the form 'Drive_name'nx where the x is a number again but the n is the character p
            if (strstr(drive, "nvme") != NULL || strstr(drive, "mmcblk") != NULL) sprintf(partname, "%sp",drive);
            else strcpy(partname,drive);
   

            // Hostname

            inputbox(10,50,"[ Hostname ]","Enter the name of your device",hostname);

            // Partitioning

            msgbox(12,40,"","Partitioning your drive");
            sleep(1);
            flushinp();
            sprintf(command,"./Partition.sh %s %s %s %s",drive,cfg.partitions[0],cfg.partitions[1],cfg.partitions[2]);
            system(command);

            // Formatting

            msgbox(12,40,"","Formatting your drive");
            sleep(1);
            // If a single format is not in either fat32 or ext4 it will send the formats as commands
            if(cfg.format[0]=="fat32" || cfg.format[0]=="ext4") sprintf(command,"./Format.sh %s %s %s",partname,cfg.format[0],cfg.format[1]);
            else sprintf(command,"%s | %s | mkswap /dev/%s2",cfg.format[0],cfg.format[0],partname);
            system(command);

            // Mounting - Again if 1 of them is different then they should be informed as so

            if(cfg.mounting[1]!="/mnt"){
                msgbox(12,50,"[ INFORMATION ]","Make sure that after the installation is finished to propperly setup your boot loader");
                cfg.grub=0;
                Makedir(cfg.mounting[1]);
            }
            msgbox(6,30,"","Mounting Paritions");
            sleep(1);
            Makedir(cfg.mounting[0]);

            sprintf(command, "mount /dev/%s3 %s",partname,cfg.mounting[1]);
            system(command);
            sprintf(command, "mount /dev/%s1 %s",partname,cfg.mounting[0]);
            system(command);
            sprintf(command, "swapon /dev/%s2",partname);

            // System installation
            msgbox(6,40,"","Installing packages and applications");
            sleep(1);
            sprintf(command, "pacstrap -K %s %s",cfg.mounting[1],cfg.Applications);
            system(command);
            // Fstab initilisation and changeing root
            sprintf(command,"genfstab -U %s >> %s/etc/fstab",cfg.mounting[1],cfg.mounting[1]);
            system(command);
            sprintf(command, "arch-chroot %s",cfg.mounting[1]);
            system(command);


            // localisation and time
            selection = Open_Dir(result,location);
            array_to_string(result,message,selection);
            msgbox(8,60,"[ Locale ]","Select the locale which is closest to your region~and timezone");
            getch();
            index=menu(12,40,"",message);
            strcat(location,result[index]);
            selection = Open_Dir(result,location);
            
            if(selection!=0){
                array_to_string(result,message,selection);
                index=menu(12,40,"",message);
                sprintf(location,"%s/%s",location,result[index]);
            }
            selection=Commands(result,3,"");
            array_to_string(result,message,selection);
            index=menu(16,20,"[ Localisation ]",message);
            msgbox(AVG_Height,AVG_Width,"[ Configuring ]","Setting your keymap localisation and grub unless you've selected otherwise");
            sprintf(command,"./"/*MIGHT BE DIFFRENT LOCATION HERE*/"Config.sh %s %s %s %s %s %s",drive,cfg.keymap,location,hostname,index,cfg.grub);
            system(command);
            system("exit");
            
        }
    }

    endwin();
    return 0;
}