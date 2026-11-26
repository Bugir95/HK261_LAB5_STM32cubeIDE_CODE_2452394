/*
 * command_parser.h
 */

#ifndef INC_COMMAND_PARSER_H_
#define INC_COMMAND_PARSER_H_

#include <stdint.h>

#define MAX_BUFFER_SIZE 30

typedef enum
{
    COMMAND_NONE = 0,
    COMMAND_RST,
    COMMAND_OK
} CommandType;

extern volatile CommandType command_data;
extern volatile uint8_t command_flag;

/* Called from the UART receive interrupt callback. */
void command_parser_receive_byte(uint8_t data);

/* Called repeatedly from the main loop. */
void command_parser_fsm(void);

#endif /* INC_COMMAND_PARSER_H_ */
