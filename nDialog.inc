/**
  * <summary>
  *   Dialog system similar to easyDialog
  *
  *   <ul>
  *     <li>No OnDialogPerformed, never saw any use for it, just put the code directly into the dialog</li>
  *     <li>No format parameters in Dialog_Show, just format the string before calling the dialog</li>
  *   </ul>
  *
  *   Note: Modifies OnDialogResponse address in the public table to skip initialization code
  * </summary>
  *
  * Report generation broken for ndi_ShowPlayerDialog and __ndi_OnDialogResponse, Reason: Unknown
  */

#if defined _inc_nDialog_
    #endinput
#else
    #define _inc_nDialog_
#endif

#include <a_samp> // for ShowPlayerDialog

static
    bPublicBase, /// <summary>Contains the base of the public table</summar>
    bNativeBase, /// <summary>Contains the base of the native table</summar>
    sEmpty[] = "", /// <summary>Empty string</summar>
    pDialogResponse, /// <summary>Stores the pointer to the next Response function</summar>
    iDialogName[MAX_PLAYERS] = {-1, ...} /// <summary>Stores the funcidx of the functionname</summar>
;

native Dialog_Show(const playerid, _FUNCTION_, const style, const caption[], const info[], const button1[], const button2[]);
native Dialog_Close(const playerid);
native Dialog_Opened(const playerid);

#define Dialog:%0(%1) forward public ndi_%0(%1);public ndi_%0(%1)
#define Dialog_Show(%0,%1, ndi_ShowPlayerDialog(%0, "ndi_" #%1,
#define Dialog_Close ndi_HidePlayerDialog
#define Dialog_Opened ndi_IsDialogOpen

stock ndi_ShowPlayerDialog(const playerid, const function[], const style, const caption[], const info[], const button1[], const button2[]) {
    /// <summary>Shows a dialog box and calls the function at response</summary>
    /// <export/>
    /// <param name="playerid">The playerid</param>
    /// <param name="function">The response function name</param>
    /// <param name="style">Same as ShowPlayerDialog</param>
    /// <param name="caption">Same as ShowPlayerDialog</param>
    /// <param name="info">Same as ShowPlayerDialog</param>
    /// <param name="button1">Same as ShowPlayerDialog</param>
    /// <param name="button2">Same as ShowPlayerDialog</param>
    /// <returns>Same as ShowPlayerDialog</returns>

    #pragma unused style, caption, info, button1, button2

    const n8 = -8;

    iDialogName[playerid] = funcidx(function);
    // call ShowPlayerDialog
    #emit zero.s function // just to make sure that it is a valid id
    #emit stack 8
    #emit sysreq.c ShowPlayerDialog
    #emit stack n8
    #emit retn

    return false;
}

stock ndi_HidePlayerDialog(const playerid) {
    /// <summary>Hides the current dialog</summary>
    /// <export/>
    /// <param name="playerid">The playerid</param>
    /// <returns>Same as ShowPlayerDialog</returns>

    return ShowPlayerDialog(playerid, (iDialogName[playerid] = -1), DIALOG_STYLE_MSGBOX, sEmpty, sEmpty, sEmpty, sEmpty);
}

stock ndi_IsDialogOpen(const playerid) {
    /// <summary>Checks if a ndi dialog is open</summary>
    /// <export/>
    /// <param name="playerid">The playerid</param>
    /// <returns>True if a ndi dialog is open otherwise false</returns>

    return (iDialogName[playerid] != -1);
}

stock bool: ndi_IsDialog(const dialogid) {
    /// <summary>Checks if dialogid is a ndi dialogid</summary>
    /// <export/>
    /// <param name="dialogid">Given from OnDialogResponse</param>
    /// <returns>True if it is a ndi dialogid otherwise false</returns>

    return (bPublicBase <= dialogid < bNativeBase);
}

stock ndi_GetDialogName(const dialogid, buffer[32]) {
    /// <summary>Get the function name of the dialog</summary>
    /// <export/>
    /// <param name="dialogid">Given from OnDialogResponse</param>
    /// <param name="buffer">Buffer to store the name</param>
    /// <returns>The number of characters</returns>

    if(ndi_IsDialog(dialogid)) {
        new
            srcAddr,
            destAddr
        ; // get source address
        #emit load.s.pri dialogid
        #emit add.c 4
        #emit stor.s.pri dialogid
        #emit lref.s.alt dialogid
        #emit lctrl 1
        #emit sub.alt
        #emit stor.s.pri srcAddr
        // get dest address
        #emit load.s.pri buffer
        #emit stor.s.pri destAddr
        // swap bytes
        for(new i = 1; i < sizeof buffer / 4; ++i) {
            srcAddr += 4;

            #emit lref.s.pri srcAddr
            #emit push.pri
            #emit push.c 4
            #emit sysreq.c swapchars
            #emit stack 8
            #emit sref.s.pri destAddr

            destAddr += 4;
        } // unpack string
        return strunpack(buffer, buffer);
    }
    return 0;
}

static __ndi_OnDialogResponse(playerid, dialogid, response, listitem, & inputtext) {
    /// <summary>Extension of OnDialogResponse</summary>
    /// <param name="playerid">The playerid</param>
    /// <param name="dialogid">Unused, set to zero</param>
    /// <param name="response">Same as OnDialogResponse</param>
    /// <param name="listitem">Same as OnDialogResponse</param>
    /// <param name="inputtext">Same as OnDialogResponse</param>
    /// <returns>Same as OnDialogResponse</returns>
    /// <seealso name="OnDialogResponse"/>

    #emit push.s inputtext
    #emit const.alt iDialogName
    #emit load.s.pri playerid
    #emit idxaddr
    #emit stor.s.pri inputtext

    if(inputtext != -1) {
        const n1 = -1;
        // load function address
        #emit load.alt bPublicBase
        #emit lref.s.pri inputtext
        #emit shl.c.pri 3
        #emit add
        #emit stor.s.pri dialogid // dialogid contains function entry in public table
        #emit lref.s.pri dialogid
        // reset iDialogName
        #emit const.alt n1
        #emit sref.s.alt inputtext
        // restore inputtext
        #emit pop.alt // #emit pick doesn't exists in this version
        // push parameter
        #emit push.alt // so I have to pop and push it :/
        #emit push.s listitem
        #emit push.s response
        #emit push.s playerid
        #emit push.c 16
        #emit push pDialogResponse
    } else { // if not exist jump straight to the next DialogResponse if exists
        #emit pop.alt
        #emit load.pri pDialogResponse
    }
    #emit stor.s.alt inputtext
    #emit sctrl 6
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) { // gets only called once
    #if defined _ALS_OnDialogResponse
        #undef OnDialogResponse
    #else
        #define _ALS_OnDialogResponse
    #endif
    #define OnDialogResponse _ndi_OnDialogResponse_
    // get base address of publics table
    #emit lctrl 1 // dat
    #emit move.alt
    #emit const.pri 32 // publics
    #emit sub
    #emit stor.pri bPublicBase
    #emit add.c 4
    #emit stor.pri bNativeBase
    #emit lref.pri bPublicBase
    #emit sub
    #emit stor.pri bPublicBase
    #emit lref.pri bNativeBase
    #emit sub
    #emit stor.pri bNativeBase
    // replace function address of this public with the new one
    funcidx("OnDialogResponse");
    // calculate amx offset
    #emit shl.c.pri 3
    #emit load.alt bPublicBase
    #emit add
    #emit stor.pri pDialogResponse
    #emit lref.pri pDialogResponse
    #emit const.alt OnDialogResponse
    #emit sub
    // set new function address
    #emit const.alt __ndi_OnDialogResponse
    #emit add
    #emit sref.pri pDialogResponse
    // funcindex of next DialogResponse
    pDialogResponse = funcidx(#OnDialogResponse);
    // if exists calculate and load final function address
    if(pDialogResponse != -1) {
        #emit load.pri pDialogResponse
        #emit shl.c.pri 3
        #emit load.alt bPublicBase
        #emit add
        #emit stor.pri pDialogResponse
        #emit lref.pri pDialogResponse
        #emit add.c 4 // jumping over proc
        #emit stor.pri pDialogResponse
    } else { // otherwise skip to end of this function
        #emit lctrl 6
        #emit add.c 40
        #emit stor.pri pDialogResponse
    }
    // call new DialogResponse
    #emit const.pri __ndi_OnDialogResponse
    #emit add.c 4
    #emit sctrl 6

    return true;
}

forward public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);