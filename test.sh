Drive=$1
sfdisk "/dev/$Drive" <<EOF
size=500M, type=83
size=4G,   type=83
type=83
EOF