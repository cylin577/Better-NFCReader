#include <3ds.h>
#include <citro2d.h>
#include <string.h>
#include <stdio.h>
#include <string> // For std::string
#include "qrcodegen.hpp" // Include the QR code generation library

#define SCREEN_WIDTH_TOP  400
#define SCREEN_HEIGHT_TOP 240
#define SCREEN_WIDTH_BOTTOM 320
#define SCREEN_HEIGHT_BOTTOM 240

typedef enum {
    NFC_STATE_SCANNING,
    NFC_STATE_TAG_DETECTED,
} NFC_App_State;

// Compare two UID arrays
bool compareUID(const u8 *a, const u8 *b, int len) {
    for (int i = 0; i < len; i++)
        if (a[i] != b[i])
            return false;
    return true;
}

int main(int argc, char **argv)
{
    // === Initialize services ===
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Red screen at the beginning
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32(255, 0, 0, 255));
    C3D_FrameEnd(0);
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    romfsInit();

    // Green screen after romfsInit
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32(0, 255, 0, 255));
    C3D_FrameEnd(0);
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    Result nfcResult = nfcInit(NFC_OpType_NFCTag);

    // Blue screen after nfcInit
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32(0, 0, 255, 255));
    C3D_FrameEnd(0);
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    C2D_Font font = C2D_FontLoad("romfs:/JetBrainsMono-Regular.ttf");

    // Yellow screen after font load
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32(255, 255, 0, 255));
    C3D_FrameEnd(0);
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    NFC_App_State appState = NFC_STATE_SCANNING;
    NFC_TagState nfcHardwareState;
    bool tagDetected = false;

    u8 UID[7] = {0};
    u8 lastUID[7] = {0};

    C2D_TextBuf g_staticBuf = C2D_TextBufNew(4096);
    C2D_Text g_staticText[3];

    Result scanResult = nfcStartScanning(0);

    // White screen after nfcStartScanning
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, C2D_Color32(255, 255, 255, 255));
    C3D_FrameEnd(0);
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();

    // QR Code related variables
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText("", qrcodegen::QrCode::Ecc::LOW);
    bool qr_generated = false;

    // Hidden mode variable
    bool hiddenModeActive = false;

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START)
            break;

        // Toggle hidden mode with SELECT button
        if (kDown & KEY_SELECT) {
            hiddenModeActive = !hiddenModeActive;
        }

        nfcGetTagState(&nfcHardwareState);

        if (nfcHardwareState == NFC_TagState_InRange) {
            NFC_TagInfo currentTag;
            Result r = nfcGetTagInfo(&currentTag);
            if (R_SUCCEEDED(r)) {
                memcpy(UID, currentTag.id, sizeof(UID));

                // Detect new tag (different from last)
                if (!compareUID(UID, lastUID, sizeof(UID))) {
                    memcpy(lastUID, UID, sizeof(lastUID));
                    tagDetected = true;
                    appState = NFC_STATE_TAG_DETECTED;

                    // Generate QR code
                    char uid_str_qr[64];
                    sprintf(uid_str_qr, "UID:%02X%02X%02X%02X%02X%02X%02X",
                            (unsigned int)UID[0], (unsigned int)UID[1], (unsigned int)UID[2], (unsigned int)UID[3], (unsigned int)UID[4], (unsigned int)UID[5], (unsigned int)UID[6]);
                    qr = qrcodegen::QrCode::encodeText(uid_str_qr, qrcodegen::QrCode::Ecc::QUARTILE);
                    qr_generated = true;
                }
            }
        } else if (nfcHardwareState == NFC_TagState_OutOfRange) {
            if (tagDetected) {
                // Tag removed -> reset
                memset(UID, 0, sizeof(UID));
                memset(lastUID, 0, sizeof(lastUID));
                tagDetected = false;
                appState = NFC_STATE_SCANNING;
                qr_generated = false; // Clear QR code state

                // Release scanner and restart to detect next tag
                nfcStopScanning();
                svcSleepThread(50000000); // 50ms pause
                nfcStartScanning(0);
            }
        }

        // === Draw on screen ===
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        
        // Top screen
        C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(top);

        C2D_TextBufClear(g_staticBuf);

        if (hiddenModeActive) {
            C2D_TextFontParse(&g_staticText[0], font, g_staticBuf, "Better-NFCKisser");
        } else {
            C2D_TextFontParse(&g_staticText[0], font, g_staticBuf, "Better-NFCReader");
        }
        C2D_TextOptimize(&g_staticText[0]);

        char uid_str[64];
        sprintf(uid_str, "UID: %02X:%02X:%02X:%02X:%02X:%02X:%02X",
                UID[0], UID[1], UID[2], UID[3], UID[4], UID[5], UID[6]);
        C2D_TextFontParse(&g_staticText[1], font, g_staticBuf, uid_str);
        C2D_TextOptimize(&g_staticText[1]);

        char scan_str[64];
        if (hiddenModeActive) {
            sprintf(scan_str, "Status: %s", appState == NFC_STATE_SCANNING ? "Looking for tag to kiss" : "Tag kissed!");
        } else {
            sprintf(scan_str, "Status: %s", appState == NFC_STATE_SCANNING ? "Looking for tag to read" : "Tag found!");
        }
        C2D_TextFontParse(&g_staticText[2], font, g_staticBuf, scan_str);
        C2D_TextOptimize(&g_staticText[2]);

        C2D_DrawText(&g_staticText[0], C2D_WithColor, 10, 0, 0.5f, 0.5f, 0.5f, C2D_Color32(255,255,255,255));
        C2D_DrawText(&g_staticText[1], C2D_WithColor, 10, 16, 0.5f, 0.5f, 0.5f, C2D_Color32(255,255,255,255));
        C2D_DrawText(&g_staticText[2], C2D_WithColor, 10, 32, 0.5f, 0.5f, 0.5f, C2D_Color32(255,255,255,255));

        // Bottom screen (QR Code)
        C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255)); // Clear bottom screen
        C2D_SceneBegin(bottom);

        if (qr_generated) {
            int qr_size = qr.getSize();
            int module_size = 3; // Size of each QR code module in pixels
            int qr_draw_size = qr_size * module_size;

            // Center the QR code on the bottom screen
            int offset_x = (SCREEN_WIDTH_BOTTOM - qr_draw_size) / 2;
            int offset_y = (SCREEN_HEIGHT_BOTTOM - qr_draw_size) / 2;

            for (int y = 0; y < qr_size; y++) {
                for (int x = 0; x < qr_size; x++) {
                    if (qr.getModule(x, y)) {
                        C2D_DrawRectangle(
                            offset_x + x * module_size,
                            offset_y + y * module_size,
                            0, // Depth
                            module_size,
                            module_size,
                            C2D_Color32(255, 255, 255, 255), // color_start
                            C2D_Color32(255, 255, 255, 255), // color_end
                            C2D_Color32(255, 255, 255, 255), // color_border
                            C2D_Color32(255, 255, 255, 255)  // color_fill
                        );
                    }
                }
            }
        }

        C3D_FrameEnd(0);
    }

    // === Cleanup ===
    nfcStopScanning();
    nfcExit();
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();

    return 0;
}
