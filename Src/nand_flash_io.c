#include "nand_flash_io.h"
#include "log.h"
#define  LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#define  LOG_MODULE_NAME     "nand"


#ifndef  NULL
#define  NULL          (void*)0
#endif

#ifndef  TRUE
#define  TRUE           1
#endif

#ifndef  FALSE
#define  FALSE          0
#endif


#define  ASSERT_NULL_POINTER(x)  \
{                                \
  if((x) == NULL){               \
  return -1;                     \
  }                              \
} 


#define  STATUS_READ_CMD          0x70

#define  ID_READ_CMD              0x90
#define  ID_READ_ADDR             0x00

#define  PAGE_READ_PREPARE_CMD    0x00
#define  PAGE_READ_EXECUTE_CMD    0x30

#define  PAGE_PROGRAM_PREPARE_CMD 0x80
#define  PAGE_PROGRAM_EXECUTE_CMD 0x10

#define  BLOCK_ERASE_PREPARE_CMD  0x60
#define  BLOCK_ERASE_EXECUTE_CMD  0xD0

#define  RESET_CMD                0xFF

#define  PAGE_PROGRAM_TIMEOUT     2
#define  BLOCK_ERASE_TIMEOUT      4

static void nand_flash_cmd(uint8_t cmd);

static nand_flash_hal_io_t *hal_io;

int nand_flash_register_hal_io(nand_flash_hal_io_t *io)
{
ASSERT_NULL_POINTER(io);
ASSERT_NULL_POINTER(io->io_ale_ctrl);
ASSERT_NULL_POINTER(io->io_cle_ctrl);
ASSERT_NULL_POINTER(io->io_ce_ctrl);
ASSERT_NULL_POINTER(io->io_re_ctrl);
ASSERT_NULL_POINTER(io->io_we_ctrl);
ASSERT_NULL_POINTER(io->io_rb_in);
ASSERT_NULL_POINTER(io->io_data_out);
ASSERT_NULL_POINTER(io->io_data_in);
hal_io=io;

return 0;
}
int nand_flash_init(void)
{
 ASSERT_NULL_POINTER(hal_io);  
 hal_io->io_ce_ctrl(IO_SET);
 hal_io->io_we_ctrl(IO_SET);
 hal_io->io_re_ctrl(IO_SET);
 hal_io->io_cle_ctrl(IO_RESET); 
 hal_io->io_ale_ctrl(IO_RESET); 
 
 hal_io->io_ce_ctrl(IO_RESET);
 nand_flash_cmd(RESET_CMD);
 hal_io->io_ce_ctrl(IO_SET);
 
 osDelay(100);

 return 0;
}

static void nand_flash_byte_write(uint8_t byte)
{
 hal_io->io_we_ctrl(IO_RESET);
 hal_io->io_data_out(byte);
 hal_io->io_we_ctrl(IO_SET);   
}
static uint8_t nand_flash_byte_read(void)
{
 uint8_t read;
 hal_io->io_re_ctrl(IO_RESET);
 read = hal_io->io_data_in();
 hal_io->io_re_ctrl(IO_SET); 
 return read;
}


static void nand_flash_cmd(uint8_t cmd)
{
 hal_io->io_cle_ctrl(IO_SET);
 nand_flash_byte_write(cmd); 
 hal_io->io_cle_ctrl(IO_RESET);
}

static void nand_flash_page_addr(uint32_t addr)
{
 hal_io->io_ale_ctrl(IO_SET);
 
 nand_flash_byte_write(0); 
 nand_flash_byte_write(0); 
 
 nand_flash_byte_write(addr & 0xFF); 
 nand_flash_byte_write((addr >> 8) & 0xFF); 
 nand_flash_byte_write((addr >> 16) & 0xFF); 
 
 hal_io->io_ale_ctrl(IO_RESET);
}
static void nand_flash_id_addr(uint8_t addr)
{
 hal_io->io_ale_ctrl(IO_SET);
 nand_flash_byte_write(addr); 
 hal_io->io_ale_ctrl(IO_RESET);
}

static void nand_flash_block_addr(uint16_t addr)
{
 uint32_t page_addr;
 page_addr = (uint32_t)addr << 12;
 
 hal_io->io_ale_ctrl(IO_SET);
 
 nand_flash_byte_write(page_addr & 0xFF); 
 nand_flash_byte_write((page_addr >> 8) & 0xFF); 
 nand_flash_byte_write((page_addr >> 16) & 0xFF); 
 
 hal_io->io_ale_ctrl(IO_RESET);
}

static bool is_nand_flash_busy(void)
{
  if(hal_io->io_rb_in() == IO_RESET){
  return TRUE;
  }
  return FALSE;
}

static bool is_nand_flash_error(void)
{
 uint8_t status;
 nand_flash_cmd(STATUS_READ_CMD);
 status = nand_flash_byte_read();
 
 if(status & 0x01){
 return TRUE;
 }
 
 return FALSE;
}

int nand_flash_id_read(uint8_t *id)
{
 uint8_t i;
 ASSERT_NULL_POINTER(id);
 
 hal_io->io_ce_ctrl(IO_RESET);
 
 nand_flash_cmd(ID_READ_CMD);
 nand_flash_id_addr(ID_READ_ADDR);
 
 for(i=0;i<5;i++){
 *id++ = nand_flash_byte_read(); 
 }
 hal_io->io_ce_ctrl(IO_SET);
 
 return 0;
}

int nand_flash_page_read(uint32_t addr,uint8_t *buff,uint16_t len)
{
 uint16_t i;
 ASSERT_NULL_POINTER(buff);
 ASSERT_NULL_POINTER(hal_io);
 hal_io->io_ce_ctrl(IO_RESET);
 nand_flash_cmd(PAGE_READ_PREPARE_CMD);
 nand_flash_page_addr(addr);
 nand_flash_cmd(PAGE_READ_EXECUTE_CMD);
 
 for(i=0;i<len;i++){
 *buff++ = nand_flash_byte_read(); 
 }
 hal_io->io_ce_ctrl(IO_SET);
 
 return i;
}

static int nand_flash_wait(uint8_t timeout)
{
 int status ;
 uint8_t time=0;
  while(1){
  if(time < timeout){
  osDelay(1);  
  time++;
  if(is_nand_flash_busy() == TRUE){
   continue;
  }else{  
  if(is_nand_flash_error() == TRUE){
  status=-1;
  break;
  }else{
  status =0;
  break;
  }
  }
  }else{
  log_error("wait timeout.\r\n");
  status =-1;
  break;
  }
  }
 return status;
}

int nand_flash_page_program(uint32_t addr,uint8_t *buff,uint16_t len)
{
 uint16_t i;
 int status;
 
 ASSERT_NULL_POINTER(buff);
 ASSERT_NULL_POINTER(hal_io);
 
 hal_io->io_ce_ctrl(IO_RESET);
 nand_flash_cmd(PAGE_PROGRAM_PREPARE_CMD);
 nand_flash_page_addr(addr);
 
 for(i=0;i<len;i++){
 nand_flash_byte_write(*buff++); 
 }
 nand_flash_cmd(PAGE_PROGRAM_EXECUTE_CMD);  
 
 status = nand_flash_wait(PAGE_PROGRAM_TIMEOUT);
 hal_io->io_ce_ctrl(IO_SET);
 
 return status;
}

int nand_flash_block_erase(uint16_t addr)
{
 int status;
 ASSERT_NULL_POINTER(hal_io);
 
 hal_io->io_ce_ctrl(IO_RESET);
 
 nand_flash_cmd(BLOCK_ERASE_PREPARE_CMD);
 nand_flash_block_addr(addr); 
 nand_flash_cmd(BLOCK_ERASE_EXECUTE_CMD);

 status = nand_flash_wait(BLOCK_ERASE_TIMEOUT);
 
 hal_io->io_ce_ctrl(IO_SET);
 return status;
}



int nand_flash_page_copy_back(uint16_t block_addr)
{
  ASSERT_NULL_POINTER(hal_io);
  return 0;
}
                         
                         
                         
                         
                         
                         
                         
                         
                         
                         