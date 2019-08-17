#define RUN_TESTS

#include <YSI_Core\y_testing>

#include "nDialog"

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    // printf("OnDialogResponse(%d, %d, %d, %d, %d)", playerid, dialogid, response, listitem, inputtext[0]);

    if(ndi_IsDialog(dialogid)) {
        new buffer[32];

        printf("Name: %s Length: %d", buffer, ndi_GetDialogName(dialogid, buffer));
    }
    return true;
}

Dialog:MyDialog1(playerid, response, listitem, inputtext[]) {
    inputtext[1]++;
}

Dialog:MyDialog2(playerid, response, listitem, inputtext[]) {
    inputtext[2]++;
}

Test:Test() {
    new
        dialog2,
        dialog1,
        addr
    ;
    #emit addr.pri addr
    #emit stor.s.pri addr

    Dialog_Show(0, MyDialog1, DIALOG_STYLE_MSGBOX, "Caption", "Info", "Ok", "");
    CallLocalFunction("OnDialogResponse", "iiiii", 0, 1, false, 2, addr);

    ASSERT(dialog1 == 1); // dialog1 called once

    CallLocalFunction("OnDialogResponse", "iiiii", 0, 1, false, 2, addr);

    ASSERT(dialog1 == 1); // dialog1 not recalled

    Dialog_Show(0, MyDialog2, DIALOG_STYLE_MSGBOX, "Caption", "Info", "Ok", "");
    CallLocalFunction("OnDialogResponse", "iiiii", 0, 3, false, 4, addr);

    ASSERT(dialog2 == 1); // dialog2 called once

    CallLocalFunction("OnDialogResponse", "iiiii", 0, 3, false, 4, addr);

    ASSERT(dialog2 == 1); // dialog2 not recalled

    if(Dialog_Opened(0)) {
        Dialog_Close(0); // combile test
    }
}