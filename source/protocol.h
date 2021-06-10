/*
 * (c) 2021 by Tomasz bla Fortuna
 * License: GPLv2
 *
 * This file is shared with QMK firmware.
 */

#ifndef PROTOCOL_INCLUDED
#define PROTOCOL_INCLUDED
#include <inttypes.h>

#define PROTOCOL_SD SD1

enum {
  /*
   * Main -> LED
   */
  /* Basic config */
  CMD_LED_ON = 0x01,
  CMD_LED_OFF = 0x02,

  CMD_LED_SET_PROFILE = 0x03,
  CMD_LED_NEXT_PROFILE = 0x04,
  CMD_LED_PREV_PROFILE = 0x05,

  CMD_LED_NEXT_INTENSITY = 0x06,
  CMD_LED_NEXT_ANIMATION_SPEED = 0x07,

  /* Masks */
  /* Override a key color, eg. capslock */
  CMD_LED_MASK_SET_KEY = 0x10,
  /* Override all keys individually (huge payload) */
  CMD_LED_MASK_SET_ALL = 0x11,

  /* Override all keys with single color (eg. foreground color) */
  CMD_LED_MASK_SET_SINGLE = 0x12,
  /* Clear color override completely */
  CMD_LED_MASK_CLEAR = 0x13,

  /* Reactive / status */
  CMD_LED_GET_STATUS = 0x20,
  CMD_LED_KEY_BT_BLINK = 0x21, /* TODO: Blue blink */
  CMD_LED_KEY_DOWN = 0x22,
  CMD_LED_KEY_UP = 0x23, /* TODO */
  CMD_LED_IAP = 0x24,

  /* LED -> Main */
  /* TODO: Payload with data to send over HID */
  CMD_LED_DEBUG = 0x40,

  /* Number of profiles, current profile, on/off state,
     reactive flag, brightness, errors */
  CMD_LED_STATUS = 0x41,
};

/* 4 ROWS * 14 COLS * 4B (RGBX) = 224 */
#define MAX_PAYLOAD_SIZE 230

enum protoState {
  /* 2-byte initial start-of-message sync */
  FSA_SYNC_1,
  FSA_SYNC_2,
  /* Waiting for command byte */
  FSA_CMD,
  /* Waiting for ID byte */
  FSA_ID,
  /* Waiting for payload size */
  FSA_PAYLOAD_SIZE,
  /* Reading payload until payloadPosition == payloadSize */
  FSA_PAYLOAD,
};

/* Buffer holding a single message */
typedef struct {
  uint8_t command;
  uint8_t msgId;
  uint8_t payload[MAX_PAYLOAD_SIZE];
  uint8_t payloadSize;
} message_t;

/* Internal protocol state */
typedef struct {
  /* Callback to call upon receiving a valid message */
  void (*callback)(message_t *);

  /* Number of read payload bytes */
  uint8_t payloadPosition;

  /* Current finite-state-automata state */
  enum protoState state;

  uint8_t previousId;
  uint8_t errors;

  /* Currently constructed message */
  message_t buffer;
} protocol_t;

/* NOTE: This didn't work when defined on stack */
extern protocol_t proto;

/* Init state */
extern void protoInit(protocol_t *proto, void (*callback)(message_t *));

/* Consume one byte and push state forward - might call the callback */
extern void protoConsume(protocol_t *proto, uint8_t byte);

/* Prolonged silence - reset state */
extern void protoSilence(protocol_t *proto);

/* Transmit message */
extern void protoTx(uint8_t cmd, const unsigned char *buf, int payloadSize,
                    int retries);

#endif
