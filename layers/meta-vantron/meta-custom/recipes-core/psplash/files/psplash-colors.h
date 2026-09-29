/* Replaces psplash's own psplash-colors.h (copied over it in do_configure).
 *
 * Black everywhere. The kernel draws the ThermaSol boot logo (a compiled-in
 * fbcon logo, see recipes-kernel/linux) centred on a black framebuffer at
 * ~1.5 s; psplash takes the framebuffer over at ~3 s and draws the same
 * artwork at the same centred position, so with a black background the
 * handoff does not change a single pixel. Upstream's light-grey
 * 0xec,0xec,0xe1 would flash the whole panel grey at that moment.
 */
#ifndef _HAVE_PSPLASH_COLORS_H
#define _HAVE_PSPLASH_COLORS_H

#define PSPLASH_BACKGROUND_COLOR 0x00,0x00,0x00
#define PSPLASH_TEXT_COLOR 0x60,0x75,0x79
#define PSPLASH_BAR_COLOR 0x60,0x75,0x79
#define PSPLASH_BAR_BACKGROUND_COLOR 0x00,0x00,0x00

#endif
