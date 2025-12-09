
menu_items=$(awk 'NR>18 {gsub(/^#/,""); s=$1; for(i=2;i<=NF;i++) s=s"."$i; print NR-18, s}' /etc/locale.gen)
selected_locale=$(whiptail --title "Select Locale" \
  --menu "Choose your locale:" 20 60 10 \
  $menu_items 3>&1 1>&2 2>&3)
file_line=$((selected_locale + 18))
awk -v line="$file_line" 'NR==line {gsub(/^#/,""); s=$1; if($2) s=s"."$2; print s}' /etc/locale.gen >> /etc/locale.gen