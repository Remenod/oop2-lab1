#!/bin/bash

CONF="$HOME/.config/hypr/shader_block.conf"
CONF1="$HOME/.config/hypr/_shader_block.conf"

cat $CONF1 > $CONF

hyprctl reload

sleep 5

echo "" > $CONF

hyprctl reload