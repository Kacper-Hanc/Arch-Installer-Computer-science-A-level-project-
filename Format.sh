drive=$1
format1=$2
format2=$3

if [[ $2 == "fat32" ]]
then
    mkfs.fat -F 32 /dev/$drive"1"
else
    mkfs.ext4 /dev/$drive"1"
fi
if [[ $3 == "fat32" ]]
then
    mkfs.fat -F 32 /dev/$drive"3"
else
    mkfs.ext4 /dev/$drive"3"
fi
mkswap /dev/$drive"2"