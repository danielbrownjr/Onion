#!/bin/sh
patch="$1"
config="$2"

if [ "$config" == "" ]; then
    config=/mnt/SDCARD/RetroArch/.retroarch/retroarch.cfg
fi

if [ ! -f "$patch" ]; then
    echo "Patch file doesn't exist"
    return 1
fi

if [ ! -f "$config" ]; then
    echo "Config file doesn't exist"
    return 1
fi

cat "$patch" | (
    count=0
    content=`cat "$config"` || exit 1
    original="$content"

    while read line; do
        key=`echo "$line" | sed 's/^\s*//g' | sed 's/\s*=.*$//g'`
        value=`echo "$line" | sed 's/^[^\"]*"//g' | sed 's/"[^\"]*$//g'`

        if echo "$content" | grep -q "^\s*$key\s*="; then
            content=`echo "$content" | sed "/$key\s*=/c\${key} = \"$value\""`
        else
            content="$content
$key = \"$value\""
        fi

        let count++
    done

    if [ "$content" = "$original" ]; then
        echo "0 lines changed"
        exit 0
    fi

    # Write to a temp file and rename it over the original, so a power loss
    # or full card mid-write can't leave a truncated retroarch.cfg
    if echo "$content" > "$config.tmp" && sync && mv -f "$config.tmp" "$config"; then
        :
    else
        rm -f "$config.tmp"
        exit 1
    fi

    echo "$count lines patched"
)
