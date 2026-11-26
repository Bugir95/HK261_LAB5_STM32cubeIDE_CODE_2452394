#include "command_parser.h"


static volatile uint8_t rx_buffer[MAX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

volatile CommandType command_data = COMMAND_NONE;
volatile uint8_t command_flag = 0;

void command_parser_receive_byte(uint8_t data)
{
    uint8_t next = (uint8_t)((rx_head + 1U) % MAX_BUFFER_SIZE);

    if (next == rx_tail)
    {
        rx_tail = (uint8_t)((rx_tail + 1U) % MAX_BUFFER_SIZE);
    }

    rx_buffer[rx_head] = data;
    rx_head = next;
}

void command_parser_fsm(void)
{
    static uint8_t state = 0;
    static uint8_t command_index = 0;
    static char command[3];

    if (command_flag)
    {
        return;
    }

    while (rx_tail != rx_head)
    {
        uint8_t data = rx_buffer[rx_tail];
        rx_tail = (uint8_t)((rx_tail + 1U) % MAX_BUFFER_SIZE);

        switch (state)
        {
        case 0:
            if (data == '!')
            {
                state = 1;
                command_index = 0;
            }
            break;

        case 1:
            if (data == '!')
            {
                command_index = 0;
            }
            else if (data == '#')
            {
                if (command_index == 3U)
                {
                    if (command[0] == 'R' &&
                        command[1] == 'S' &&
                        command[2] == 'T')
                    {
                        command_data = COMMAND_RST;
                        command_flag = 1;
                        state = 0;
                        return;
                    }

                }
                else if (command_index == 2U &&
                         command[0] == 'O' &&
                         command[1] == 'K')
                {
                    command_data = COMMAND_OK;
                    command_flag = 1;
                    state = 0;
                    return;
                }

                state = 0;
                command_index = 0;
            }
            else
            {
                if (command_index < 3U)
                {
                    command[command_index++] = (char)data;
                }
                else
                {
                    state = 0;
                    command_index = 0;
                }
            }
            break;

        default:
            state = 0;
            command_index = 0;
            break;
        }
    }
}
