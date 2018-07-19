#include "nand_flash_io.h"
#include "main.h"
#include "gpio.h"

typedef enum
{
  DATA_IO_DIR_INPUT,
  DATA_IO_DIR_OUTPUT
}data_io_dir_t;

static data_io_dir_t io_dir=DATA_IO_DIR_OUTPUT;

static void nand_flash_hal_io_ale_ctrl(io_status_t status)
{
HAL_GPIO_WritePin(ALE_GPIO_Port,ALE_Pin,(GPIO_PinState)status);
}
static void nand_flash_hal_io_cle_ctrl(io_status_t status)
{
HAL_GPIO_WritePin(CLE_GPIO_Port,CLE_Pin,(GPIO_PinState)status);
}
static void nand_flash_hal_io_ce_ctrl(io_status_t status)
{
HAL_GPIO_WritePin(CE_GPIO_Port,CE_Pin,(GPIO_PinState)status);
}
static void nand_flash_hal_io_we_ctrl(io_status_t status)
{
HAL_GPIO_WritePin(WE_GPIO_Port,WE_Pin,(GPIO_PinState)status);
}
static void nand_flash_hal_io_re_ctrl(io_status_t status)
{
HAL_GPIO_WritePin(RE_GPIO_Port,RE_Pin,(GPIO_PinState)status);
}

static void  data_io_init_output()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /*Configure GPIO pins : PAPin PAPin PAPin PAPin 
                           PAPin PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = D0_Pin|D1_Pin|D2_Pin|D3_Pin 
                          |D4_Pin|D5_Pin|D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void  data_io_init_input()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /*Configure GPIO pin Output Level */
 // HAL_GPIO_WritePin(GPIOA, D0_Pin|D1_Pin|D2_Pin|D3_Pin 
       //                   |D4_Pin|D5_Pin|D6_Pin|D7_Pin, GPIO_PIN_SET);
  
  /*Configure GPIO pins : PAPin PAPin PAPin PAPin 
                           PAPin PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = D0_Pin|D1_Pin|D2_Pin|D3_Pin 
                          |D4_Pin|D5_Pin|D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static uint8_t  nand_flash_hal_io_data_in()
{
GPIO_PinState status;
uint8_t i;
uint8_t data_in=0;

if(io_dir == DATA_IO_DIR_OUTPUT){
data_io_init_input();
io_dir = DATA_IO_DIR_INPUT;
}

for(i=0;i<8;i++){
status = HAL_GPIO_ReadPin(D0_GPIO_Port,D0_Pin<<i);
if(status == GPIO_PIN_SET){
data_in|=1<<i;
 }
}
return data_in;
}
static void nand_flash_hal_io_data_out(uint8_t data_out)
{
  uint8_t i;
  if(io_dir == DATA_IO_DIR_INPUT){
  data_io_init_output();
  io_dir = DATA_IO_DIR_OUTPUT;
  }
  
  for(i=0;i<8;i++){
  if(data_out & (1<<i)){
  HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin<<i,GPIO_PIN_SET);  
  }else{
  HAL_GPIO_WritePin(D0_GPIO_Port,D0_Pin<<i,GPIO_PIN_RESET);  
  }
  }
}
static io_status_t nand_flash_hal_io_rb_in(void)
{
GPIO_PinState status;
status = HAL_GPIO_ReadPin(RB_GPIO_Port,RB_Pin);
return (io_status_t)status;
}

 nand_flash_hal_io_t nand_hal_io={
 .io_ale_ctrl=nand_flash_hal_io_ale_ctrl,
 .io_cle_ctrl=nand_flash_hal_io_cle_ctrl,
 .io_ce_ctrl=nand_flash_hal_io_ce_ctrl,
 .io_re_ctrl=nand_flash_hal_io_re_ctrl,
 .io_we_ctrl=nand_flash_hal_io_we_ctrl,
 .io_data_in=nand_flash_hal_io_data_in,
 .io_data_out=nand_flash_hal_io_data_out,
 .io_rb_in=nand_flash_hal_io_rb_in
        
};
