#ifndef MIN_H
#define MIN_H

#include <stdbool.h>
#include <stdint.h>
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include"app_uart.h"

#ifndef MAX_PAYLOAD
#define MAX_PAYLOAD (255U)
#endif

struct crc32_context {
    uint32_t crc;
};

struct min_context {
    uint8_t rx_frame_payload_buf[MAX_PAYLOAD]; // Payload received so far
    uint32_t rx_frame_checksum;                // Checksum received over the wire
    struct crc32_context rx_checksum;          // Calculated checksum for receiving frame
    struct crc32_context tx_checksum;          // Calculated checksum for sending frame
    uint8_t rx_header_bytes_seen;              // Countdown of header bytes to reset state
    uint8_t rx_frame_state;                    // State of receiver
    uint8_t rx_frame_payload_bytes;            // Length of payload received so far
    uint8_t rx_frame_id_control;               // ID and control bit of frame being received
    uint8_t rx_frame_seq;                      // Sequence number of frame being received
    uint8_t rx_frame_length;                   // Length of frame
    uint8_t rx_control;                        // Control byte
    uint8_t tx_header_byte_countdown;          // Count out the header bytes
    uint8_t port;                              // Number of the port associated with the context
};

// Send a non-transport frame MIN frame
void min_send_frame(struct min_context *self, uint8_t min_id, uint8_t const *payload, uint8_t payload_len);

// Must be regularly called, with the received bytes since the last call.
// NB: if the transport protocol is being used then even if there are no bytes
// this call must still be made in order to drive the state machine for retransmits.
void min_poll(struct min_context *self, uint8_t const *buf, uint32_t buf_len);

// Reset the state machine and (optionally) tell the other side that we have done so
void min_transport_reset(struct min_context *self, bool inform_other_side);

// CALLBACK. Handle incoming MIN frame
void min_application_handler(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload, uint8_t port);

// CALLBACK. Must return current buffer space in the given port. Used to check that a frame can be
// queued.
uint16_t min_tx_space(uint8_t port);

// CALLBACK. Send a byte on the given line.
void min_tx_byte(uint8_t port, uint8_t byte);

// CALLBACK. Indcates when frame transmission is finished; useful for buffering bytes into a single serial call.
void min_tx_start(uint8_t port);
void min_tx_finished(uint8_t port);

// Initialize a MIN context ready for receiving bytes from a serial link
// (Can have multiple MIN contexts)
void min_init_context(struct min_context *self, uint8_t port);

#ifdef MIN_DEBUG_PRINTING
// Debug print
void min_debug_print(const char *msg, ...);
#else
#define min_debug_print(...)
#endif
#endif // MIN_H
