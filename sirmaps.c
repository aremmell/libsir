#include "sirmaps.h"

/**
 * @brief Overrides for ::sir_level <-> ::sir_textstyle mappings.
 *
 * ::sir_settextstyle sets (overrides) the style values in this array
 * at runtime; only the SIRL_* values are constant.
 * 
 * ::sir_default_style_map in @ref sirdefaults.h contains the constant,
 * default styles for each level.
 * 
 * @attention Entries *must* remain in numeric ascending order (by SIRL_*);
 * binary search is used to look up entries based on those values.
 */
sir_level_style_pair sir_override_style_map[SIR_NUMLEVELS] = {
    {SIRL_EMERG,  SIRS_INVALID},
    {SIRL_ALERT,  SIRS_INVALID},
    {SIRL_CRIT,   SIRS_INVALID},
    {SIRL_ERROR,  SIRS_INVALID},
    {SIRL_WARN,   SIRS_INVALID},
    {SIRL_NOTICE, SIRS_INVALID},
    {SIRL_INFO,   SIRS_INVALID},
    {SIRL_DEBUG,  SIRS_INVALID}
};

/**
 * @brief Mapping of ::sir_level <-> string representation (@ref sirconfig.h)
 * 
 * ::_sir_levelstr obtains string representations from this array
 * for output formatting.
 * 
 * @attention Entries *must* remain in numeric ascending order (by SIRL_*);
 * binary search is used to look up entries based on those values.
 */
const sir_level_str_pair sir_level_str_map[SIR_NUMLEVELS] = {
    {SIRL_EMERG,  SIRL_S_EMERG},
    {SIRL_ALERT,  SIRL_S_ALERT},
    {SIRL_CRIT,   SIRL_S_CRIT},
    {SIRL_ERROR,  SIRL_S_ERROR},
    {SIRL_WARN,   SIRL_S_WARN},
    {SIRL_NOTICE, SIRL_S_NOTICE},
    {SIRL_INFO,   SIRL_S_INFO},
    {SIRL_DEBUG,  SIRL_S_DEBUG}
};

/** 
 * @brief Mapping of ::sir_textstyle <-> values used to generate
 * styled terminal output for 4-bit (16-color) mode.
 * 
 * @attention Entries *must* remain in numeric ascending order
 * (by SIRS_*); binary search is used to look up entries based
 * on those values.
 */
const sir_style_16color_pair sir_style_16color_map[SIR_NUM16_COLOR_MAPPINGS] = {
    /* intensity */
    {SIRS_NONE,          0},
    {SIRS_BRIGHT,        1},
    {SIRS_DIM,           2},
    /* foreground color */
    {SIRS_FG_BLACK,     30},
    {SIRS_FG_RED,       31},
    {SIRS_FG_GREEN,     32},
    {SIRS_FG_YELLOW,    33},
    {SIRS_FG_BLUE,      34},
    {SIRS_FG_MAGENTA,   35},
    {SIRS_FG_CYAN,      36},
    {SIRS_FG_LGRAY,     37},
    {SIRS_FG_DEFAULT,   39},
    {SIRS_FG_DGRAY,     90},
    {SIRS_FG_LRED,      91},
    {SIRS_FG_LGREEN,    92},
    {SIRS_FG_LYELLOW,   93},
    {SIRS_FG_LBLUE,     94},
    {SIRS_FG_LMAGENTA,  95},
    {SIRS_FG_LCYAN,     96},
    {SIRS_FG_WHITE,     97},
    /* background color */
    {SIRS_BG_BLACK,     40},
    {SIRS_BG_RED,       41},
    {SIRS_BG_GREEN,     42},
    {SIRS_BG_YELLOW,    43},
    {SIRS_BG_BLUE,      44},
    {SIRS_BG_MAGENTA,   45},
    {SIRS_BG_CYAN,      46},
    {SIRS_BG_LGRAY,     47},
    {SIRS_BG_DEFAULT,   49},
    {SIRS_BG_DGRAY,    100},
    {SIRS_BG_LRED,     101},
    {SIRS_BG_LGREEN,   102},
    {SIRS_BG_LYELLOW,  103},
    {SIRS_BG_LBLUE,    104},
    {SIRS_BG_LMAGENTA, 105},
    {SIRS_BG_LCYAN,    106},
    {SIRS_BG_WHITE,    107}
};
