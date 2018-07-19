#ifndef  __NAND_FLASH_IO_H__
#define  __NAND_FLASH_IO_H__

#include "stdint.h"
#include "stdbool.h"
#include "cmsis_os.h"

#ifdef   __cplusplus
#define  NAND_FLASH_IO_BEGIN   extern "C" {
#else
#define  NAND_FLASH_IO_BEGIN   
#endif

#ifdef   __cplusplus
#define  NAND_FLASH_IO_END                }
#else
#define  NAND_FLASH_IO_END
#endif  


NAND_FLASH_IO_BEGIN

#define  PAGE_SIZE                   2048
#define  PAGE_NUM_PER_BLOCK          64
#define  BLOCK_SIZE                 (PAGE_SIZE*PAGE_NUM_PER_BLOCK)
#define  BLOCK_NUM_PER_CHIP          2048


typedef enum
{
 IO_RESET=0,
 IO_SET
}io_status_t;

typedef struct 
{
 void (*io_ale_ctrl)(io_status_t status);
 void (*io_cle_ctrl)(io_status_t status);
 void (*io_ce_ctrl)(io_status_t status);
 void (*io_we_ctrl)(io_status_t status);
 void (*io_re_ctrl)(io_status_t status);
 io_status_t (*io_rb_in)(void);
 void (*io_data_out)(uint8_t io_data);
 uint8_t (*io_data_in)(void);
}nand_flash_hal_io_t;

int nand_flash_register_hal_io(nand_flash_hal_io_t *io);
int nand_flash_init(void);
int nand_flash_id_read(uint8_t *id);
int nand_flash_page_read(uint32_t addr,uint16_t offset,uint8_t *buff,uint16_t len);
int nand_flash_page_program(uint32_t addr,uint16_t offset,uint8_t *buff,uint16_t len);
int nand_flash_block_erase(uint16_t addr);



NAND_FLASH_IO_END




#endif