#!/usr/bin/env bash

CONFIG_DIR="$HOME/.config/eww-which-key"

togleBase=false

while getopts 'bh:' OPTION; do
  case "$OPTION" in
  b)
    toggleBase=true
    ;;
  h)
    echo "you have supplied the -h option it is TODO tho :)"
    ;;
  ?)
    echo "script usage: $(basename \$0) [-d] [-h]" >&2
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
  grouped=$(echo "$merged" | jq -c '[ _nwise('$columns') ]')
  # echo "$grouped"
  eww --config "$CONFIG_DIR" open which-key --arg bindsJson="$grouped"
}

submap=$(echo "$1" | jq -c '.submap' | sed 's/"//g')

if [[ $toggleBase == true ]]; then
  isActive=$(eww --config "$CONFIG_DIR" active-windows | grep "which-key")
  if [[ $isActive == "" ]]; then
    openWidget "" 3
  else
    eww --config "$CONFIG_DIR" close which-key
  fi
  exit
fi

if [[ $submap == "" ]]; then
  eww --config "$CONFIG_DIR" close which-key
else
  openWidget "$submap"
fi
