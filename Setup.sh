# TUI Control explanation
whiptail --title 'TUI Controls' --msgbox \
    'To manuver the Terminal User Interface the controls are shown bellow:\n
        1.<^> arrow keys to move across the options
        2.The space bar is to select option
        3.The enter/return key is used to confirm the 
        selected option\n
Thats all for the controls to manuver the UI to continue to the Networking section press enter'  20 60 --nocancel
for i in {1..4}; do
    per=$(($i*100/4))
    sleep 0.1
    export TERM=linux
    echo $per | whiptail --title "IMPORTANT MESSAGE" --gauge \
        "Please remember that you cannot return to previous interfaces and after you select an option the action will be complete. If you want to cancel you need to press CTRL+C before this message disapears do not cancel after this you device will lose it's data" \
        18 50 $per
    sleep 2
    export TERM=linux
done
# Set keymap
list=$(localectl list-keymaps | awk '{print $1" ."}')
km=$(whiptail --title "Keymaps" --nocancel --menu "" 30 65 20 $list 3>&1 1>&2 2>&3)
loadkeys $km
# Networking Setup

internet_check(){
    if : >/dev/tcp/8.8.8.8/53; then
        echo true
    else
        echo false
    fi
}
network=$( internet_check )
while true; do
    network=$( internet_check )
    if (whiptail --title 'Networking' --yesno \
    'You need to connect to the internet to do so you need to decide if you will connect with either ethernet or wifi\n
    \nThis Menu will reapear if you are not connected to the internet' \
    --yes-button 'wifi' --no-button 'ethernet' 20 40); then
    nmtui
    fi
    if [ $network == true ]; then
        break
    fi
done
./Partition.sh