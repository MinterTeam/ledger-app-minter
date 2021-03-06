// Each command has some state associated with it that sticks around for the
// life of the command. A separate context_t struct should be defined for each
// command.

#ifndef LEDGER_MINTER_UX_H
#define LEDGER_MINTER_UX_H

#include <stdbool.h>
#include <os_io_seproxyhal.h>

typedef struct {
	uint32_t deriveIndex;
	uint8_t displayIndex;

	// NUL-terminated strings for display
	uint8_t headerStr[40]; // variable-length
	uint8_t deriveIndexStr[40]; // variable-length
	uint8_t addressStrFull[43]; // variable length
	// partialStr contains 12 characters of a longer string. This allows text
	// to be scrolled.
	uint8_t addressStrPart[13];
} getAddressContext_t;

typedef struct {
	uint32_t deriveIndex;
	uint8_t hash[32];
	uint8_t hashStrFull[64];
	uint8_t displayIndex;
	// NUL-terminated strings for display
	uint8_t deriveIndexStr[40]; // variable-length
	uint8_t hashStrPart[13];
} signHashContext_t;

// To save memory, we store all the context types in a single global union,
// taking advantage of the fact that only one command is executed at a time.
typedef union {
	getAddressContext_t getAddressContext;
	signHashContext_t signHashContext;
} commandContext;
extern commandContext global;

// ux is a magic global variable implicitly referenced by the UX_ macros. Apps
// should never need to reference it directly.
extern ux_state_t ux;

// These are helper macros for defining UI elements. There are four basic UI
// elements: the background, which is a black rectangle that fills the whole
// screen; icons on the left and right sides of the screen, typically used for
// navigation or approval; and text, which can be anywhere. The UI_TEXT macro
// uses Open Sans Regular 11px, which I've found to be adequate for all text
// elements; if other fonts are desired, I suggest defining a separate macro
// for each of them (e.g. UI_TEXT_BOLD).
//
// In the event that you want to define your own UI elements from scratch,
// you'll want to read include/bagl.h and include/os_io_seproxyhal.h in the
// nanos-secure-sdk repo to learn what each of the fields are used for.
#define UI_BACKGROUND() {{BAGL_RECTANGLE,0,0,0,128,32,0,0,BAGL_FILL,0,0xFFFFFF,0,0},NULL,0,0,0,NULL,NULL,NULL}
#define UI_ICON_LEFT(userid, glyph) {{BAGL_ICON,userid,3,12,7,7,0,0,0,0xFFFFFF,0,0,glyph},NULL,0,0,0,NULL,NULL,NULL}
#define UI_ICON_RIGHT(userid, glyph) {{BAGL_ICON,userid,117,13,8,6,0,0,0,0xFFFFFF,0,0,glyph},NULL,0,0,0,NULL,NULL,NULL}
#define UI_TEXT(userid, x, y, w, text) {{BAGL_LABELINE,userid,x,y,w,12,0,0,0,0xFFFFFF,0,BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER,0},(char *)text,0,0,0,NULL,NULL,NULL}

static void slideText(bool forward, uint8_t *index, uint8_t *fullStr, size_t fullStrSz, uint8_t *partStr) {
    if(!forward) {
        if ((*index) > 0) {
            (*index)--;
        } else {
            // no need to redraw UI
            return;
        }
    } else {
        if ((*index) < (fullStrSz - 12)) {
            (*index)++;
        } else {
            return;
        }
    }

    os_memmove(partStr, fullStr + (*index), 12);
    UX_REDISPLAY();
}

// ui_idle displays the main menu screen. Command handlers should call ui_idle
// when they finish.
void ui_idle(void);

// io_exchange_with_code is a helper function for sending APDUs, primarily
// from button handlers. It appends code to G_io_apdu_buffer and calls
// io_exchange with the IO_RETURN_AFTER_TX flag. tx is the current offset
// within G_io_apdu_buffer (before the code is appended).
void io_exchange_with_code(uint16_t code, uint16_t tx);

#endif //LEDGER_MINTER_UX_H