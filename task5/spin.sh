#!/bin/bash

CONF="$HOME/.config/hypr/hyprland.conf"
TMP="$CONF.tmp"

awk 'NR<=3 {sub(/^#/,"")}1' "$CONF" > "$TMP"
mv "$TMP" "$CONF"

hyprctl reload

sleep 5

awk 'NR<=3 {sub(/^/,"#")}1' "$CONF" > "$TMP"
mv "$TMP" "$CONF"
hyprctl reload