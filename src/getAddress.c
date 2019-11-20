// This file contains the implementation of the getPublicKey command. It is
// broadly similar to the signHash command, but with a few new features. Since
// much of the code is the same, expect far fewer comments.
//
// A high-level description of getPublicKey is as follows. The user initiates
// the command on their computer by requesting the generation of a specific
// public key. The command handler then displays a screen asking the user to
// confirm the action. If the user presses the 'approve' button, the requested
// key is generated, sent to the computer, and displayed on the device. The
// user may then visually compare the key shown on the device to the key
// received by the computer. Augmenting this, the user may optionally request
// that an address be generated from the public key, in which case this
// address is displayed instead of the public key. A final two-button press
// returns the user to the main screen.
//
// Note that the order of the getPublicKey screens is the reverse of signHash:
// first approval, then comparison.
//
// Keep this description in mind as you read through the implementation.

#include <stdint.h>
#include <stdbool.h>
#include <os.h>
#include <os_io_seproxyhal.h>
#include "minter.h"
#include "ux.h"

// Get a pointer to getPublicKey's state variables.
static getAddressContext_t *ctx = &global.getAddressContext;

// Define the comparison screen. This is where the user will compare the
// public key (or address) on their device to the one shown on the computer.
static const bagl_element_t ui_getAddress_compare[] = {
    UI_BACKGROUND(),
    UI_ICON_LEFT(0x01, BAGL_GLYPH_ICON_LEFT),
    UI_ICON_RIGHT(0x02, BAGL_GLYPH_ICON_RIGHT),
    UI_TEXT(0x00, 0, 12, 128, global.getAddressContext.headerStr),
    // The visible portion of the public key or address.
    UI_TEXT(0x00, 0, 26, 128, global.getAddressContext.addressStrPart),
};

// Define the preprocessor for the comparison screen. As in signHash, this
// preprocessor selectively hides the left/right arrows. The only difference
// is that, since public keys and addresses have different lengths, checking
// for the end of the string is slightly more complicated.
static const bagl_element_t *ui_prepro_getAddress_compare(const bagl_element_t *element) {
    int fullSize = 42;
    if ((element->component.userid == 1 && ctx->displayIndex == 0) ||
        (element->component.userid == 2 && ctx->displayIndex == fullSize - 12)) {
        return NULL;
    }
    return element;
}

// Define the button handler for the comparison screen. Again, this is nearly
// identical to the signHash comparison button handler.
static unsigned int ui_getAddress_compare_button(unsigned int button_mask, unsigned int button_mask_counter) {
    switch (button_mask) {
        case BUTTON_LEFT:
        case BUTTON_EVT_FAST | BUTTON_LEFT: // SEEK LEFT
            slideText(false, &ctx->displayIndex, ctx->addressStrFull, 42, ctx->addressStrPart);
            break;
        case BUTTON_RIGHT:
        case BUTTON_EVT_FAST | BUTTON_RIGHT: // SEEK RIGHT
            slideText(true, &ctx->displayIndex, ctx->addressStrFull, 42, ctx->addressStrPart);
            break;
        case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: // PROCEED
            // The user has finished comparing, so return to the main screen.
            ui_idle();
            break;
    }
    return 0;
}

// Define the approval screen. This is where the user will approve the
// generation of the public key (or address).
static const bagl_element_t ui_getAddress_approve[] = {
    UI_BACKGROUND(),
    UI_ICON_LEFT(0x00, BAGL_GLYPH_ICON_CROSS),
    UI_ICON_RIGHT(0x00, BAGL_GLYPH_ICON_CHECK),
    //
    //    Generate Address
    //     from Key #123?
    //
    // Since both lines differ based on user-supplied parameters, we can't use
    // compile-time string literals for either of them.
    UI_TEXT(0x00, 0, 12, 128, global.getAddressContext.headerStr),
    UI_TEXT(0x00, 0, 26, 128, global.getAddressContext.deriveIndexStr),
};

static void ioWriteAddress(uint16_t *tx) {
    cx_ecfp_public_key_t publicKey;
    // Derive the public key and address and store them in the APDU
    // buffer. Even though we know that tx starts at 0, it's best to
    // always add it explicitly; this prevents a bug if we reorder the
    // statements later.
    deriveMinterKeypair(ctx->deriveIndex, NULL, &publicKey);

    // write 20 bytes address
    pubkeyToMinterAddress(G_io_apdu_buffer + (*tx), publicKey.W);
    *tx += 20;

    // cleanup public key
    os_memset(&publicKey, 0, sizeof(publicKey));
}

// This is the button handler for the approval screen. If the user approves,
// it generates and sends the public key and address. (For simplicity, we
// always send both, regardless of which one the user requested.)
static unsigned int ui_getAddress_approve_button(unsigned int button_mask, unsigned int button_mask_counter) {
    // The response APDU will contain multiple objects, which means we need to
    // remember our offset within G_io_apdu_buffer. By convention, the offset
    // variable is named 'tx'.
    uint16_t tx = 0;
    cx_ecfp_public_key_t publicKey;
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT: // REJECT
            io_exchange_with_code(SW_USER_REJECTED, 0);
            ui_idle();
            break;

        case BUTTON_EVT_RELEASED | BUTTON_RIGHT: // APPROVE
            //TODO: investigate wth is going on if use ioWriteAddress here

            // Derive the public key and address and store them in the APDU
            // buffer. Even though we know that tx starts at 0, it's best to
            // always add it explicitly; this prevents a bug if we reorder the
            // statements later.
            deriveMinterKeypair(ctx->deriveIndex, NULL, &publicKey);

            // write 20 bytes address
            pubkeyToMinterAddress(G_io_apdu_buffer + tx, publicKey.W);
            tx += 20;

            // cleanup public key
            os_memset(&publicKey, 0, sizeof(publicKey));

            // Flush the APDU buffer, sending the response.
            io_exchange_with_code(SW_OK, tx);

            // UI BEGIN

            // Prepare the comparison screen, filling in the header and body text.
            os_memmove(ctx->headerStr, "Compare Address:", 16);
            ctx->headerStr[16] = '\0';
            // The APDU buffer already contains the hex-encoded address, so
            // copy it directly.

            // write minter address prefix
            os_memmove(ctx->addressStrFull, "Mx", 2);
            uint8_t addrStr[40];
            // convert bytes to hex string
            bin2hex(addrStr, G_io_apdu_buffer, 40);
            // insert address to context
            os_memmove(ctx->addressStrFull + 2, addrStr, 40);

            // finalize with terminator
            ctx->addressStrFull[42] = '\0';

            // write to partial buf 12 bytes of full address
            os_memmove(ctx->addressStrPart, ctx->addressStrFull, 12);
            ctx->addressStrPart[12] = '\0';
            // notify, this is first part of address
            ctx->displayIndex = 0;

            // Display the comparison screen.
            UX_DISPLAY(ui_getAddress_compare, ui_prepro_getAddress_compare);
            break;
    }
    return 0;
}

// These are APDU parameters that control the behavior of the getPublicKey
// command.
#define P1_GET_ADDRESS_SILENT 0x01

// handleGetAddress is the entry point for the getPublicKey command. It
// reads the command parameters, prepares and displays the approval screen,
// and sets the IO_ASYNC_REPLY flag.
void handleGetAddress(uint8_t p1,
                      uint8_t p2,
                      uint8_t *dataBuffer,
                      uint16_t dataLength,
                      volatile unsigned int *flags,
                      volatile unsigned int *tx) {

    // Read the key index from dataBuffer
    ctx->deriveIndex = U4LE(dataBuffer, 0);

    if(p1 == P1_GET_ADDRESS_SILENT) {
        uint16_t txl = 0;
        // Write to G_io_apdu_frame generated address and increment tx
        ioWriteAddress(&txl);
        // Flush the APDU buffer, sending the response.
        io_exchange_with_code(SW_OK, txl);
        return;
    }

    // Prepare the approval screen, filling in the header and body text.
    os_memmove(ctx->headerStr, "Generate Address", 16);
    ctx->headerStr[16] = '\0';

    os_memmove(ctx->deriveIndexStr, "from Key #", 10);
    int n = bin2dec(ctx->deriveIndexStr + 10, ctx->deriveIndex);
    os_memmove(ctx->deriveIndexStr + 10 + n, "?", 2);

    UX_DISPLAY(ui_getAddress_approve, NULL);
    *flags |= IO_ASYNCH_REPLY;
}

// Having previously read through signHash.c, getPublicKey.c shouldn't be too
// difficult to make sense of. We'll move on to the last (and most complex)
// command file in the walkthrough, calcTxnHash.c. Hold onto your hat!
