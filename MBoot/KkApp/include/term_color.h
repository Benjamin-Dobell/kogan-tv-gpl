#ifndef _TERM_COLOR_H
#define _TERM_COLOR_H

#define TERM_NORM		"\033[0m"
#define TERM_FG_RED		"31"
#define TERM_FG_GREEN	"32"
#define TERM_FG_YELLOW	"33"
#define TERM_FG_BLUE	"34"
#define TERM_FG_PURPLE	"35"
#define TERM_FG_CYAN	"36"
#define TERM_FG_GRAY	"37"

#define TERM_BG_BLACK	"40"
#define TERM_BG_RED		"41"
#define TERM_BG_GREEN	"42"
#define TERM_BG_YELLOW	"43"
#define TERM_BG_BLUE	"44"
#define TERM_BG_PURPLE	"45"
#define TERM_BG_CYAN	"46"
#define TERM_BG_GRAY	"47"

#define PRINTF_CLF(fg, fmt, args...)	printf ("\033[" fg "m" fmt TERM_NORM, ##args)
#define PRINTF_CLFB(fg, bg, fmt, args...)	printf ("\033[" fg ";" bg "m" fmt TERM_NORM, ##args)
#define PRINTF_CLFBA(attr, fg, bg, fmt, args...)	printf("\033[" attr ";" fg ";" bg "m" fmt TERM_NORM, ##args)

#endif

