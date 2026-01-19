#sets date and time and syncs hardware clock
area=$(whiptail --title "Area" --menu "Select your current area" 30 35 15 $(ls /usr/share/zoneinfo| awk 'NR>1 {print $1,"."}') 3>&1 1>&2 2>&3)
location=$(whiptail --title "Location" --menu "Select your location" 30 35 15 $(ls /usr/share/zoneinfo/$area| awk 'NR>1 {print $1,"."}') 3>&1 1>&2 2>&3) || location=""
if [ -z "$location" ]; then
    ln -sf /usr/share/zoneinfo/$area /etc/localtime
else
    ln -sf /usr/share/zoneinfo/$area/$location /etc/localtime
fi
hwclock --systohc
#generates locales and also pushes them to locale.conf
menu_items=$(awk 'NR>18 {gsub(/^#/,""); s=$1; for(i=2;i<=NF;i++) s=s"."$i; print NR-18, s}' /etc/locale.gen)
selected_locale=$(whiptail --title "Select Locale" \
  --menu "Choose your locale based on your keyboard layout and current location:" 20 60 10 \
  $menu_items 3>&1 1>&2 2>&3)
file_line=$((selected_locale + 18))
awk -v line="$file_line" 'NR==line {gsub(/^#/,""); s=$1; if($2) s=s" "$2; print s}' /etc/locale.gen >> /etc/locale.gen
locale-gen
echo LANG=$(awk '!/^#/ && NF {print $1}' /etc/locale.gen) >> /etc/locale.conf


echo KEYMAP=$2 >> /etc/vconsole.conf
echo $(whiptail --title "HOSTNAME" --inputbox "Enter the name of your machine:" 20 30 3>&1 1>&2 2>&3) >> /etc/hostname

grub-install /dev/$1
grub-mkconfig -o /boot/grub/grub.cfg