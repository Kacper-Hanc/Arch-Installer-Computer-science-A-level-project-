drive=$1
format1=$2
format2=$3


if [[ $format1=="fat32" ]]
then
    mkfs.fat -F 32 /dev/$drive"1"
elif [[ $format1=="ext4 "]]
then
    mkfs.ext4 /dev/$drive"1"
fi
if [[ $format2=="fat32" ]]
then
    mkfs.fat -F 32 /dev/$drive"3"
elif [[ $format2=="ext4 "]]
then
    mkfs.ext4 /dev/$drive"3"
fi

mkswap /dev/$drive"2"