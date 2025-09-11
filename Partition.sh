drives="$(lsblk -dn -o NAME,SIZE)"

for i in {0..3}; do
    messages=("You are going to partition your drive now, \nThis means that you are going to split your drive into sections"
    "These sections are where your OS is going to be installed, We do this to make sure each part is installed in the correct place"
    "you are now going to select wheather you are going to go with the default partition scheme(Simple) or create a custom partition scheme(Advanced)\n \nyou will now select the drive that the OS will be installed on"
    "             THIS IS IMPORTANT \n\nThis will wipe the drive you select in the next prompt")

    whiptail --title "IMPORTANT Instructions" --msgbox \
        "${messages[$i]}" 18 50
done


# Gets the drive to partition
Drive=$(whiptail --title "Partitioning" --nocancel --menu "Choose the drive that the OS should be installed on" 15 25 4 \
    $drives  3>&1 1>&2 2>&3)

# Partitioning
if !(whiptail --title "Simple/Advanced" --yesno \
        "The simple mode will partition (split up) the drive automatically in a default configuration for those who understand it will in the shceme of 500M Boot 4G Swap and the rest will be allocated for the userspace/root There is also the Advanced option this will give you complete control of the partitioning scheme using cfdisk on the drive you have selected this is the suggested option if you are comftable with cfdisk or linux already
        \nThere will be a selection like this in all applicable sections" \
        --yes-button "Simple" --no-button "Advanced" 20 50); then
    sudo cfdisk /dev/$Drive
else
    # Automatically wipes drive and partitions it

    sfdisk "/dev/$Drive" <<EOF
    size=500M, type=83
    size=4G,   type=83
    type=83
EOF

fi