switch-sys-tweak
================

[![Build](https://github.com/p-sam/switch-sys-tweak/workflows/Build/badge.svg?branch=master)](https://github.com/p-sam/switch-sys-tweak/actions?query=workflow%3ABuild)

A collection of miscellaneous mitms that may eventually grow as stuff gets added.

## Features

* `FEAT_NSVM_SAFE` : Mitm's `ns:vm`->NeedsUpdateVulnerability to always return 0 (enable using web applets on outdated fws)
* `FEAT_NSAM_CONTROL` : [5.1.0+] Mitm's `ns:am2`->GetReadOnlyApplicationControlDataInterface to override icon/author/version/name by title.
* `FEAT_NSRO_CONTROL` : [11.0.0+] Same hook as above, but mitm target is `ns:ro`
* `FEAT_VCON` : [7.0.0+] Registers virtual controllers that stream inputs from a Nintendo 3DS with [3dsnxcontroller](https://github.com/p-sam/3dsnxcontroller)
* `FEAT_HOTKEY` : Allow to simulate presses of the CAPTURE button with ZL + L, and HOME with ZR + R with NSO SNES controllers

## Toggles

* `TOGL_LOGGING` : Enable logging to "sdmc:/sys-tweak.log"
* `TOGL_CUSTOM_HOTKEY` : If `FEAT_HOTKEY` is enabled, allows to trigger a custom hotkey action by pressing both sticks or ZL + ZR on NSO SNES controllers
	- This toggle expects a function to be defined in `src/hotkey_custom.inc` with the following prototype:
	```c
	void __CustomHotkeyAction(bool longPressed)
	```

## How to compile

Add the required flags to your make command line. `FEAT_ALL` and `TOGL_ALL` are available as wildcard flags.

Example: `make FEAT_ALL="Y" TOGL_LOGGING="Y"`

**Note:** You should probably `make clean` if you change the flags you're using.
