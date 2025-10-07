#Get variables from the excecution command in Partition.sh
drive=$1
Advanced=$2
paritions=$(lsblk -ln -o NAME "/dev/"$drive | awk 'NR>1 {print $1,"."}')


#Drive check
if [[ $drive != "sd"* ]];
then
    drive=$drive"p"
    echo $drive
fi


if [ $Advanced == "True" ] 
then # ADVANCED AND MORE ADVANCED FORMAT+MOUNT
    if ( whiptail --title "More Advanced options" --yesno "The extra options will ask for the specific format type per drive and the mounting point\n       (DON'T CHOOSE UNLESS YOU KNOW WHAT YOU ARE DOING)" --no-button "Simpler" --yes-button "Extra options" 20 70 ); 
    then #MORE ADVANCED FORMAT+MOUNT
        Advanced_extra=True


        #FORMATTING
        selection=$(whiptail --title "FORMATTING" --cancel-button "Done" --menu "please make sure to format the partitions" 20 20 4 $paritions 3>&1 1>&2 2>&3)
        while [[ $selection == "$drive"* ]];
        do
            #Formating till the User is finished
            format=$(whiptail --title "Format style" --menu $selection 30 40 4 "Fat32" "for boot partition" "Swap" "for swap memory" "Ext4" "Linux file system for root" 3>&1 1>&2 2>&3)
            case "$format" in
                "Fat32") mkfs.fat -F 32 /dev/$selection
                ;;
                "Swap") mkswap /dev/$selection
                ;;
                "Ext4") mkfs.ext4 /dev/$selection
                ;;
            esac
            selection=$(whiptail --title "FORMATTING" --cancel-button "Done" --menu "please make sure to format the partitions" 20 20 4 $paritions 3>&1 1>&2 2>&3)
        done


        #MOUNTING
        selection=$(whiptail --title "MOUNTING" --cancel-button "Done" --menu "please make sure to Mount the partitions" 20 20 4 $paritions 3>&1 1>&2 2>&3)
        while [[ $selection == "$drive"* ]];
        do
            mount=$(whiptail --title "Mounting position" --inputbox "Enter your desired mounting position \n(MAKE SURE YOU START WITH / IT WILL USE mkdir COMMAND)\nif the partition is to be swap then just enter 'swap' in the text box bello" 30 50 3>&1 1>&2 2>&3)
            if [ $mount == "swap" ]; then
                swapon /dev/$selection
            else
                if ( whiptail --title "New directory" --yesno "do you need this directory to be created" 20 70);
                then
                    mkdir -p $mount
                fi
                mount /dev/$selection $mount 
            fi
            selection=$(whiptail --title "MOUNTING" --cancel-button "Done" --menu "please make sure to Mount the partitions" 20 20 4 $paritions 3>&1 1>&2 2>&3)
        done



    else #ADVANCED FORMAT+MOUNT
        Advanced_extra=False
        

        #FORMATTING
        selection=$(whiptail --title "paritions" --cancel-button "Done" --menu "please make sure to format the partitions" 20 20 4 $paritions 3>&1 1>&2 2>&3)
        while [[ $selection == "$drive"* ]];
        do
            #Formating till the User is finished
            format=$(whiptail --title "Format style" --menu $selection 30 40 4 "Fat32" "for boot partition" "Swap" "for swap memory" "Ext4" "Linux file system for root" 3>&1 1>&2 2>&3)
            case "$format" in
                "Fat32") mkfs.fat -F 32 /dev/$selection
                ;;
                "Swap") mkswap /dev/$selection
                ;;
                "Ext4") mkfs.ext4 /dev/$selection
                ;;
            esac
            selection=$(whiptail --title "paritions" --cancel-button "Done" --menu "please make sure to format the partitions" 20 20 4 $paritions 3>&1 1>&2 2>&3)
        done


        #MOUNTING
        selection=$(whiptail --title "paritions" --cancel-button "Done" --menu "please make sure to Mount the partitions" 20 30 4 $paritions 3>&1 1>&2 2>&3)
        while [[ $selection == "$drive"* ]];
        do
            mount=$(whiptail --title "Mounting position" --menu "What type of partition is this, make sure it matches your format selection" \
                20 70 3 "Root" "This should be your main drive the Ext4 formatted partition" "Boot" "This allows the system to launch | Fat32 formatted drive" "Swap" "Virtual memory to stop your computer from crashing when running out of ram" 3>&1 1>&2 2>&3)
            case "$mount" in
                "Root") 
                mount /dev/$selection /mnt
                ;;
                "Boot")
                mkdir -p /mnt/boot/efi
                mount /dev/$selection /mnt/boot/efi
                ;;
                "Swap")
                swapon /dev/$selection
                ;;
            esac
            selection=$(whiptail --title "paritions" --cancel-button "Done" --menu "please make sure to Mount the partitions" 20 20 4 $paritions 3>&1 1>&2 2>&3)
        done

    fi
else #SIMPLE FORMAT+MOUNT

    #Quick format
    mkfs.fat -F 32 /dev/$drive"1"
    mkswap /dev/$drive"2"
    mkfs.ext4 /dev/$drive"3"

    #Quick mount
    mount /dev/$drive"3" /mnt
    mkdir -p /mnt/boot/efi
    mount /dev/$drive"1" /mnt/boot/efi
    swapon /dev/$drive"2"
fi

