/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static unsigned int borderpx  = 3;        /* border pixel of windows */
static unsigned int gappx     = 18;       /* gap pixel between windows */
static unsigned int snap      = 32;       /* snap pixel */
static int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=11", "Noto Color Emoji:size=11" };
static char normbgcolor[]           = "#222222";        /* layout symbol and status bar bg */
static char normbordercolor[]       = "#444444";        /* non-selected window border */
static char normfgcolor[]           = "#bbbbbb";        /* layout symbol and staus bar fg */
static char selfgcolor[]            = "#eeeeee";        /* tag and window title fg */
static char selbordercolor[]        = "#005577";        /* selected window border */
static char selbgcolor[]            = "#005577";        /* tag and window title bg */
static char urgentbgcolor[]         = "#ff0000";        /* urgent bg */
static char warnbgcolor[]           = "#ffff00";        /* warning bg */
static char urgentfgcolor[]         = "#ffffff";        /* urgent fg */
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel] = { selfgcolor, selbgcolor, selbordercolor  },
       [SchemeWarn] = { normfgcolor, warnbgcolor, urgentbgcolor },
       [SchemeUrgent] = { urgentfgcolor, urgentbgcolor, urgentbgcolor },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd0[] = {"st", "-n", "spterm", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd0},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance     title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "St",      NULL,        NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,        "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
	{ NULL,      "spterm",    NULL,     SPTAG(0),        1,          -1,          1,        -1 },
};

/* layout(s) */
static float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static int attachbottomflag = 1; /* 1 means attach new clients at the bottom of the stack */

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "urgentbgcolor",      STRING,  &urgentbgcolor },
		{ "warnbgcolor",        STRING,  &warnbgcolor },
		{ "urgentfgcolor",      STRING,  &urgentfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "gappx",          	INTEGER, &gappx },
		{ "snap",          		INTEGER, &snap },
		{ "swallowfloating",	INTEGER, &swallowfloating },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
		{ "attachbottom",   	INTEGER, &attachbottomflag },
};

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define RALT Mod5Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY|RALT,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|RALT,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask|RALT,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask|RALT, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *browsercmd[]  = { "firefox", NULL };
static const char *browserprivatecmd[]  = { "firefox", "-private-window", NULL };
static const char *lockcmd[]  = { "lock", "-f", NULL };
static const char *locktranslucentcmd[]  = { "lock", "-f", "-t", NULL };
static const char *screenshotcmd[]  = { "screenshot", NULL };
static const char *screenshotselectcmd[]  = { "screenshot", "-s", NULL };
static const char *quitcmd[]  = { "quit", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },             // Spawn dmenu
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },              // Spawn terminal
	{ MODKEY,                       XK_b,      spawn,          {.v = browsercmd } },           // Spawn browser
	{ MODKEY|ShiftMask,             XK_b,      spawn,          {.v = browserprivatecmd } },    // Spawn private browser
	{ MODKEY,                       XK_l,      spawn,          {.v = lockcmd } },              // Spawn locker
	{ MODKEY|ShiftMask,             XK_l,      spawn,          {.v = locktranslucentcmd } },   // Spawn locker translucent
	{ 0,                            XK_Print,  spawn,          {.v = screenshotcmd } },        // Capture screenshot
	{ ShiftMask,                    XK_Print,  spawn,          {.v = screenshotselectcmd } },  // Capture select screenshot
	{ 0, XF86XK_AudioMute,			spawn,     SHCMD("pamixer --toggle-mute; kill -45 \"$(pidof dwmblocks)\"") },
	{ 0, XF86XK_AudioLowerVolume,	spawn,     SHCMD("pamixer --decrease 2; kill -45 \"$(pidof dwmblocks)\"") },
	{ 0, XF86XK_AudioRaiseVolume,	spawn,     SHCMD("pamixer --increase 2; kill -45 \"$(pidof dwmblocks)\"") },
	{ 0, XF86XK_MonBrightnessDown,	spawn,     SHCMD("xbacklight -dec 5 && kill -44 \"$(pidof dwmblocks)\"") },
	{ 0, XF86XK_MonBrightnessUp,	spawn,     SHCMD("xbacklight -inc 5 && kill -44 \"$(pidof dwmblocks)\"") },
	{ 0, XF86XK_Search,         	spawn,     SHCMD("eyedropper") },
	{ MODKEY,                       XK_e,      focusstack,     {.i = -1 } },                   // Focus up stack
	{ MODKEY,                       XK_i,      focusstack,     {.i = +1 } },                   // Focus down stack
	{ MODKEY,                       XK_o,      toggleattachbottom, {0} },                      // Toggle attachbottom
	{ MODKEY,                       XK_n,      setcfact,       {.f =  0.00} },                 // Reset window height
	{ MODKEY|ShiftMask,             XK_e,      movestack,      {.i = -1 } },                   // Move window up stack
	{ MODKEY|ShiftMask,             XK_i,      movestack,      {.i = +1 } },                   // Move window down stack
	{ MODKEY|ShiftMask,             XK_n,      incnmaster,     {.i = +1 } },                   // Add a master
	{ MODKEY|ShiftMask,             XK_o,      incnmaster,     {.i = -1 } },                   // Remove a master
	{ MODKEY,                       XK_t,      setmfact,       {.f = +0.05} },                 // Inc master width
	{ MODKEY,                       XK_a,      setmfact,       {.f = -0.05} },                 // Dec master width
	{ MODKEY,                       XK_r,      setcfact,       {.f = +0.25} },                 // Inc window height
	{ MODKEY,                       XK_s,      setcfact,       {.f = -0.25} },                 // Dec window height
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },                          // Bring window to top
	{ MODKEY,                       XK_Tab,    view,           {0} },                          // Toggle previous tag
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },                          // Close window
	{ MODKEY,                       XK_w,      setlayout,      {.v = &layouts[0]} },           // Set tiling layout
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },           // Set floating layout
	{ MODKEY,                       XK_p,      setlayout,      {.v = &layouts[2]} },           // Set monocle layout
	{ MODKEY,            			XK_space,  togglescratch,  {.ui = 0 } },                   // Toggle scratchpad 0
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },                          // Toggle window floating
	{ MODKEY|Mod5Mask,              XK_o,      view,           {.ui = ~0 } },                  // View all tags
	{ MODKEY|Mod5Mask|ShiftMask,    XK_o,      tag,            {.ui = ~0 } },                  // Add window to all tags
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },                   // Focus prev monitor
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },                   // Focus next monitor
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },                   // Tag prev monitor
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },                   // Tag next monitor
	{ MODKEY,                       XK_Down,   moveresize,     {.v = "0x 25y 0w 0h" } },       // Move floating window down
	{ MODKEY,                       XK_Up,     moveresize,     {.v = "0x -25y 0w 0h" } },      // Move floating window up
	{ MODKEY,                       XK_Right,  moveresize,     {.v = "25x 0y 0w 0h" } },       // Move floating window right
	{ MODKEY,                       XK_Left,   moveresize,     {.v = "-25x 0y 0w 0h" } },      // Move floating window left
	{ MODKEY|ShiftMask,             XK_Down,   moveresize,     {.v = "0x 0y 0w 25h" } },       // Inc floating window height
	{ MODKEY|ShiftMask,             XK_Up,     moveresize,     {.v = "0x 0y 0w -25h" } },      // Dec floating window height
	{ MODKEY|ShiftMask,             XK_Right,  moveresize,     {.v = "0x 0y 25w 0h" } },       // Inc floating window width
	{ MODKEY|ShiftMask,             XK_Left,   moveresize,     {.v = "0x 0y -25w 0h" } },      // Dec floating window width
	{ MODKEY|ControlMask,           XK_Up,     moveresizeedge, {.v = "t"} },                   // Snap floating window top
	{ MODKEY|ControlMask,           XK_Down,   moveresizeedge, {.v = "b"} },                   // Snap floating window bottom
	{ MODKEY|ControlMask,           XK_Left,   moveresizeedge, {.v = "l"} },                   // Snap floating window left
	{ MODKEY|ControlMask,           XK_Right,  moveresizeedge, {.v = "r"} },                   // Snap floating window right
	{ MODKEY|ControlMask|ShiftMask, XK_Up,     moveresizeedge, {.v = "T"} },                   // Resize floating window top
	{ MODKEY|ControlMask|ShiftMask, XK_Down,   moveresizeedge, {.v = "B"} },                   // Resize floating window bottom
	{ MODKEY|ControlMask|ShiftMask, XK_Left,   moveresizeedge, {.v = "L"} },                   // Resize floating window left
	{ MODKEY|ControlMask|ShiftMask, XK_Right,  moveresizeedge, {.v = "R"} },                   // Resize floating window right
	TAGKEYS(                        XK_a,                      0)                              // Tag 1
	TAGKEYS(                        XK_r,                      1)                              // Tag 2
	TAGKEYS(                        XK_s,                      2)                              // Tag 3
	TAGKEYS(                        XK_t,                      3)                              // Tag 4
	TAGKEYS(                        XK_g,                      4)                              // Tag 5
	TAGKEYS(                        XK_k,                      5)                              // Tag 6
	TAGKEYS(                        XK_n,                      6)                              // Tag 7
	TAGKEYS(                        XK_e,                      7)                              // Tag 8
	TAGKEYS(                        XK_i,                      8)                              // Tag 9
	{ MODKEY|ShiftMask,             XK_q,      spawn,          {.v = quitcmd} },               // Quit dwm
	//{ MODKEY,                       XK_b,      togglebar,      {0} },                          // Toggle bar
	//{ MODKEY,                       XK_space,  setlayout,      {0} },                          // Toggle previous layout
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },                   // Toggle recent layout
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },    // Set monicle
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },                   // Bring window to top
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },       // Spawn terminal
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },                   // Drag floating window
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },                   // Toggle window float
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },                   // Resize window
	{ ClkTagBar,            0,              Button1,        view,           {0} },                   // Select tag
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },                   // View tag
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },                   // Move window to tag
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },                   // Add window to tag
};
