drive=$1
device="/dev/$drive"
if [[ $drive == *"nvme"* ]] || [[ $drive == "mmcblk" ]]
then
    $device="${device}p"
fi
umount "${device}"* 2>/dev/null || true
swapoff "${device}"*
for i in {1..3}
do
    wipefs -a "$device$i"
done


if [[ $4 == "" ]]
then
    sfdisk "/dev/$drive" <<EOF
    size=$2, type=83
    size=$3,   type=83
    type=83
EOF
else
    sfdisk "/dev/$drive" <<EOF
    size=$2,   type=83
    size=$3,   type=83
    size=$4,   type=83
EOF
fi