#ifndef CODE_BREAKER_KEYPAD_H
#define CODE_BREAKER_KEYPAD_H

typedef enum {
    KEY_NONE = 0,
    KEY_1, KEY_2, KEY_3, KEY_A,
    KEY_4, KEY_5, KEY_6, KEY_B,
    KEY_7, KEY_8, KEY_9, KEY_C,
    KEY_STAR, KEY_0, KEY_HASH, KEY_D,
} Key;

void Keypad_Init(void);

Key Keypad_Poll(void);

#endif //CODE_BREAKER_KEYPAD_H
