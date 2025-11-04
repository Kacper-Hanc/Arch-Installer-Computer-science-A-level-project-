#How I want the code to work
Dualboot=T/F
if Dualboot; then
    pacstrap -K /mnt firmware-and-software-necessary-for-dualboot-installation
else
    pacstrap -K /mnt firmware-and-software-for-a-basic-installation
fi

genfstab -U /mnt >> /mnt/etc/fstab
arch-chroot /mnt

ln -sf /usr/share/zoneinfo/Region/City /etc/localtime
hwclock --systohc

userselectedlocale >> /etc/locale.conf
locale-gen
userselectedkeymap >> /etc/vconsole.conf

userselectedhostname >> /etc/hostname

setpassword root userselectedpassword

if Dualboot; then
    bootloadersetupfordualboot
else
    bootloadersetupforbasicinstall
fi


##########################################################################
#           THIS IS A NON FINISHED SCRIPT NEEDS TO                       #
#           MODIFIED AND USER CONFIGURATION NEEDS TO BE MADE             #
##########################################################################
pacstrap -K /mnt base linux linux-firmware sof-firmware networkmanager base-devel grub efibootmgr


#configuration
#Fstab
genfstab -U /mnt >> /mnt/etc/fstab
arch-chroot /mnt
#Time & locale
ln -sf /usr/share/zoneinfo/Region/City /etc/localtime
hwclock --systohc
echo LANG=en_US.UTF-8 >> /etc/locale.conf
locale-gen
echo KEYMAP=uk >> /etc/vconsole.conf

HOSTNAME!!

passwd


BOOTLOADER!!