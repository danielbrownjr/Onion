#!/bin/bash

mkdir -p cache
cd cache

if ! wget -O featured.txt https://raw.githubusercontent.com/OnionUI/Themes/main/.github/data/featured.txt > /dev/null 2>&1; then
    echo "-- failed to download featured theme list" >&2
    rm -f ./featured.txt
    exit 1
fi
featured=`cat ./featured.txt`
rm -f ./featured.txt

if [ -z "$featured" ]; then
    echo "-- featured theme list is empty" >&2
    exit 1
fi

readarray -t themes <<< "$featured"

f() { themes=("${BASH_ARGV[@]}"); }

shopt -s extdebug
f "${themes[@]}"
shopt -u extdebug

mkdir -p ../dist/Themes

for element in "${themes[@]}"
do
    zipfile="$element.zip"

    if [[ ! -f "$zipfile" ]]
    then
        echo "-- downloading theme: $element"
        # Download to a temp name so a failed download is never cached as a valid zip
        if ! wget -O "$zipfile.part" "https://github.com/OnionUI/Themes/raw/main/release/$element.zip" -q --show-progress; then
            echo "-- failed to download theme: $element" >&2
            rm -f "$zipfile.part"
            exit 1
        fi
        mv "$zipfile.part" "$zipfile"
    fi

    if [ "$element" == "Silky by DiMo" ]; then
        echo "-- extracting theme: $element"
        unzip -oq "$zipfile" -d ../dist/Themes
    else
        echo "-- copying theme: $element"
        cp "$zipfile" ../dist/Themes
    fi
done
