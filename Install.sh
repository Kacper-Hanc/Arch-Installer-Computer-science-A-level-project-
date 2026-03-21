#gets the additional apps the user wants to install the uses pacstrap to install them
apps=$(whiptail --title "Applications" --inputbox "Write in any apps to be installed" 15 25  3>&1 1>&2 2>&3)
if (whiptail --title "GRUB" --yesno "Do you want grub as your bootmanager" 15 25 3>&1 1>&2 2>&3) then
    pacstrap /mnt linux base base-devel sof-firmware grub efibootmgr $apps
    grub=True
else
    pacstrap /mnt linux base base-devel sof-firmware $apps
    grub=False
fi
#copies necesary scirpts
cp Config.sh /mnt/Config.sh
chmod +x /mnt/Config.sh
#generates the fstab file. Important for auto mounting drives during boot sequence
genfstab /mnt >> /mnt/etc/fstab
cat /mnt/etc/fstab
arch-chroot /mnt bash Config.sh $1 $2 $grub

umount -a 
swapoff /dev/$1
reboot
