#ifndef PLATFORM_KEYCODE_H
#define PLATFORM_KEYCODE_H

// Platform-independent key codes.
// On EGE, these map directly to ege::key_msg_e values.
enum KeyCode {
    KEY_A = 'A',
    KEY_B = 'B',
    KEY_C = 'C',
    KEY_Q = 'Q',
    KEY_S = 'S',
    KEY_0 = '0',
    KEY_1 = '1',
    KEY_2 = '2',
    KEY_3 = '3',
    KEY_4 = '4',
    KEY_5 = '5',
    KEY_ESC     = 0x1B,
    KEY_DELETE  = 0x2E,
};

#endif
