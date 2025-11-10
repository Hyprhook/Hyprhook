#!/usr/bin/env bash

CONFIG_DIR="$HOME/.config/eww-which-key"

togleBase=false

print_help() {
  cat <<EOF
Usage: $(basename "$0") [OPTIONS]

Description:
  This script interacts with the 'eww' and 'hyprctl' utilities to manage widget visibility based on keyboard bindings.
  It retrieves key bindings and displays them in a dynamically calculated layout using 'eww'.

Options:
  -b            Displays all keybinds that are not in a submap. Parameters are ignored.
  -h            Displays help message and exit.
  
Examples:
  $(basename "$0") <submap>
      Shows the keybinds with the given submap.

  $(basename "$0") -b
      Toggles the keybinds with no submap.

  $(basename "$0") -h
      Shows this help message.

EOF
}

while getopts 'bh' OPTION; do
  case "$OPTION" in
  b)
    toggleBase=true
    ;;
  h)
    print_help
    exit 0
    ;;
  ?)
    print_help >&2
    exit 1
    ;;
  esac
done

openWidget() {
  binds=$(hyprctl binds -j | jq -c '[ .[] | select(.submap | . == "'"$1"'") ]')
  deleted=$(echo "$binds" | jq -c '[ .[] | select(.arg | contains("reset") | not ) ]')
  columns=$(echo "$deleted" | jq 'length / 4' | jq -c 'round')
  if [[ $2 != "" ]]; then
    columns=$(echo "$deleted" | jq 'length / '"$2"'' | jq -c 'round')
  fi
  if [[ columns -lt 4 ]]; then
    columns=4
  fi
  submapGroup=$(echo "$deleted" | jq -c '[ .[] | select(.dispatcher | contains("submap")) ]')
  submapGroupSorted=$(echo "$submapGroup" | jq -c ' sort_by(.key) ')
  commandGroup=$(echo "$deleted" | jq -c '[ .[] | select(.dispatcher | contains("submap") | not) ]')
  commandGroupSorted=$(echo "$commandGroup" | jq -c ' sort_by(.key) ')
  merged=$(echo "$commandGroupSorted$submapGroupSorted" | jq -s 'reduce .[] as $x ([]; . + $x)')
  grouped=$(echo "$merged" | jq -c --argjson n "$columns" '
    def chunks(s):
      . as $in
      | reduce range(0; length; $n) as $i
          ([]; . + [$in[$i : ($i + $n)]]);
    chunks($n)
  ')
  eww --config "$CONFIG_DIR" open which-key --arg bindsJson="$grouped"
}

if [[ $toggleBase == true ]]; then
  isActive=$(eww --config "$CONFIG_DIR" active-windows | grep "which-key")
  if [[ $isActive == "" ]]; then
    openWidget "" 3
  else
    eww --config "$CONFIG_DIR" close which-key
  fi
  exit
fi

submap=$(echo "$1" | jq -c '.submap' | sed 's/"//g')

if [[ $submap == "" ]]; then
  eww --config "$CONFIG_DIR" close which-key
else
  openWidget "$submap"
fi
