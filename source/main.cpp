#include <3ds.h>
#include <citro2d.h>
#include <string.h>

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

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
    romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    nfcInit(NFC_OpType_NFCTag);

    // === Create screen target ===
    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    // === Load font ===
    C2D_Font font = C2D_FontLoad("romfs:/OpenSans.ttf");

    NFC_App_State appState = NFC_STATE_SCANNING;
    NFC_TagState nfcHardwareState;
    bool tagDetected = false;

    u8 UID[7] = {0};
    u8 lastUID[7] = {0};

    C2D_TextBuf g_staticBuf = C2D_TextBufNew(4096);
    C2D_Text g_staticText[3];

    // Start scanning
    nfcStartScanning(0);

    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_START)
            break;

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
                }
            }
        } else if (nfcHardwareState == NFC_TagState_OutOfRange) {
            if (tagDetected) {
                // Tag removed → reset
                memset(UID, 0, sizeof(UID));
                memset(lastUID, 0, sizeof(lastUID));
                tagDetected = false;
                appState = NFC_STATE_SCANNING;

                // Release scanner and restart to detect next tag
                nfcStopScanning();
                svcSleepThread(50000000); // 50ms pause
                nfcStartScanning(0);
            }
        }

        // === Draw on screen ===
        C2D_TextBufClear(g_staticBuf);

        C2D_TextFontParse(&g_staticText[0], font, g_staticBuf, "Better-NFCReader");
        C2D_TextOptimize(&g_staticText[0]);

        char uid_str[64];
        sprintf(uid_str, "UID: %02X:%02X:%02X:%02X:%02X:%02X:%02X",
                UID[0], UID[1], UID[2], UID[3], UID[4], UID[5], UID[6]);
        C2D_TextFontParse(&g_staticText[1], font, g_staticBuf, uid_str);
        C2D_TextOptimize(&g_staticText[1]);

        char scan_str[32];
        sprintf(scan_str, "Status: %s", appState == NFC_STATE_SCANNING ? "Looking for tag to read" : "Tag found!");
        C2D_TextFontParse(&g_staticText[2], font, g_staticBuf, scan_str);
        C2D_TextOptimize(&g_staticText[2]);

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(top);

        C2D_DrawText(&g_staticText[0], C2D_WithColor, 10, 0, 0.5f, 0.5f, 0.5f, C2D_Color32(255,255,255,255));
        C2D_DrawText(&g_staticText[1], C2D_WithColor, 10, 16, 0.5f, 0.5f, 0.5f, C2D_Color32(255,255,255,255));
        C2D_DrawText(&g_staticText[2], C2D_WithColor, 10, 32, 0.5f, 0.5f, 0.5f, C2D_Color32(255,255,255,255));

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
