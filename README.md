# nDialog

[![sampctl](https://shields.southcla.ws/badge/sampctl-nDialog-2f2f2f.svg?style=for-the-badge)](https://github.com/Shiska/nDialog)

Dialog system similar to easyDialog but

* No OnDialogPerformed, never saw any use for it, just put the code directly into the dialog
* No format parameters in Dialog_Show, just format the string before calling the dialog
* Uses ***ndi_*** instead of ***debug_*** as function prefix, you shouldn't call them manually anyways

Raison d'être: Just releasing old stuff which might by useful for someone

## Installation

Simply install to your project:

```bash
sampctl package install Shiska/nDialog
```

Include in your code and begin using the library:

```pawn
#include <nDialog>
```

## Functions

Similar to easyDialog

```pawn
native Dialog_Show(const playerid, _FUNCTION_, const style, const caption[], const info[], const button1[], const button2[]);
native Dialog_Close(const playerid);
native Dialog_Opened(const playerid);
```

Additional functions for OnDialogRepsonse  
Could be used to replicate OnDialogPerformed by using ndi_IsDialog

```pawn
// To check if the dialogid is part of the system
ndi_IsDialog(const dialogid);
// To get the dialog name (function name without prefix) of the dialogid
ndi_GetDialogName(const dialogid, buffer[32]);
```

## Example

To call the dialog
```pawn
Dialog_Show(playerid, MyDialog, DIALOG_STYLE_MSGBOX, "Caption", "Info", "Ok", "");
```
With the matching function handle called "MyDialog"
```pawn
Dialog:MyDialog(playerid, response, listitem, inputtext[]) {
    //code
}
```

## Testing

<!--
Depending on whether your package is tested via in-game "demo tests" or
y_testing unit-tests, you should indicate to readers what to expect below here.
-->

To test, simply run the package:

```bash
sampctl package run
```
