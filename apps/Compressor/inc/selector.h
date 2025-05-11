#ifndef COMPRESSOR_SELECTOR_H_
#define COMPRESSOR_SELECTOR_H_

#define SELECTOR_COLOR_BG 0xFFFF
#define SELECTOR_COLOR_FG 0x0000
#define SELECTOR_COLOR_FG_SELECT 0x7D19
#define SELECTOR_COLOR_HEAD_BG 0x0000
#define SELECTOR_COLOR_HEAD_FG 0xFFFF

const char * select_file(const char * extension_to_match, int max_files);

#endif
