/*
*
*Author: darwinzh
*
*/
#include "nrf24l01.h"

/*------------------ Function Prototype ----------*/

void RF24L01_send_command(uint8_t command);
void RF24L01_write_register(uint8_t register_addr, uint8_t *value, uint8_t length);
uint8_t RF24L01_read_register(uint8_t register_addr);
RF24L01_reg_STATUS_content RF24L01_get_status(void);

/*------------------ Functions -----------------*/

/** Function init  */
void RF24L01_init(void) {
  wiringPiSetup();

  //CE
  RF24L01_CE_setOutput();
  RF24L01_CE_setLow();

  //SPI
  if( wiringPiSPISetup(0, 5000000) == -1 )
  	printf("SPI0 No se Inicia Correctamente.\n");

}//End init

/** Función Setup */
void RF24L01_setup(uint8_t *tx_addr, uint8_t *rx_addr, uint8_t channel) {
  RF24L01_CE_setLow(); //CE -> Low

  /* Configuration Register(0x00)  */
  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  config.PRIM_RX = 1;//RX/TX control(1: PRX, 0:PTX)
  config.PWR_UP  = 0;//1: Power up, 0: Power Down
  config.CRCO    = 0;//CRC (0-1 byte, 1-2 bytes)
  config.EN_CRC  = 1;//Enable CRC. Forced high if one of the bits in the EN_AA is high
  config.MASK_MAX_RT = 0;//Mask interrupt caused by MAX_RT(0:Reflect MAX_RT as active low interrupt on the IRQ)
  config.MASK_TX_DS  = 0;//Mask interrupt caused by TX_DS (0:Reflect TX_DS  as active low interrupt on the IRQ)
  config.MASK_RX_DR  = 0;//Mask interrupt caused by RX_DR (0:Reflect RX_DR  as active low interrupt on the IRQ)
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

 /* Enable Auto Acknowledgment(0x01) */
  RF24L01_reg_EN_AA_content EN_AA;
  *((uint8_t *)&EN_AA) = 0;
  EN_AA.ENAA_P0 = 1;//Enable data pipe 0
  EN_AA.ENAA_P1 = 1;//Enable data pipe 1
  RF24L01_write_register(RF24L01_reg_EN_AA, ((uint8_t *)&EN_AA), 1);

  /* Enable RX Address(0x02) */
  RF24L01_reg_EN_RXADDR_content RX_ADDR;
  *((uint8_t *)&RX_ADDR) = 0;
  RX_ADDR.ERX_P0 = 1;//Enable data pipe 0
  RX_ADDR.ERX_P1 = 1;//Enable data pipe 1
  RF24L01_write_register(RF24L01_reg_EN_RXADDR, ((uint8_t *)&RX_ADDR), 1);

  /*Setup of Addres Widths(0x03) */
  RF24L01_reg_SETUP_AW_content SETUP_AW;
  *((uint8_t *)&SETUP_AW) = 0;
  SETUP_AW.AW = 0x03;//RX/TX Address field width('11' - 5 bytes)
  RF24L01_write_register(RF24L01_reg_SETUP_AW, ((uint8_t *)&SETUP_AW), 1);

  /*Setup of Automatic Retransmission(0x04) */
  RF24L01_reg_SETUP_RETR_content SETUP_RETR;
  *((uint8_t *)&SETUP_RETR) = 0;
  SETUP_RETR.ARC = 0x03;//Auto Retransmit Count(Up to 3 Re-Transmit on fail of AA)
  SETUP_RETR.ARD = 0x01;//Auto Retransmit Delay(wait 500uS)
  RF24L01_write_register(RF24L01_reg_SETUP_RETR, ((uint8_t *)&SETUP_RETR), 1);

  /* RF Channel(0x05) */
  RF24L01_reg_RF_CH_content RF_CH;
  *((uint8_t *)&RF_CH) = 0;
  RF_CH.RF_CH = channel;//Sets the frequency channel nRF24L01 operates on
  RF24L01_write_register(RF24L01_reg_RF_CH, ((uint8_t *)&RF_CH), 1);

  /* RF Setup Register(0x06) */
  RF24L01_reg_RF_SETUP_content RF_SETUP;
  *((uint8_t *)&RF_SETUP) = 0;
  RF_SETUP.LNA_HCURR = 0x01;//Setup LNA gain
  RF_SETUP.RF_PWR    = 0x03;//Set RF output power in TX mode('11' - 0dBm)
  RF_SETUP.RF_DR     = 0x01;//Air Data Rate  ('1' - 2Mbps)
  RF24L01_write_register(RF24L01_reg_RF_SETUP, ((uint8_t *)&RF_SETUP), 1);

  /* Escribimos la direccion rx en pipe0(0x0A)  */
  RF24L01_write_register(RF24L01_reg_RX_ADDR_P0, rx_addr, 5);

  /* Escribimos la direccion en tx(0x10)  */
  RF24L01_write_register(RF24L01_reg_TX_ADDR, tx_addr, 5);

  /* RX payload in data pipe0 (0x11) */
  RF24L01_reg_RX_PW_P0_content RX_PW_P0;
  *((uint8_t *)&RX_PW_P0) = 0;
  RX_PW_P0.RX_PW_P0 = 0x20;//Number of bytes in RX payload in data pipe 0 (32 bytes).
  RF24L01_write_register(RF24L01_reg_RX_PW_P0, ((uint8_t *)&RX_PW_P0), 1);

  /* Enable dynamic payload length */
  RF24L01_reg_DYNPD_content DYNPD;
  *((uint8_t *)&DYNPD) = 0;
  RF24L01_write_register(RF24L01_reg_DYNPD, ((uint8_t *)&DYNPD), 1);

}//End Setup

/** Function send command */
void RF24L01_send_command(uint8_t command) {
  uint8_t aux[2];
  aux[0] = command;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);
}//End Send Command

/** Function Write Register */
void RF24L01_write_register(uint8_t register_addr, uint8_t *value, uint8_t length) {
  uint8_t i, aux[length+1];
  //Address and write command
  aux[0] = RF24L01_command_W_REGISTER | register_addr;
  for (i = 1; i < (length+1); i++) {
  	aux[i] = value[i-1];
  }
   //Send data
   wiringPiSPIDataRW( CHANNEL, aux, (length+1) );
}//End Werite Register

/** Function read register */
uint8_t RF24L01_read_register(uint8_t register_addr) {
  //Send address and read command
  aux[0] = RF24L01_command_R_REGISTER | register_addr;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);

  return aux[1];
}//End Read Register

/** Function Set Mode TX */
void RF24L01_set_mode_TX(void) {
  RF24L01_send_command(RF24L01_command_FLUSH_TX);

  //CE->LOW
  RF24L01_CE_setLow();

  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  config.PRIM_RX 	 = 0;//PTX
  config.PWR_UP  	 = 1;//Power Up
  config.CRCO    	 = 0;//Enconding 1 byte
  config.EN_CRC      = 1;//Enable CRC
  config.MASK_MAX_RT = 0;//Active low interrupt
  config.MASK_TX_DS  = 0;//Active low interrupt
  config.MASK_RX_DR  = 0;//Active low interrupt
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

  //Clear the status register to discard any data in the buffers
  RF24L01_reg_STATUS_content a;
  *((uint8_t *) &a) = 0;
  a.RX_DR  = 1;
  a.MAX_RT = 1;
  a.TX_DS  = 1;
  RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t *) &a, 1);

}//End Set Mode TX

/** Function Set Mode RX */
void RF24L01_set_mode_RX(void) {
  RF24L01_reg_CONFIG_content config;
  *((uint8_t *)&config) = 0;
  config.PRIM_RX 	 = 1;//PRX
  config.PWR_UP 	 = 1;//Power Up
  config.CRCO    	 = 0;//Enconding 1 byte
  config.EN_CRC 	 = 1;//Enable CRC
  config.MASK_MAX_RT = 0;//Active low interrupt
  config.MASK_TX_DS  = 0;//Active low interrupt
  config.MASK_RX_DR  = 0;//Active low interrupt
  RF24L01_write_register(RF24L01_reg_CONFIG, ((uint8_t *)&config), 1);

  //Clear the status register to discard any data in the buffers
  RF24L01_reg_STATUS_content a;
  *((uint8_t *) &a) = 0;
  a.RX_DR  = 1;
  a.MAX_RT = 1;
  a.TX_DS  = 1;
  RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t *) &a, 1);

  RF24L01_send_command(RF24L01_command_FLUSH_RX);

  //CE -> High
  RF24L01_CE_setHigh();

}//End Set Mode RX

/** Function Get Status  */
RF24L01_reg_STATUS_content RF24L01_get_status(void) {
  uint8_t status;
  uint8_t aux[2];

  //Send address and command
  aux[0] = RF24L01_reg_STATUS;
  aux[1] = 0x00;
  wiringPiSPIDataRW(CHANNEL, aux, 2);
  status = aux[1];
  return *((RF24L01_reg_STATUS_content *) &status);

}//End Get Status

/** Function Write Payload */
void RF24L01_write_payload(uint8_t *data, uint8_t length) {
  uint8_t aux[33], i;

  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();
  if (a.MAX_RT == 1) {
    //If MAX_RT, clears it so we can send data
    *((uint8_t *) &a) = 0;
    a.TX_DS = 1;
    RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t *) &a, 1);
  }

  //Send address and command
  aux[0] = RF24L01_command_W_TX_PAYLOAD;

  //Send data
  for(i = 1; i < length+1; i++){
  	aux[i] = data[i-1];
  }

  wiringPiSPIDataRW(CHANNEL, aux, sizeof(aux));

  RF24L01_send_command(RF24L01_command_W_ACK_PAYLOAD);

  //Generates an impulsion for CE to send the data
  RF24L01_CE_setHigh();
  uint16_t delay = 0xFF;
  while(delay--);
  RF24L01_CE_setLow();

  RF24L01_CE_setHigh();

}//End Write Payload

/** Function Read Payload  */
void RF24L01_read_payload(uint8_t *data, uint8_t length) {
  uint8_t i, status,aux[33];

  //Command of Read RX Payload
  aux[0] = RF24L01_command_R_RX_PAYLOAD;

  //Get data
  for (i=1; i<length+1; i++) {
  	aux[i] = 0x00;
  }

  wiringPiSPIDataRW(CHANNEL, aux, sizeof(aux));

  for(i = 1; i<length+1; i++){
  	*(data++) = aux[i];
  }

  RF24L01_write_register(RF24L01_reg_STATUS, &status, 1);

  RF24L01_send_command(RF24L01_command_FLUSH_RX);

}//End Read Payload

/** Function Was Data Sent */
uint8_t RF24L01_was_data_sent(void) {
  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();
  uint8_t res = 0;

  if (a.TX_DS) {
    res = 1;
  }
  else if (a.MAX_RT) {
    res = 2;
  }

  return res;

}//End Was Data Sent

/** Function Is Data Available */
uint8_t RF24L01_is_data_available(void) {
  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();
  return a.RX_DR;

}//End Is Data Available

/** Function Clear Interrupts */
void RF24L01_clear_interrupts(void) {
  RF24L01_reg_STATUS_content a;
  a = RF24L01_get_status();
  RF24L01_write_register(RF24L01_reg_STATUS, (uint8_t*)&a, 1);

}//End Clear Interrupts

/** Function Clear Setup */
void RF24L01_clear_setup(void){
	uint8_t aux[2], i;
	for(i=0; i<=0x1D; i++){
		aux[0]= RF24L01_command_W_REGISTER | i;
		aux[1]=0x00;
		wiringPiSPIDataRW(CHANNEL, aux, 2);
	}
}//Enc Clear Setup
