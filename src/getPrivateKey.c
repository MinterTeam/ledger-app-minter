//
// Created by edward on 07.11.2019.
//

#ifndef LEDGER_APP_GETPRIVATEKEY_H
#define LEDGER_APP_GETPRIVATEKEY_H

#include <stdint.h>
#include <stdbool.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include "blake2b.h"
#include "minter.h"
#include "ux.h"
#include "../libs/testlib/simple_cpp.h"

// Get a pointer to getPrivateKey's state variables.
static getPrivateKeyContext_t *ctx = &global.getPrivateKeyContext;

static const bagl_element_t ui_getPrivateKey_approve[] = {
    // The background; literally a black rectangle. This element must be
    // defined first, so that the other elements render on top of it. Also, if
    // your screen doesn't include a background, it will render directly on
    // top of the previous screen.
    UI_BACKGROUND(),

    // Rejection/approval icons, represented by a cross and a check mark,
    // respectively. The cross will be displayed on the far left of the
    // screen, and the check on the far right, so as to indicate which button
    // corresponds to each action.
    UI_ICON_LEFT(0x00, BAGL_GLYPH_ICON_CROSS),
    UI_ICON_RIGHT(0x00, BAGL_GLYPH_ICON_CHECK),

    // The two lines of text, which together form a complete sentence:
    //
    //    Sign this Hash
    //    with Key #123?
    //
    // The first line is always the same, but the second line must reflect
    // which signing key is used. Hence, the first UI_TEXT points to a
    // compile-time string literal, while the second points to a buffer whose
    // contents we can modify.
    //
    // There is an important restriction here, though: the elements of a
    // screen are declared const, so their fields cannot be modified at
    // runtime. In other words, we can change the *contents* of the text
    // buffer, but we cannot change the *pointer* to the buffer, and thus we
    // cannot resize it. (This is also why we cannot write ctx->indexStr: ctx
    // is not const.) So it is important to ensure that the buffer will be
    // large enough to hold any string we want to display. In practice, the
    // Nano S screen is only wide enough for a small number of characters, so
    // you should never need a buffer larger than 40 bytes. Later on, we'll
    // demonstrate a technique for displaying larger strings.
    UI_TEXT(0x00, 0, 12, 128, "Show Private Key"),
//    UI_TEXT(0x00, 0, 26, 128, global.getPrivateKeyContext.indexStr),

};

static unsigned int ui_getPrivateKey_approve_button(unsigned int button_mask, unsigned int button_mask_counter) {
    uint16_t tx_len = 0;
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT: // REJECT
            // Send an error code to the computer. The application on the computer
            // should recognize this code and display a "user refused to sign"
            // message instead of a generic error.
            io_exchange_with_code(SW_USER_REJECTED, 0);
            // Return to the main screen.
            ui_idle();
            break;

        case BUTTON_EVT_RELEASED | BUTTON_RIGHT: // APPROVE
        {
            cx_ecfp_private_key_t privKey;
            deriveMinterKeypair(ctx->keyIndex, &privKey, NULL);
//            bin2hex(ctx->keyHex, privKey.d, 32);
            os_memmove(G_io_apdu_buffer + tx_len, privKey.d, privKey.d_len);
            tx_len += privKey.d_len;

            // Send the data in the APDU buffer, along with a special code that
            // indicates approval. 32 is the number of bytes in the response APDU,
            // sans response code.
            io_exchange_with_code(SW_OK, tx_len);
            // Return to the main screen.
            ui_idle();
        }
            break;
    }

    return 0;
}

void handleGetPrivateKey(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {
    // Read the key index from dataBuffer
    ctx->keyIndex = U4LE(dataBuffer, 0);

    UX_DISPLAY(ui_getPrivateKey_approve, NULL);

    *flags |= IO_ASYNCH_REPLY;
}

#endif //LEDGER_APP_GETPRIVATEKEY_H
