#include <stdint.h>

#include "keypad.h"
#include "main.h"

typedef union {
    uint16_t raw;

    struct {
        uint16_t key_1: 1;
        uint16_t key_2: 1;
        uint16_t key_3: 1;
        uint16_t key_a: 1;

        uint16_t key_4: 1;
        uint16_t key_5: 1;
        uint16_t key_6: 1;
        uint16_t key_b: 1;

        uint16_t key_7: 1;
        uint16_t key_8: 1;
        uint16_t key_9: 1;
        uint16_t key_c: 1;

        uint16_t key_star: 1;
        uint16_t key_0: 1;
        uint16_t key_hash: 1;
        uint16_t key_d: 1;
    };
} Key_State;

_Static_assert(sizeof(Key_State) == 2, "Key_State must be exactly 16 bits");
_Static_assert(KEY_D - KEY_1 == 15, "Key enum must have 16 consecutive keys after KEY_NONE");

#define KEY_STATE_NONE ((Key_State){ .raw = 0 })

#define KEYPAD_DEBOUNCE_PASSES 3U
#define KEYPAD_SCAN_PERIOD_MS 2U

static GPIO_TypeDef *const row_ports[4] = {
    KEYPAD_ROW_1_GPIO_Port,
    KEYPAD_ROW_2_GPIO_Port,
    KEYPAD_ROW_3_GPIO_Port,
    KEYPAD_ROW_4_GPIO_Port
};

static const uint16_t row_pins[4] = {
    KEYPAD_ROW_1_Pin,
    KEYPAD_ROW_2_Pin,
    KEYPAD_ROW_3_Pin,
    KEYPAD_ROW_4_Pin
};

static GPIO_TypeDef *const col_ports[4] = {
    KEYPAD_COL_1_GPIO_Port,
    KEYPAD_COL_2_GPIO_Port,
    KEYPAD_COL_3_GPIO_Port,
    KEYPAD_COL_4_GPIO_Port
};

static const uint16_t col_pins[4] = {
    KEYPAD_COL_1_Pin,
    KEYPAD_COL_2_Pin,
    KEYPAD_COL_3_Pin,
    KEYPAD_COL_4_Pin
};
static Key_State keys_scan_acc = {.raw = 0};
static uint32_t keys_scan_active_row = 0;
static uint8_t keys_scan_locked = 0;
static Key_State keys_scan_state = {.raw = 0};
static uint32_t keys_scan_tick = 0;

static uint8_t keys_scan_candidate_count = 0;
static Key keys_scan_candidate_key = KEY_NONE;

static Key keys_scan_stable_key = KEY_NONE;

void Keypad_Init(void) {
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(row_ports[i], row_pins[i], GPIO_PIN_SET);
    }

    keys_scan_acc = KEY_STATE_NONE;
    keys_scan_active_row = 0;
    keys_scan_locked = 0;
    keys_scan_state = KEY_STATE_NONE;

    keys_scan_candidate_count = 0;
    keys_scan_candidate_key = KEY_NONE;

    keys_scan_stable_key = KEY_NONE;

    HAL_GPIO_WritePin(row_ports[keys_scan_active_row], row_pins[keys_scan_active_row], GPIO_PIN_RESET);
    keys_scan_tick = HAL_GetTick();
}

static Key Keypad_Decode(const Key_State state) {
    const uint16_t raw = state.raw;
    if (raw == 0 || (raw & (raw - 1)) != 0) {
        return KEY_NONE;
    }
    return (Key) (KEY_1 + __builtin_ctz(raw));
}

static int Keypad_ScanStep(void) {
    int is_full_pass_done = 0;

    for (uint32_t i = 0; i < 4; i++) {
        if (HAL_GPIO_ReadPin(col_ports[i], col_pins[i]) == GPIO_PIN_RESET) {
            keys_scan_acc.raw |= (uint16_t) (1U << (keys_scan_active_row * 4 + i));
        }
    }

    HAL_GPIO_WritePin(row_ports[keys_scan_active_row], row_pins[keys_scan_active_row], GPIO_PIN_SET);
    keys_scan_active_row = (keys_scan_active_row + 1) % 4;

    if (keys_scan_active_row == 0) {
        keys_scan_state = keys_scan_acc;
        keys_scan_acc = KEY_STATE_NONE;
        is_full_pass_done = 1;
    }

    HAL_GPIO_WritePin(row_ports[keys_scan_active_row], row_pins[keys_scan_active_row], GPIO_PIN_RESET);

    return is_full_pass_done;
}

Key Keypad_Poll(void) {
    const uint32_t now = HAL_GetTick();
    if (now - keys_scan_tick < KEYPAD_SCAN_PERIOD_MS) {
        return KEY_NONE;
    }
    keys_scan_tick = now;

    if (!Keypad_ScanStep()) {
        return KEY_NONE;
    }

    const uint16_t raw = keys_scan_state.raw;

    if (raw == 0) {
        keys_scan_locked = 0;
    } else if ((raw & (raw - 1)) != 0) {
        keys_scan_locked = 1;
    }

    const Key key = keys_scan_locked ? KEY_NONE : Keypad_Decode(keys_scan_state);

    if (key == keys_scan_candidate_key) {
        if (keys_scan_candidate_count < KEYPAD_DEBOUNCE_PASSES) {
            keys_scan_candidate_count++;
        }
    } else {
        keys_scan_candidate_key = key;
        keys_scan_candidate_count = 1;
    }

    if (keys_scan_candidate_count >= KEYPAD_DEBOUNCE_PASSES && keys_scan_candidate_key != keys_scan_stable_key) {
        keys_scan_stable_key = keys_scan_candidate_key;
        return keys_scan_candidate_key;
    }

    return KEY_NONE;
}
