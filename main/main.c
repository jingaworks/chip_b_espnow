#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_random.h"

#include "interchip_comm.h"
#include "protocol_types.h"
#include "config.h"

static const char *TAG = "APP_MAIN";

// Sample callback for received packets (chip-specific handling)
static void sample_packet_callback(
    uint8_t source_device,
    uint8_t msg_type,
    uint8_t seq_number,
    const uint8_t *payload,
    uint8_t payload_len,
    void *user_ctx)
{
    // Ignore self-sent packets (safety)
    if (source_device == DEVICE_OWN_ID) {
        ESP_LOGW(TAG, "Ignoring self-sent packet (seq=%u)", seq_number);
        return;
    }

    ESP_LOGI(TAG, "Received packet: from 0x%02X | type 0x%02X | seq %u | len %u",
             source_device, msg_type, seq_number, payload_len);

    // Optional: Log payload snippet
    if (payload_len > 0) {
        ESP_LOG_BUFFER_HEX(TAG, payload, payload_len > 16 ? 16 : payload_len);
    }

    // Dummy handling based on message type (expand per chip)
    switch (msg_type) {
        case MSG_TOUCH_EVENT:
            ESP_LOGI(TAG, "Handling touch event (e.g., for Chip A: forward to LVGL)");
            // Send ACK
            ack_nack_t ack = { .seq = seq_number, .error_code = ERR_OK };
            interchip_send(source_device, MSG_ACK, seq_number, &ack, sizeof(ack));
            break;

        case MSG_ESPNOW_DATA:
            ESP_LOGI(TAG, "Handling ESP-NOW data (e.g., for Chip A: process sensor data)");
            break;

        case MSG_STATUS_UPDATE:
            ESP_LOGI(TAG, "Handling status update (e.g., for Chip C: update UI)");
            break;

        case MSG_NOTIFICATION:
            ESP_LOGI(TAG, "Handling notification (e.g., for Chip C: display popup)");
            break;

        default:
            ESP_LOGW(TAG, "Unhandled message type: 0x%02X", msg_type);
            // Send NACK
            ack_nack_t nack = { .seq = seq_number, .error_code = ERR_UNKNOWN_TYPE };
            interchip_send(source_device, MSG_NACK, seq_number, &nack, sizeof(nack));
            break;
    }
}

void app_main(void)
{
    // Initialize the interchip_comm component
    ESP_ERROR_CHECK(interchip_init());

    // Register the callback (with NULL user context for simplicity)
    interchip_register_callback(sample_packet_callback, NULL);

    // Delay to allow initialization
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Send a dummy notification to test (e.g., target Chip C: Display)
    notification_t test_notify = {
        .severity = 0,  // Info level
        .duration_sec = 5,
        .title = "Test Notification",
        .message = "Interchip comm is active!"
    };
    interchip_send(DEVICE_DISPLAY, MSG_NOTIFICATION, 0, &test_notify, sizeof(test_notify));

    ESP_LOGI(TAG, "Dummy main loop started. Monitoring for packets...");

    // Infinite loop for ongoing operation
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        // Optional: Add periodic sends or other logic here
    }
}