//
// blackosx 2014
//

#ifndef _EMBEDDEDGRAPHICSUI_H
#define _EMBEDDEDGRAPHICSUI_H

#include "libegint.h"

DEFINE_EMB_DATA(emb_scroll_up_button) {0x00};
DEFINE_EMB_SIZE(emb_scroll_up_button)

DEFINE_EMB_DATA(emb_scroll_bar_start) {0x00};
DEFINE_EMB_SIZE(emb_scroll_bar_start)

DEFINE_EMB_DATA(emb_scroll_scroll_start) {0x00};
DEFINE_EMB_SIZE(emb_scroll_scroll_start)

DEFINE_EMB_DATA(emb_scroll_scroll_fill) {0x00};
DEFINE_EMB_SIZE(emb_scroll_scroll_fill)

DEFINE_EMB_DATA(emb_scroll_scroll_end) {0x00};
DEFINE_EMB_SIZE(emb_scroll_scroll_end)

DEFINE_EMB_DATA(emb_scroll_bar_fill) {0x00};
DEFINE_EMB_SIZE(emb_scroll_bar_fill)

DEFINE_EMB_DATA(emb_scroll_bar_end) {0x00};
DEFINE_EMB_SIZE(emb_scroll_bar_end)

DEFINE_EMB_DATA(emb_scroll_down_button){0x00};
DEFINE_EMB_SIZE(emb_scroll_down_button)

DEFINE_EMB_DATA(emb_radio_button_selected) {0x00};
DEFINE_EMB_SIZE(emb_radio_button_selected)
//unsigned int radio_button_selected_fs8_png_len = 418;

DEFINE_EMB_DATA(emb_radio_button){0x00};
DEFINE_EMB_SIZE(emb_radio_button)

//unsigned int radio_button_fs8_png_len = 324;
DEFINE_EMB_DATA(emb_selection_indicator) {0x00};
DEFINE_EMB_SIZE(emb_selection_indicator)

// Checkbox
DEFINE_EMB_DATA(emb_checkbox) {0x00};
DEFINE_EMB_SIZE(emb_checkbox)
//unsigned int checkbox_png_len = 173;

// Checkbox checked
DEFINE_EMB_DATA(emb_checkbox_checked) {0x00};
DEFINE_EMB_SIZE(emb_checkbox_checked)
//unsigned int checkbox_checked_png_len = 357;

#endif
