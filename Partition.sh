drive=$1
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

