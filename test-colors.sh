#!/usr/bin/env zsh

FG=38
DEF_FG=39
BG=48
DEF_BG=49
COLOR_256=5
COLOR_RGB=2

ANSI_ESC="\x1b["
ANSI_ESC_RST="${ANSI_ESC}0m"

# $1 = fg color
# $2 = bg color
# $3 = msg
echo_256_color_msg() {
    if [[ -z $1 ]]; then
        # background only:
        # \esc[39;5;48;5;<bg-color>m
        echo "${ANSI_ESC}${DEF_FG};${COLOR_256};${BG};${COLOR_256};${2}m${3}${ANSI_ESC_RST}"
    elif [[ -z $2 ]]; then
        # foreground only:
        # \esc[<attr>;38;5;<fg-color>;49;5m
        echo "${ANSI_ESC}${FG};${COLOR_256};${1};${DEF_BG};${COLOR_256}m${3}${ANSI_ESC_RST}"
    else
        # foreground and background:
        # \esc[<attr>;38;5;<fg-color>;48;5;<bg-color>m
        echo "${ANSI_ESC}${FG};${COLOR_256};${1};${BG};${COLOR_256};${2}m${3}${ANSI_ESC_RST}"
    fi
}

# $1 = fg color (e.g. "23;56;82")
# $2 = bg color
# $3 = msg
echo_RGB_color_msg() {
    if [[ -z $1 ]]; then
        # background only:
        # \esc[39;2;48;2;<r;g;b>m
        echo "${ANSI_ESC}${DEF_FG};${COLOR_RGB};${BG};${COLOR_RGB};${2}m${3}${ANSI_ESC_RST}"
    elif [[ -z $2 ]]; then
        # foreground only:
        # \esc[<attr>;38;2;<r;g;b>;49;2m
        echo "${ANSI_ESC}${FG};${COLOR_RGB};${1};${DEF_BG};${COLOR_RGB}m${3}${ANSI_ESC_RST}"
    else
        # foreground and background:
        # \esc[<attr>;38;2;<r;g;b>;48;2;<r;g;b>m
        echo "${ANSI_ESC}${FG};${COLOR_RGB};${1};${BG};${COLOR_RGB};${2}m${3}${ANSI_ESC_RST}"
    fi
}

#echo "Testing 256-color output..."
#echo_256_color_msg "171" "" "256: Foreground only"
#echo_256_color_msg "" "49" "256: Background only"
#echo_256_color_msg "196" "223" "256: Both foreground and background"

#echo "Testing RGB output..."
#echo_RGB_color_msg "23;223;82" "" "RGB: Foreground only"
#echo_RGB_color_msg "" "23;223;82" "RGB: Background only"
#echo_RGB_color_msg "255;255;255" "0;0;0" "RGB: Both foreground and background"

for n in {17..255}; do
    echo "SIR_CLR_${n}       = ${n},"
done
