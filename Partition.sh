drives=$1
if [ "$2"==0 ]
then
    sfdisk "/dev/$Drive" <<EOF
    size=500M, type=83
    size=4G,   type=83
    type=83
EOF
else
    sfdisk "/dev/$Drive" <<EOF
    size=$3,   type=83
    size=$4,   type=83
    type=83
EOF
fi
