# Scan-line Miryoku

## Pitch
[Miryoku](https://github.com/manna-harbour/miryoku/) layout - lightly adapted.

* OS mode for cut/copy/paste
* Audio-visual feedback
* Consistent num/symbol layers

## Features

### OS mode

OS mode is selectable by keys in the Miryoku bluetooth positions.
Order is Windows/Mac/Linux.
OS mode sets:

* Cut/copy/paste
* Undo/redo
* Alt/gui swap on Mac

### Feedback

Feedback is audio and/or visual depending on the keyboard.
Supports:

* Layer change
* Layer lock
* OS mode
* Toggle keys
* Slider position

### Consistent layers

Miryoku has a symbol layer based on keys shifted from the num layer.
Well almost, there is a single difference.
Symbol left-paren is in the num dot position, but shift-num-dot becomes greater-than.
Set a shift override, and customize auto shift, to make consistent.

Setting a shift override changes the (os) auto-repeat behaviour of this shifted key from no-auto-repeat to auto-repeat.
To restore auto-repeat consistency on the left thumb, shift-override the remaining two positions.

### User key

The left-paren key is duplicated on the symbol layer, once on the left thumb (to be near right-paren) and once on the index finger (as shifted nine).
Steal the shifted nine position as a user key.

The user key is set to uk (currency) pound.

## Refinements

Tune behaviour of caps-word minus and underscore.
Treat both symbols as typed (no capitalization of minus to underscore - both are easy to type in plain Miryoku).
Both symbols continue caps word.

Increase tapping term for the weaker home-row fingers. Avoid gui misfires on Windows.

Slower double-taps for layer locks.

Key to mute keyboard audio. In the External Power Toggle position.

## Build

Run the GitHub "Build QMK Firmware" action. Download firmware from the action run.

## AVR disabled

The "avr" branch cuts down firmware size by
* [Squeezing AVR](https://docs.qmk.fm/squeezing_avr#squeezing-the-most-out-of-avr)
* Replacing some QMK features (double tap, key overrides) with smaller-size, cut-down implementations 

(Update) Avr builds are now disabled.

* Qmk is growing slowly in size. The cut-down "avr" branch  no longer passes the avr size check with latest QMK (tested on 2024/09).
* There is an avr size check in qmk compile, but this is ignored by the GitHub publish action.

## QMK Userspace

See [QMK Userspace](https://github.com/qmk/qmk_userspace) for information about adding keyboards and building firmware.
