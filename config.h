/* See LICENSE file for copyright and license details. */

/* Constants */
#define TERMINAL "st"
#define TERMCLASS "St"
#define BROWSER "firefox"

/* appearance */
static unsigned int borderpx  = 0;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static unsigned int gappih    = 20;       /* horiz inner gap between windows */
static unsigned int gappiv    = 10;       /* vert inner gap between windows */
static unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 0;        /* 0 means bottom bar */
static char *fonts[]          = { "Noto Sans CJK KR:style=Regular:size=10", "NotoColorEmoji:pixelsize=10:antialias=true:autohint=true"  };
static char normbgcolor[]           = "#000000";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#770000";
static char selbgcolor[]            = "#000000";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {TERMINAL, "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {TERMINAL, "-n", "spcalc", "-f", "monospace:size=16", "-g", "50x20", "-e", "bc", "-lq", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spcalc",      spcmd2},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const unsigned int ulinepad	= 5;	/* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	= 2;	/* thickness / height of the underline */
static const unsigned int ulinevoffset	= 0;	/* how far above the bottom of the bar the line should appear */
static const int ulineall 		= 0;	/* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	*/
	/* class    instance      title       	 tags mask    isfloating   isterminal  noswallow  monitor */
	{ "Gimp",     NULL,       NULL,       	    1 << 8,       0,           0,         0,        -1 },
	{ TERMCLASS,  NULL,       NULL,       	    0,            0,           1,         0,        -1 },
	{ NULL,       NULL,       "Event Tester",   0,            0,           0,         1,        -1 },
	{ TERMCLASS,      "floatterm", NULL,       	    0,       1,           1,         0,        -1 },
	{ TERMCLASS,      "bg",        NULL,       	    1 << 7,       0,           1,         0,        -1 },
	{ TERMCLASS,      "spterm",    NULL,       	    SPTAG(0),     1,           1,         0,        -1 },
	{ TERMCLASS,      "spcalc",    NULL,       	    SPTAG(1),     1,           1,         0,        -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",	tile },			/* Default: Master on left, slaves on right */
	{ "TTT",	bstack },		/* Master on top, slaves on bottom */

	{ "[@]",	spiral },		/* Fibonacci spiral */
	{ "[\\]",	dwindle },		/* Decreasing in size right and leftward */

	{ "[D]",	deck },			/* Master on left, slaves in monocle-like mode on right */
	{ "[M]",	monocle },		/* All windows on top of eachother */

	{ "|M|",	centeredmaster },		/* Master in middle, slaves on sides */
	{ ">M>",	centeredfloatingmaster },	/* Same but master floats */

	{ "><>",	NULL },			/* no layout function means floating behavior */
	{ NULL,		NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD,	XK_j,	ACTION##stack,	{.i = INC(+1) } }, \
	{ MOD,	XK_k,	ACTION##stack,	{.i = INC(-1) } }, \
	{ MOD,  XK_v,   ACTION##stack,  {.i = 0 } }, \
	/* { MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \ */
	/* { MOD, XK_a,     ACTION##stack, {.i = 1 } }, \ */
	/* { MOD, XK_z,     ACTION##stack, {.i = 2 } }, \ */
	/* { MOD, XK_x,     ACTION##stack, {.i = -1 } }, */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[]  = { TERMINAL, NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "color0",		STRING,	&normbordercolor },
		{ "color8",		STRING,	&selbordercolor },
		{ "color0",		STRING,	&normbgcolor },
		{ "color4",		STRING,	&normfgcolor },
		{ "color0",		STRING,	&selfgcolor },
		{ "color4",		STRING,	&selbgcolor },
		{ "borderpx",		INTEGER, &borderpx },
		{ "snap",		INTEGER, &snap },
		{ "showbar",		INTEGER, &showbar },
		{ "topbar",		INTEGER, &topbar },
		{ "nmaster",		INTEGER, &nmaster },
		{ "resizehints",	INTEGER, &resizehints },
		{ "mfact",		FLOAT,	&mfact },
		{ "gappih",		INTEGER, &gappih },
		{ "gappiv",		INTEGER, &gappiv },
		{ "gappoh",		INTEGER, &gappoh },
		{ "gappov",		INTEGER, &gappov },
		{ "swallowfloating",	INTEGER, &swallowfloating },
		{ "smartgaps",		INTEGER, &smartgaps },
};

#include <X11/XF86keysym.h>
#include "shiftview.c"

static const Key keys[] = {
	/* modifier                     key        function        argument */
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	/* { MODKEY|ShiftMask,		XK_Escape,	spawn,	SHCMD("") }, */
	{ MODKEY,			XK_grave,	spawn,	SHCMD("c=$(cut -d ';' -f1 ~/.local/share/chars/* | dmenu -i -l 30); [ -n \"$c\" ] && printf %s ${c%% *} | xclip -sel c && xdotool key Shift+Insert") },
	/* { MODKEY|ShiftMask,		XK_grave,	togglescratch,	SHCMD("") }, */
	TAGKEYS(			XK_1,		0)
	TAGKEYS(			XK_2,		1)
	TAGKEYS(			XK_3,		2)
	TAGKEYS(			XK_4,		3)
	TAGKEYS(			XK_5,		4)
	TAGKEYS(			XK_6,		5)
	TAGKEYS(			XK_7,		6)
	TAGKEYS(			XK_8,		7)
	TAGKEYS(			XK_9,		8)
	{ MODKEY,			XK_0,		view,		{.ui = ~0 } },
	{ MODKEY|ShiftMask,		XK_0,		tag,		{.ui = ~0 } },
	{ MODKEY,			XK_minus,	spawn,		SHCMD("wpctl set-volume -l 1 @DEFAULT_AUDIO_SINK@ 3%-; v=$(wpctl get-volume @DEFAULT_AUDIO_SINK@); [ \"${v#*MUTED}\" != \"$v\" ] && dunstify -a Volume -u low -i $PIX/volume-mute.svg Volume Muted -r 9993 -t 2000 || { v=${v#Volume: }; v=$(IFS='.'; set -- $v; printf %s \"$@\"); v=$(printf %.0f $v); dunstify -a Volume -u low -r 9993 -h int:value:${v} -i $PIX/volume-on.svg Volume ${v}% -t 2000; }") },
	{ MODKEY|ShiftMask,		XK_minus,	spawn,		SHCMD("wpctl set-volume -l 1 @DEFAULT_AUDIO_SINK@ 15%-; v=$(wpctl get-volume @DEFAULT_AUDIO_SINK@); [ \"${v#*MUTED}\" != \"$v\" ] && dunstify -a Volume -u low -i $PIX/volume-mute.svg Volume Muted -r 9993 -t 2000 || { v=${v#Volume: }; v=$(IFS='.'; set -- $v; printf %s \"$@\"); v=$(printf %.0f $v); dunstify -a Volume -u low -r 9993 -h int:value:${v} -i $PIX/volume-on.svg Volume ${v}% -t 2000; }") },
	{ MODKEY,			XK_equal,	spawn,		SHCMD("wpctl set-volume -l 1 @DEFAULT_AUDIO_SINK@ 3%+; v=$(wpctl get-volume @DEFAULT_AUDIO_SINK@); [ \"${v#*MUTED}\" != \"$v\" ] && dunstify -a Volume -u low -i $PIX/volume-mute.svg Volume Muted -r 9993 -t 2000 || { v=${v#Volume: }; v=$(IFS='.'; set -- $v; printf %s \"$@\"); v=$(printf %.0f $v); dunstify -a Volume -u low -r 9993 -h int:value:${v} -i $PIX/volume-on.svg Volume ${v}% -t 2000; }") },
	{ MODKEY|ShiftMask,		XK_equal,	spawn,		SHCMD("wpctl set-volume -l 1 @DEFAULT_AUDIO_SINK@ 15%+; v=$(wpctl get-volume @DEFAULT_AUDIO_SINK@); [ \"${v#*MUTED}\" != \"$v\" ] && dunstify -a Volume -u low -i $PIX/volume-mute.svg Volume Muted -r 9993 -t 2000 || { v=${v#Volume: }; v=$(IFS='.'; set -- $v; printf %s \"$@\"); v=$(printf %.0f $v); dunstify -a Volume -u low -r 9993 -h int:value:${v} -i $PIX/volume-on.svg Volume ${v}% -t 2000; }") },

	{ MODKEY,			XK_Tab,		spawn,		SHCMD("mpv --terminal=no ~/Videos/Watchlist") },
	{ MODKEY,			XK_c,		spawn,		SHCMD("cd $NOTES_DIR || exit; n=`printf '%s\n' * | dmenu -l 15 -i -p 'Notes'`; [ -n \"$n\" ] && $TERMINAL -e $EDITOR \"$n\"") },
	{ MODKEY,			XK_z,		spawn,		SHCMD("dl -v") },
	{ MODKEY,			XK_x,		spawn,		SHCMD("st -n floatterm -g 60x1 -e sh -c 'read -r input; [ -n \"$input\" ] && echo \"$input\" >> $NOTES_FILE'") },
	{ MODKEY,			XK_BackSpace,	spawn,		SHCMD("dl -d") },
	{ MODKEY,			XK_s,		spawn,		SHCMD("searchfiles") },
	{ MODKEY,			XK_o,		spawn,		SHCMD("watchlater") },
	{ MODKEY,			XK_u,		spawn,		SHCMD("if setxkbmap -query | grep -q ro; then setxkbmap us; else setxkbmap ro std; fi") },
	{ MODKEY,			XK_e,		spawn,		SHCMD("! pidof transmission-daemon >/dev/null && transmission-daemon && notify-send 'Starting torrent daemon...'; $TERMINAL -e stig") },
	/* { MODKEY|ShiftMask,		XK_Tab,		spawn,		SHCMD("") }, */
	{ MODKEY,			XK_q,		killclient,	{0} },
	{ MODKEY|ShiftMask,		XK_q,		spawn,		{.v = (const char*[]){ "sysact", NULL } } },
	{ MODKEY,			XK_w,		spawn,		{.v = (const char*[]){ BROWSER, NULL } } },
	{ MODKEY|ShiftMask,		XK_w,		spawn,		{.v = (const char*[]){ TERMINAL, "-e", "sudo", "nmtui", NULL } } },
	{ MODKEY,			XK_r,		spawn,		{.v = (const char*[]){ TERMINAL, "-e", "lfub", NULL } } },
	{ MODKEY|ShiftMask,		XK_r,		spawn,		{.v = (const char*[]){ TERMINAL, "-e", "htop", NULL } } },
	{ MODKEY,			XK_y,		setlayout,	{.v = &layouts[0]} }, /* tile */
	{ MODKEY|ShiftMask,		XK_t,		setlayout,	{.v = &layouts[1]} }, /* bstack */
	{ MODKEY|ShiftMask,		XK_y,		setlayout,	{.v = &layouts[2]} }, /* spiral */
	/* { MODKEY|ShiftMask,		XK_t,		setlayout,	{.v = &layouts[3]} }, */
	{ MODKEY|ShiftMask,		XK_u,		setlayout,	{.v = &layouts[4]} }, /* deck */
	/* { MODKEY,			XK_u,		setlayout,	{.v = &layouts[5]} }, */
	{ MODKEY,			XK_i,		setlayout,	{.v = &layouts[6]} }, /* centeredmaster */
	{ MODKEY|ShiftMask,		XK_i,		setlayout,	{.v = &layouts[7]} }, /* centeredfloatingmaster */
	/* { MODKEY,			XK_o,		incnmaster,     {.i = +1 } }, */
	{ MODKEY|ShiftMask,		XK_o,		incnmaster,     {.i = -1 } },
	{ MODKEY,			XK_p,			spawn,		{.v = (const char*[]){ "mpc", "toggle", NULL } } },
	{ MODKEY|ShiftMask,		XK_p,			spawn,		SHCMD("mpc pause") },
	{ MODKEY,			XK_bracketleft,		spawn,		{.v = (const char*[]){ "mpc", "seek", "-10", NULL } } },
	{ MODKEY|ShiftMask,		XK_bracketleft,		spawn,		{.v = (const char*[]){ "mpc", "seek", "-60", NULL } } },
	{ MODKEY,			XK_bracketright,	spawn,		{.v = (const char*[]){ "mpc", "seek", "+10", NULL } } },
	{ MODKEY|ShiftMask,		XK_bracketright,	spawn,		{.v = (const char*[]){ "mpc", "seek", "+60", NULL } } },
	{ MODKEY,			XK_backslash,		view,		{0} },
	/* { MODKEY|ShiftMask,		XK_backslash,		spawn,		SHCMD("") }, */

	{ MODKEY,			XK_a,		togglegaps,	{0} },
	{ MODKEY|ShiftMask,		XK_a,		defaultgaps,	{0} },
	/* { MODKEY,			XK_s,		togglesticky,	{0} }, */
	{ MODKEY,			XK_d,		spawn,          {.v = (const char*[]){ "dmenu_run", NULL } } },
	{ MODKEY|ShiftMask,		XK_d,		spawn,		{.v = (const char*[]){ "passmenu", NULL } } },
	{ MODKEY,			XK_f,		togglefullscr,	{0} },
	{ MODKEY|ShiftMask,		XK_f,		setlayout,	{.v = &layouts[8]} },
	{ MODKEY,			XK_g,		shiftview,	{ .i = -1 } },
	{ MODKEY|ShiftMask,		XK_g,		shifttag,	{ .i = -1 } },
	{ MODKEY,			XK_h,		setmfact,	{.f = -0.05} },
	/* J and K are automatically bound above in STACKEYS */
	{ MODKEY,			XK_l,		setmfact,      	{.f = +0.05} },
	{ MODKEY,			XK_semicolon,	shiftview,	{ .i = 1 } },
	{ MODKEY|ShiftMask,		XK_semicolon,	shifttag,	{ .i = 1 } },
	{ MODKEY,			XK_apostrophe,	togglescratch,	{.ui = 1} },
	/* { MODKEY|ShiftMask,		XK_apostrophe,	spawn,		SHCMD("") }, */
	{ MODKEY|ShiftMask,		XK_apostrophe,	togglesmartgaps,	{0} },
	{ MODKEY,			XK_Return,	spawn,		{.v = termcmd } },
	{ MODKEY|ShiftMask,		XK_Return,	togglescratch,	{.ui = 0} },

	{ MODKEY|ShiftMask,		XK_b,		incrgaps,	{.i = +3 } },
	{ MODKEY|ShiftMask,		XK_v,		incrgaps,	{.i = -3 } },
	/* V is automatically bound above in STACKKEYS */
	{ MODKEY,			XK_b,		togglebar,	{0} },
	/* { MODKEY|ShiftMask,		XK_b,		spawn,		SHCMD("") }, */
	{ MODKEY|ShiftMask,		XK_k,		spawn,		SHCMD("s=$(ps -a -u $USER | dmenu -l 15 -i -p 'Select process to kill' | awk '{print $1}'); [ -n \"$s\" ] && kill -9 $s") },
	{ MODKEY,			XK_t,		spawn,		SHCMD("t=$(tsp | dmenu -l 15 -i -p 'Task Spooler' | awk '{print $1}'); [ -n \"$t\" ] && tsp -k $t") },
	{ MODKEY,			XK_n,		spawn,		{.v = (const char*[]){ TERMINAL, "-e", "nvim", "-c", "VimwikiIndex", NULL } } },
	{ MODKEY|ShiftMask,		XK_n,		spawn,		SHCMD(TERMINAL " -e podboat") },
	{ MODKEY,			XK_slash,	spawn,		SHCMD(TERMINAL " -e newsboat") },
	{ MODKEY,			XK_m,		spawn,		{.v = (const char*[]){ TERMINAL, "-e", "ncmpcpp", NULL } } },
	{ MODKEY,			XK_comma,	spawn,		{.v = (const char*[]){ "mpc", "prev", NULL } } },
	{ MODKEY|ShiftMask,		XK_comma,	spawn,		{.v = (const char*[]){ "mpc", "seek", "0%", NULL } } },
	{ MODKEY,			XK_period,	spawn,		{.v = (const char*[]){ "mpc", "next", NULL } } },
	{ MODKEY|ShiftMask,		XK_period,	spawn,		{.v = (const char*[]){ "mpc", "repeat", NULL } } },

	{ MODKEY,			XK_Left,	focusmon,	{.i = -1 } },
	{ MODKEY|ShiftMask,		XK_Left,	tagmon,		{.i = -1 } },
	{ MODKEY,			XK_Right,	focusmon,	{.i = +1 } },
	{ MODKEY|ShiftMask,		XK_Right,	tagmon,		{.i = +1 } },

	{ MODKEY,			XK_Page_Up,	shiftview,	{ .i = -1 } },
	{ MODKEY|ShiftMask,		XK_Page_Up,	shifttag,	{ .i = -1 } },
	{ MODKEY,			XK_Page_Down,	shiftview,	{ .i = +1 } },
	{ MODKEY|ShiftMask,		XK_Page_Down,	shifttag,	{ .i = +1 } },
	{ MODKEY,			XK_Insert,	spawn,		SHCMD("xdotool type $(grep -v '^#' ~/.local/share/larbs/snippets | dmenu -i -l 50 | cut -d' ' -f1)") },

	{ MODKEY|ShiftMask,		XK_BackSpace,	spawn,		{.v = (const char*[]){ "arandr", NULL } } },
	{ MODKEY|ShiftMask,		XK_s,		spawn,		SHCMD(TERMINAL " -e pulsemixer") },
	{ MODKEY,			XK_F5,		xrdb,		{.v = NULL } },
	{ MODKEY|ShiftMask,		XK_e,		spawn,		SHCMD("pidof transmission-daemon >/dev/null && { [ \"$(printf 'No\\nYes' | dmenu -i -p 'Turn off transmission-daemon?')\" = \"Yes\" ] && killall transmission-daemon && notify-send -i $PIX/transmission.svg 'Transmission disabled'; } || { [ \"$(printf 'No\\nYes' | dmenu -i -p 'Turn on transmission-daemon?')\" = \"Yes\" ] && transmission-daemon && notify-send -i $PIX/transmission.svg 'Transmission enabled'; }") },
	{ MODKEY|ShiftMask,		XK_x,		spawn,		SHCMD("systemctl is-active emby-server.service >/dev/null && { [ \"$(printf 'No\\nYes' | dmenu -i -p 'Turn off Emby?')\" = \"Yes\" ] && sudo systemctl stop emby-server.service && notify-send -i $PIX/emby.svg 'Emby disabled'; } || { [ \"$(printf 'No\\nYes' | dmenu -i -p 'Turn on Emby?')\" = \"Yes\" ] && sudo systemctl start emby-server.service && notify-send -i $PIX/emby.svg 'Emby enabled'; }") },
	{ MODKEY,			XK_F8,		spawn,		{.v = (const char*[]){ "mailsync", NULL } } },
	{ MODKEY|ShiftMask,		XK_z,		spawn,		{.v = (const char*[]){ "mounter", NULL } } },
	{ MODKEY|ShiftMask,		XK_m,		spawn,		{.v = (const char*[]){ "mounter", "unmount", NULL } } },
	{ MODKEY|ShiftMask,		XK_c,		spawn,		SHCMD("mpv --untimed --no-cache --no-osc --no-input-default-bindings --profile=low-latency --input-conf=/dev/null --title=webcam $(ls /dev/video[0,2,4,6,8] | tail -n 1)") },
	{ MODKEY,			XK_F12,		spawn,		SHCMD("remaps") },
	{ MODKEY,			XK_space,	zoom,		{0} },
	{ MODKEY|ShiftMask,		XK_space,	togglefloating,	{0} },

	{ 0,				XK_Print,	spawn,		{.v = (const char*[]){ "maimpick", NULL } } },
	{ MODKEY,			XK_Print,	spawn,		{.v = (const char*[]){ "dmenurecord", NULL } } },
	{ MODKEY|ShiftMask,		XK_Print,	spawn,		{.v = (const char*[]){ "dmenurecord", "kill", NULL } } },
	{ MODKEY,			XK_Delete,	spawn,		{.v = (const char*[]){ "dmenurecord", "kill", NULL } } },
	{ MODKEY,			XK_Scroll_Lock,	spawn,		SHCMD("killall screenkey || screenkey &") },

	{ 0, XF86XK_AudioMute,		spawn,		SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle; v=$(wpctl get-volume @DEFAULT_AUDIO_SINK@); [ \"${v#*MUTED}\" != \"$v\" ] && dunstify -a Volume -u low -i $PIX/volume-mute.svg Volume Muted -r 9993 -t 2000 || { v=${v#Volume: }; v=$(IFS='.'; set -- $v; printf %s \"$@\"); v=$(printf %.0f $v); dunstify -a Volume -u low -r 9993 -h int:value:${v} -i $PIX/volume-on.svg Volume ${v}% -t 2000; }") },
	{ 0, XF86XK_AudioRaiseVolume,	spawn,		SHCMD("wpctl set-volume -l 1 @DEFAULT_AUDIO_SINK@ 3%+; v=$(wpctl get-volume @DEFAULT_AUDIO_SINK@); [ \"${v#*MUTED}\" != \"$v\" ] && dunstify -a Volume -u low -i $PIX/volume-mute.svg Volume Muted -r 9993 -t 2000 || { v=${v#Volume: }; v=$(IFS='.'; set -- $v; printf %s \"$@\"); v=$(printf %.0f $v); dunstify -a Volume -u low -r 9993 -h int:value:${v} -i $PIX/volume-on.svg Volume ${v}% -t 2000; }") },
	{ 0, XF86XK_AudioLowerVolume,	spawn,		SHCMD("wpctl set-volume -l 1 @DEFAULT_AUDIO_SINK@ 3%-; v=$(wpctl get-volume @DEFAULT_AUDIO_SINK@); [ \"${v#*MUTED}\" != \"$v\" ] && dunstify -a Volume -u low -i $PIX/volume-mute.svg Volume Muted -r 9993 -t 2000 || { v=${v#Volume: }; v=$(IFS='.'; set -- $v; printf %s \"$@\"); v=$(printf %.0f $v); dunstify -a Volume -u low -r 9993 -h int:value:${v} -i $PIX/volume-on.svg Volume ${v}% -t 2000; }") },
	{ 0, XF86XK_AudioPrev,		spawn,		{.v = (const char*[]){ "mpc", "prev", NULL } } },
	{ 0, XF86XK_AudioNext,		spawn,		{.v = (const char*[]){ "mpc",  "next", NULL } } },
	{ 0, XF86XK_AudioPause,		spawn,		{.v = (const char*[]){ "mpc", "pause", NULL } } },
	{ 0, XF86XK_AudioPlay,		spawn,		{.v = (const char*[]){ "mpc", "play", NULL } } },
	{ 0, XF86XK_AudioStop,		spawn,		{.v = (const char*[]){ "mpc", "stop", NULL } } },
	{ 0, XF86XK_AudioRewind,	spawn,		{.v = (const char*[]){ "mpc", "seek", "-10", NULL } } },
	{ 0, XF86XK_AudioForward,	spawn,		{.v = (const char*[]){ "mpc", "seek", "+10", NULL } } },
	{ 0, XF86XK_AudioMedia,		spawn,		{.v = (const char*[]){ TERMINAL, "-e", "ncmpcpp", NULL } } },
	{ 0, XF86XK_AudioMicMute,	spawn,		SHCMD("pactl set-source-mute @DEFAULT_SOURCE@ toggle") },
	/* { 0, XF86XK_PowerOff,		spawn,		{.v = (const char*[]){ "sysact", NULL } } }, */
	{ 0, XF86XK_Calculator,		spawn,		{.v = (const char*[]){ TERMINAL, "-e", "bc", "-l", NULL } } },
	{ 0, XF86XK_Sleep,		spawn,		{.v = (const char*[]){ "sudo", "-A", "zzz", NULL } } },
	{ 0, XF86XK_WWW,		spawn,		{.v = (const char*[]){ BROWSER, NULL } } },
	{ 0, XF86XK_DOS,		spawn,		{.v = termcmd } },
	{ 0, XF86XK_ScreenSaver,	spawn,		SHCMD("slock & xset dpms force off; mpc pause") },
	{ 0, XF86XK_TaskPane,		spawn,		{.v = (const char*[]){ TERMINAL, "-e", "htop", NULL } } },
	{ 0, XF86XK_Mail,		spawn,		SHCMD(TERMINAL " -e neomutt ; pkill -RTMIN+12 dwmblocks") },
	{ 0, XF86XK_MyComputer,		spawn,		{.v = (const char*[]){ TERMINAL, "-e",  "lfub",  "/", NULL } } },
	/* { 0, XF86XK_Battery,		spawn,		SHCMD("") }, */
	{ 0, XF86XK_Launch1,		spawn,		{.v = (const char*[]){ "xset", "dpms", "force", "off", NULL } } },
	{ 0, XF86XK_TouchpadToggle,	spawn,		SHCMD("(synclient | grep 'TouchpadOff.*1' && synclient TouchpadOff=0) || synclient TouchpadOff=1") },
	{ 0, XF86XK_TouchpadOff,	spawn,		{.v = (const char*[]){ "synclient", "TouchpadOff=1", NULL } } },
	{ 0, XF86XK_TouchpadOn,		spawn,		{.v = (const char*[]){ "synclient", "TouchpadOff=0", NULL } } },
	{ 0, XF86XK_MonBrightnessUp,	spawn,		SHCMD("brightnessctl -e4 set 5%+; b=$(brightnessctl -e4 -m); b=${b#*,*,*,}; dunstify -a Backlight -u low -r 9994 -h int:value:${b%%%,*} -i $PIX/brightness.svg Brightness ${b%%,*} -t 1000") },
	{ 0, XF86XK_MonBrightnessDown,	spawn,		SHCMD("brightnessctl -e4 set 5%-; b=$(brightnessctl -e4 -m); b=${b#*,*,*,}; dunstify -a Backlight -u low -r 9994 -h int:value:${b%%%,*} -i $PIX/brightness.svg Brightness ${b%%,*} -t 1000") },

	/* { MODKEY|Mod4Mask,              XK_h,      incrgaps,       {.i = +1 } }, */
	/* { MODKEY|Mod4Mask,              XK_l,      incrgaps,       {.i = -1 } }, */
	/* { MODKEY|Mod4Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } }, */
	/* { MODKEY|Mod4Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } }, */
	/* { MODKEY|Mod4Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } }, */
	/* { MODKEY|Mod4Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } }, */
	/* { MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} }, */
	/* { MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } }, */
	/* { MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } }, */
	/* { MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } }, */
	/* { MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } }, */
	/* { MODKEY|Mod4Mask,              XK_y,      incrohgaps,     {.i = +1 } }, */
	/* { MODKEY|Mod4Mask,              XK_o,      incrohgaps,     {.i = -1 } }, */
	/* { MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } }, */
	/* { MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } }, */

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
#ifndef __OpenBSD__
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigdwmblocks,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigdwmblocks,   {.i = 5} },
	{ ClkStatusText,        ShiftMask,      Button1,        sigdwmblocks,   {.i = 6} },
#endif
	{ ClkStatusText,        ShiftMask,      Button3,        spawn,          SHCMD(TERMINAL " -e nvim ~/.local/src/dwmblocks/config.h") },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        defaultgaps,	{0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkClientWin,		MODKEY,		Button4,	incrgaps,	{.i = +1} },
	{ ClkClientWin,		MODKEY,		Button5,	incrgaps,	{.i = -1} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTagBar,		0,		Button4,	shiftview,	{.i = -1} },
	{ ClkTagBar,		0,		Button5,	shiftview,	{.i = 1} },
	{ ClkRootWin,		0,		Button2,	togglebar,	{0} },
};

