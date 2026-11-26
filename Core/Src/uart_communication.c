#include "main.h"
#include "command_parser.h"
#include "uart_communication.h"
#include <stdio.h>
#include <string.h>

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

#define ACK_TIMEOUT_MS 3000U
#define UART_TX_TIMEOUT_MS 1000U

typedef enum
{
    UART_WAIT_REQUEST = 0,
    UART_WAIT_ACK
} UartCommunicationState;

void uart_communication_fsm(void)
{
    static UartCommunicationState state = UART_WAIT_REQUEST;
    static uint32_t packet_tick = 0;
    static char previous_packet[24] = {0};
    static uint8_t packet_valid = 0;

    if (command_flag)
    {
        CommandType received_command = command_data;
        command_flag = 0;
        command_data = COMMAND_NONE;

        if (received_command == COMMAND_RST)
        {
            uint32_t adc_value = HAL_ADC_GetValue(&hadc1);

            snprintf(previous_packet,
                     sizeof(previous_packet),
                     "\r\n!ADC=%lu#",
                     (unsigned long)adc_value);

            HAL_UART_Transmit(&huart2,
                              (uint8_t *)previous_packet,
                              (uint16_t)strlen(previous_packet),
                              UART_TX_TIMEOUT_MS);

            packet_tick = HAL_GetTick();
            packet_valid = 1;
            state = UART_WAIT_ACK;
        }
        else if (received_command == COMMAND_OK)
        {
            if (state == UART_WAIT_ACK)
            {
                state = UART_WAIT_REQUEST;
            }
        }
    }

    if (state == UART_WAIT_ACK &&
        packet_valid &&
        (uint32_t)(HAL_GetTick() - packet_tick) >= ACK_TIMEOUT_MS)
    {
        HAL_UART_Transmit(&huart2,
                          (uint8_t *)previous_packet,
                          (uint16_t)strlen(previous_packet),
                          UART_TX_TIMEOUT_MS);

        packet_tick = HAL_GetTick();
    }
}
