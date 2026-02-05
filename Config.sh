##sets date and time and syncs hardware clock
drive=$1
keymap=$2
location=$3
hostname=$4
selected_locale=$5
grub=$6

file_line=$(($selected_locale+18)) 

ln -sf $location /etc/localtime
hwclock --systohc
#generates locales and also pushes them to locale.conf



awk -v line="$file_line" 'NR==line {gsub(/^#/,""); s=$1; if($2) s=s" "$2; print s}' /etc/locale.gen >> /etc/locale.gen
locale-gen
echo LANG=$(awk '!/^#/ && NF {print $1}' /etc/locale.gen) >> /etc/locale.conf


echo KEYMAP=$2 >> /etc/vconsole.conf
echo $hostname >> /etc/hostname
if [[ $grub==1 ]]
then
  grub-install /dev/$1
  grub-mkconfig -o /boot/grub/grub.cfg
fi