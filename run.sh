#!/bin/bash

dirs=$(find . -mindepth 1 -maxdepth 1 -type d -exec test -f "{}/Makefile" \; -print)

fzf_options=(
    "--border=sharp"
    "--border-label=Select directory to run make run: "
    "--height=~100%"
    "--highlight-line"
    "--no-input"
    "--pointer="
    "--reverse"
)

if [ -z "$dirs" ]; then
    echo "No directories with Makefile found."
    exit 1
fi

selected=$(printf "%s\n" "${dirs[@]}" | sort | fzf "${fzf_options[@]}") || exit 0

if [ -z "$selected" ]; then
    echo "No directory selected."
    exit 1
fi

cd "$selected" || exit 1
echo "Running 'make run' in $selected ..."
make run
