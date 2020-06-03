/**
 * @author: DzhL
 */

#include "nrf24l01.h"


/**
 * @brief Function Prototype local
 * 
 */
void RF24L01_write_register(uint8_t register_addr, uint8_t *value, uint8_t length);
uint8_t RF24L01_read_register(uint8_t register_addr);
RF24L01_reg_STATUS_content RF24L01_get_status(void);
void RF24L01_send_command(uint8_t command); 
void RF24L01_set_mode_TX(void);


/**
 * @brief Functions
 * 
 */


/**
 * @brief Function init
 * 
 */
void RF24L01_init(void) 
{
  wiringPiSetup();
  //CE
  RF24L01_CE_setOutput();
  RF24L01_CE_setLow();
  //SPI
  if( wiringPiSPISetup(0, 10000000) == -1 )
    printf("SPI0 No se Inicia Correctamente.\n");
}//End init


/**
 * @brief Function Setup
 * 
 * @param tx_addr 
 * @param rx_addr 
 * @param channel 
 */
 void RF24L01_setup(uint8_t *tx_addr, uint8_t *rx_addr, uint8_t channel)
{
  RF24L01_CE_setLow(); //CE -> Low
  uint8_t rx[5];
  uint8_t aux;

  // Enable Auto Acknowledgment(0x01)
  RF24L01_reg_EN_AA_content EN_AA;
  *((uint8_t *)&EN_AA) = 0;
  // Enable Auto ACK: Pipe0
  EN_AA.ENAA_P0 = 1;
  RF24L01_write_register(RF24L01_reg_EN_AA, ((uint8_t *)&EN_AA), 1);

  // Enable RX Address(0x02)
  RF24L01_reg_EN_RXADDR_content RX_ADDR;
  *((uint8_t *)&RX_ADDR) = 0;
  //Enable data Pipe0
  RX_ADDR.ERX_P0 = 1;
  RF24L01_write_register(RF24L01_reg_EN_RXADDR, ((uint8_t *)&RX_ADDR), 1);

  // Setup of Addres Widths(0x03)
  RF24L01_reg_SETUP_AW_content SETUP_AW;
  *((uint8_t *)&SETUP_AW) = 0;
  //RX/TX Address field width('11' - 5 bytes)
  SETUP_AW.AW = 0x03;
  RF24L01_write_register(RF24L01_reg_SETUP_AW, ((uint8_t *)&SETUP_AW), 1);

  // Setup of Automatic Retransmission(0x04)
  RF24L01_reg_SETUP_RETR_content SETUP_RETR;
  *((uint8_t *)&SETUP_RETR) = 0;
  // Auto Retransmit Count(Up to 10 Re-Transmit on fail of AA)
  SETUP_RETR.ARC = 0x02;
  //Auto Retransmit Delay(wait 500uS)
  SETUP_RETR.ARD = 0x01;
  RF24L01_write_register(RF24L01_reg_SETUP_RETR, ((uint8_t *)&SETUP_RETR), 1);

  // RF Channel(0x05)
  RF24L01_reg_RF_CH_content RF_CH;
  *((uint8_t *)&RF_CH) = 0;
  //Sets the frequency channel nRF24L01 operates on
  RF_CH.RF_CH = channel;
  RF24L01_write_register(RF24L01_reg_RF_CH, ((uint8_t *)&RF_CH), 1);

  // RF Setup Register(0x06)
  RF24L01_reg_RF_SETUP_content RF_SETUP;
  *((uint8_t *)&RF_SETUP) = 0;
  //Set RF output power in TX mode('11' - 0dBm)
  RF_SETUP.RF_PWR     = 0x03;
  //Speed data rata 2Mbps
  RF_SETUP.RF_DR_HIGH = 0x01;
  RF24L01_write_register(RF24L01_reg_RF_SETUP, ((uint8_t *)&RF_SETUP), 1);

  // Escribimos la direccion RX en Pipe0(0x0A)
  RF24L01_write_register(RF24L01_reg_RX_ADDR_P0, rx_addr, 5);

  // Escribimos la direccion en TX(0x10)
  RF24L01_write_register(RF24L01_reg_TX_ADDR, tx_addr, 5);

  // RX payload in data pipe0 (0x11) 
  RF24L01_reg_RX_PW_P0_content RX_PW_P0;
  *((uint8_t *)&RX_PW_P0) = 0;
  //Number of bytes in RX payload in data Pipe0 (1..32 bytes).
  RX_PW_P0.RX_PW_P0 = SIZEDATA ;
  RF24L01_write_register(RF24L01_reg_RX_PW_P0, ((uint8_t *)&RX_PW_P0), 1);

  // Clear interruption
  RF24L01_clear_interrupts();
  
  RF24L01_send_command(RF24L01_command_FLUSH_TX);
  RF24L01_send_command(RF24L01_command_FLUSH_RX);

  // Configuration Register(0x00)
  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  //1: Power up, 0: Power Down
  config.PWR_UP  = 1;
   //RX/TX 1:PRX,0:PTX
  config.PRIM_RX = 1;
  //CRC (0:1 byte, 1:2 bytes)
  config.CRCO    = 1;
  //Enable CRC. Forced high if one of the bits in the EN_AA is high
  config.EN_CRC  = 1;
   RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

  // Delay 5 ms
  delay(5);

}//End Setup


/**
 * @brief Function send command
 * 
 * @param command 
 */
void RF24L01_send_command(uint8_t command) 
{
  uint8_t aux[2]; aux[0] = command;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);
}//End Send Command


/**
 * @brief Function Write Register 
 * 
 * @param register_addr 
 * @param value 
 * @param length 
 */
void RF24L01_write_register(uint8_t register_addr, uint8_t *value, uint8_t length) 
{
  uint8_t i, aux[length+1];
  //Address and write command
  aux[0] = RF24L01_command_W_REGISTER | register_addr;
  for (i = 1; i < (length+1); i++) {
    aux[i] = value[i-1];
  }
  //Send data
  wiringPiSPIDataRW( CHANNEL, aux, (length+1) );
}//End Write Register


/**
 * @brief Function Write Payload 
 * 
 * @param value 
 * @param size 
 */
void WritePayload(uint8_t value[], uint8_t size)
{
	uint8_t i, aux[size+1];
	aux[0]= RF24L01_command_W_TX_PAYLOAD;
	for(i = 1; i< (size+1); i++){
		aux[i]= value[i-1];
	}
	//Send for SPI Data
	wiringPiSPIDataRW(CHANNEL, aux, (size+1));
}//End Write Payload


/**
 * @brief Function read register 
 * 
 * @param register_addr 
 * @return uint8_t 
 */
uint8_t RF24L01_read_register(uint8_t register_addr) 
{
  //Send address and read command
  aux[0] = RF24L01_command_R_REGISTER | register_addr;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);

  return aux[1];

}//End Read Register


/**
 * @brief Function Set Mode RX 
 * 
 */
void RF24L01_set_mode_RX(void)
{
  // CE->Low
  RF24L01_CE_setLow();

  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  // RX/TX Control (1:PRX, 0:PTX)
  config.PRIM_RX 	 = 1;
  // 1:PowerUp, 0:Power Down
  config.PWR_UP 	 = 1;
  // Encode (0:1Byte, 1:2Bytes
  config.CRCO    	 = 1;
  // Enable CRC
  config.EN_CRC 	 = 1;
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

  //CE -> High
  RF24L01_CE_setHigh();

  delayMicroseconds(200);

}//End Set Mode RX


/**
 * @brief Function set mode TX
 * 
 */
void RF24L01_set_mode_TX(void)
{
  //CONFIG  TX Mode
	RF24L01_reg_CONFIG_content config;
	*((uint8_t *)&config) = 0;
	// RX/TX Control (1:PRX, 0:PTX)
	config.PRIM_RX 	 = 0;
	// 1:PowerUp, 0:Power Down
	config.PWR_UP 	 = 1;
	// Encode (0:1Byte, 1:2Bytes
	config.CRCO    	 = 1;
	// Enable CRC
	config.EN_CRC 	 = 1;
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);
} // end set mode TX


/**
 * @brief Function Send Data 
 * 
 * @param data 
 * @param size 
 */
void sendData(uint8_t data[])
{
	//CE->Low
	RF24L01_CE_setLow();
  RF24L01_set_mode_TX();
	//W_TX_PAYLOAD
	WritePayload(data, SIZEDATA);
	// CE->High
  RF24L01_CE_setHigh();
  //delay 12us
  delayMicroseconds(12);
  // CE->Low
  RF24L01_CE_setLow();
}//End Fuction Send Data


/**
 * @brief Function Get Status 
 * 
 * @return RF24L01_reg_STATUS_content 
 */
RF24L01_reg_STATUS_content RF24L01_get_status(void)
{
  uint8_t status;
  uint8_t aux[2];

  //Send address and command
  aux[0] = RF24L01_reg_STATUS;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);
  status = aux[1];

  return *((RF24L01_reg_STATUS_content *) &status);

}//End Get Status


/**
 * @brief Function Read Payload
 * 
 * @param data 
 * @param length 
 */
void RF24L01_read_payload(uint8_t *data, uint8_t length) 
{
  uint8_t i;
  uint8_t aux[length+1];

  //Command of Read RX Payload
  aux[0] = RF24L01_command_R_RX_PAYLOAD;

  //Get data
  for (i=1; i < length+1; i++) {
    aux[i] = 0x00;
  }

  wiringPiSPIDataRW(CHANNEL, aux, sizeof(aux));

  for(i = 0; i<length; i++){
    *(data++) = aux[i+1];
  }

  RF24L01_send_command(RF24L01_command_FLUSH_RX);

}//End Read Payload


/**
 * @brief Function Status 
 * 
 * @return uint8_t 
 */
uint8_t RF24L01_status(void)
{
	//Variable
	uint8_t res = 0;
	RF24L01_reg_STATUS_content status;

	status = RF24L01_get_status();

	if(status.RX_DR){
		res = 1;
	}else if(status.TX_DS){
		res = 2;
	}else if(status.MAX_RT){
		res = 3;
	}

	return res;
}//End Status


/**
 * @brief Function Clear Interrupts 
 * 
 */
void RF24L01_clear_interrupts(void)
{
	//Create variable
  RF24L01_reg_STATUS_content status;
  *((uint8_t *)&status)=0;
  status.RX_DR = 1;
  status.TX_DS = 1;
  status.MAX_RT = 1;
  RF24L01_write_register(RF24L01_reg_STATUS , ((uint8_t *)&status), 1);

}//End Clear Interrupts


/**
 * @brief Function Power Down 
 * 
 */
void RF24L01_powerDown(void)
{
  RF24L01_send_command(RF24L01_command_FLUSH_TX);
  RF24L01_send_command(RF24L01_command_FLUSH_RX);
	RF24L01_CE_setLow(); //CE -> Low
	// Configuration Register(0x00)
	RF24L01_reg_CONFIG_content config;
  	*((uint8_t *)&config) = 0;
  	//1: Power up, 0: Power Down
    config.PWR_UP  = 0;
  	//CRC (0-1 byte, 1-2 bytes)
    config.CRCO    = 1;
    //Enable CRC. Forced high if one of the bits in the EN_AA is high
    config.EN_CRC  = 1;
    RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

}// End Power Down

/*
*	End File
*/
