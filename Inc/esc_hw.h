/*
 *  esc_hw.h
 *
 *  Created on: 2021/02/20
 *      Author: Yuki
 */

#ifndef __ESC_HW_H
#define __ESC_HW_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Standard includes. */
#include <stdint.h>

#define ESC_ID_REV_REG  0x050
#define ESC_BYTE_TEST_REG  0x064

#define BYTE_TEST   0x87654321

void lan9252_Reset_assert(void);
void lan9252_Reset_negate(void);

void spi_select (int8_t board);
void spi_unselect (int8_t board);
void write (int8_t board, uint8_t *data, uint8_t size);
void read (int8_t board, uint8_t *result, uint8_t size);

void lan9252_write_32 (uint16_t address, uint32_t val);
uint32_t lan9252_read_32 (uint32_t address);

#ifdef __cplusplus
}
#endif
#endif /* __ESC_HW_H */
