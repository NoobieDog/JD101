/*
  JD101.ino - Replacement firmware for the JDiag JD-101 ODB2 Diagnostics Reader.

  Use at your own risk!

  This code uses the Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
    Copyright (c) 2016, olikraus@gmail.com
    All rights reserved.

  Things that work in this version:

    LCD Sreen
      CLK = PB13
      DATA = PB15
      CS = PB12
      DC = PC8
      RST = PC9
       
    Buttons
      Down = PC10
      Up = PC13
      Enter = PC11
      Back = PC12
      
    USB Serial Console
      CDC - USB
      
    SPI flash Memory
      MISO = PA6
      MOSI = PA7
      CS = PA4
      Clock = PA5

  Things that DONT work in this version:
    
    ODB2 Connector:
      CAN High = PB9
      CAN Low = PB8
      K-line = ?
      L-Line = ?
      SAE + = ?
      SAE - = ?
  
    Voltage
      PIN = No Idea
    
    Crystal Osc
      RCC_OSC_OUT = PD1
      RCC_OSC_IN = PD0
    
    Additional USART (unpopulated USB connector on PCB)
      RX = PB11
      TX = PB10
*/

#include <U8g2lib.h>
#include <SPI.h>
#include "W25Q64JV.h"

U8G2_ST7567_ENH_DG128064I_1_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ PB13, /* data=*/ PB15, /* cs=*/ PB12, /* dc=*/ PC8, /* reset=*/ PC9);

char strbuf[64];           // Chars to output buffer

uint8_t current_selection = 0;

const char *menu1 =
      "TEST APP\n"
      "SETTINGS\n";

const char *menu2 =
      "Screen Test\n"
      "SPI Flash Test\n";

void drawText(byte X,byte Y,const char *text){
    u8g2.firstPage();
    do {
       u8g2.drawStr(X,Y,text);
    } while ( u8g2.nextPage() );
    delay(1000);
}

void select_menu(const char *nameMenu, const char *select_menu){
  current_selection = u8g2.userInterfaceSelectionList(nameMenu, current_selection, select_menu);
  delay(15);         // debounce
}

void draw_TEST_APP(){
  u8g2.firstPage();
  do {
     u8g2.drawFrame(0, 0, 127, 16);
     u8g2.drawStr(34,12,"Test App");
     u8g2.drawFrame(0, 17, 127, 47);
     u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
     u8g2.drawGlyph(47, 56, 69);
     u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);  // set the target font
  } while ( u8g2.nextPage() );
  delay(2000);
}

void draw_SPI_TEST(){
  u8g2.firstPage();
  do {
     u8g2.drawStr(10,10,"SPI FLASH TEST");
     itoa(MAN_ID_W25Q64JV_2(), strbuf, 16);
     u8g2.drawStr(10,25,strbuf);
     itoa(UNIQUE_ID_W25Q64JV(), strbuf, 16);
     u8g2.drawStr(10,40,strbuf);
  } while ( u8g2.nextPage() );
  delay(2000);
}

void draw_SCREEN_TEST(){
  u8g2.firstPage();
  do {
    u8g2.drawStr(23,23,"TEST");
  } while ( u8g2.nextPage() );
  delay(2000);
}

void draw_SETTINGS(){
  current_selection = 0;
  bool exit = 0;
  while(exit != 1) {
    const char *nameMenu = "Settings Menu";
    select_menu(nameMenu, menu2);
    if ( current_selection == 0 ) {         // home/cancel button
            exit = 1;
    }
    if (current_selection == 1) {         // Screen Test
            draw_SCREEN_TEST();
    }
    if (current_selection == 2) {         // SPI Flash Test
            draw_SPI_TEST();
    }
  }
}

void setup(void) {
  // initialize Serial:
  Serial.begin(115200);
  Serial.println("Starting Up");
  
  u8g2.begin(/*Select=*/ PC11, /*Right/Next=*/ U8X8_PIN_NONE, /*Left/Prev=*/ U8X8_PIN_NONE, /*Up=*/ PC13, /*Down=*/ PC10, /*Home/Cancel=*/ PC12);
  u8g2.setContrast(0x03A * 4);
  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);  // set the target font
  
  drawText(15,40,"Welcome");
  
  // initialize SPI:
  pinMode(PA4, OUTPUT); // CS select Pin for SPI Flash
  SPI.begin();
  
  //Small delay just for welcome screen to be benificial
  delay(1000);  
}

void loop(void) {
  const char *nameMenu = "Menu";
  select_menu(nameMenu, menu1);

  if ( current_selection == 0 ) {         // home/cancel button
          //
  }
  if (current_selection == 1) {         // NFC
          draw_TEST_APP();
  }
  if (current_selection == 2) {         // RFID
          draw_SETTINGS();
  } 
}

void SpiFlashTest(){
  // Initial pull High
  digitalWrite(PA4, HIGH);
  
  itoa(MAN_ID_W25Q64JV_2(), strbuf, 16);
  Serial.println(strbuf);
  delay(1000);
  itoa(UNIQUE_ID_W25Q64JV(), strbuf, 16);
  Serial.println(strbuf);

  READ_W25Q64JV(0,32);
  CHIP_ERASE_W25Q64JV();
  READ_W25Q64JV(0,32);
  WRITE_W25Q64JV(0,"NoobieDog");
  READ_W25Q64JV(0,32);
}

// read the chips unique ID (64bit)
uint64_t MAN_ID_W25Q64JV_2(){
  unsigned int result = 0;   // result to return
  byte inByte = 0;           // incoming byte from the SPI
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  byte data [4]={MANUFACT_DEVICE_ID, 0xFF, 0xFF, 0x00};
  SPI.transfer(data, 4);
  // send a value of 0 to read the first byte returned:
  result = SPI.transfer(0x00);
  result = result << 8;
  inByte = SPI.transfer(0x00);
  // combine the byte you just got with the previous one:
  result = result | inByte;
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  return result;
}

uint64_t UNIQUE_ID_W25Q64JV(){
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  byte data [5]={READ_UNIQUE_ID, 0xFF, 0xFF, 0xFF, 0xFF};
  SPI.transfer(data, 5);
  
  // receive ID byte per byte and assemble the 64bit value from the bytes
  uint8_t byte_buf;
  uint64_t ID = 0;
  byte_buf = SPI.transfer(0xFF);
  ID |= ( ((uint64_t)byte_buf) << 56 );
  byte_buf = SPI.transfer(0xFF);
  ID |= ( ((uint64_t)byte_buf) << 48 );
  byte_buf = SPI.transfer(0xFF);
  ID |= ( ((uint64_t)byte_buf) << 40 );
  byte_buf = SPI.transfer(0xFF);
  ID |= ( ((uint64_t)byte_buf) << 32 );
  byte_buf = SPI.transfer(0xFF);
  ID |= ( ((uint64_t)byte_buf) << 24 );
  byte_buf = SPI.transfer(0xFF);
  ID |= ( ((uint64_t)byte_buf) << 16 );
  byte_buf = SPI.transfer(0xFF);
  ID |= ( ((uint64_t)byte_buf) << 8 );
  byte_buf = SPI.transfer(0xFF);
  ID |= ( ((uint64_t)byte_buf) << 0 );
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  return ID;
}

void READ_W25Q64JV(uint32_t address, uint32_t length){
  char strbuf[64];           // Chars to output buffer
  Serial.println("READING SPI"); 
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW); 
  // send instruction
  SPI.transfer(READ_DATA);
  // send 24bit address MSB first
  SPI.transfer( (uint8_t)(address>>16) );
  SPI.transfer( (uint8_t)(address>>8) );
  SPI.transfer( (uint8_t)(address) );
  for(uint32_t byte_counter = 0; byte_counter<length; byte_counter++){
    itoa(SPI.transfer(0xFF), strbuf, 16);
    Serial.print(strbuf);
  }
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  Serial.println(""); 
}

void CHIP_ERASE_W25Q64JV(){
  Serial.println("ERASING SPI"); 
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  SPI.transfer(WRITE_ENABLE);
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  delay(10);
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  SPI.transfer(CHIP_ERASE);
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  delay(10);
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // poll status register 1 to check the BUSY bit which indicates that erase procedure is over
  WAIT_BUSY_FLAG_W25Q64JV();
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);

  Serial.println("SPI Chip erase DONE!!!");
}

void WAIT_BUSY_FLAG_W25Q64JV(){
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  SPI.transfer(READ_STATUS_REG_1);
  // poll status register 1 and check busy bit
  while( SPI.transfer(0xFF) & STATUS_REG_1_BUSY_BIT );
  return;
}

uint8_t GET_STATUS_W25Q64JV(){
  digitalWrite(PA4, LOW);
  SPI.transfer(READ_STATUS_REG_1);
  // receive register content
  uint8_t register_content = SPI.transfer(0xFF);
  digitalWrite(PA4, HIGH);
  return register_content;
}

// write a page of 1-256bytes to previously erased(!!!) locations
void WRITE_W25Q64JV(uint32_t address, String data){
  Serial.println("WRITING TO SPI"); 
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  SPI.transfer(WRITE_ENABLE);
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  delay(10);
  
  Serial.print("Data Length: ");
  Serial.println(data.length());
  
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  SPI.transfer(PAGE_PROGRAM);
  // send 24bit address MSB first
  SPI.transfer( (uint8_t)(address>>16) );
  SPI.transfer( (uint8_t)(address>>8) );
  SPI.transfer( (uint8_t)(address) );
  // If an entire 256 byte page is to be programmed, the last address byte (the 8 LSB) should be set to 0.
  if(data.length() == 256){
    SPI.transfer(0);
  }else{
    SPI.transfer( (uint8_t)(address) );
  }
  for(uint32_t byte_counter = 0; byte_counter<data.length(); byte_counter++){
    SPI.transfer(data[byte_counter]);
  }
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  delay(10);
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // poll status register 1 to check the BUSY bit which indicates that writing procedure is over
  WAIT_BUSY_FLAG_W25Q64JV();
  digitalWrite(PA4, HIGH);
  Serial.println("Written Data to SPI");
}

// 4kb Sector erase
void SEC_ERASE_W25Q64JV(uint32_t address){
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, HIGH);
  // send instruction
  SPI.transfer(WRITE_ENABLE);
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  delay(10);
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  SPI.transfer(SECTOR_ERASE_4KB);
  // send 24bit address MSB first
  SPI.transfer( (uint8_t)(address>>16) );
  SPI.transfer( (uint8_t)(address>>8) );
  SPI.transfer( (uint8_t)(address) );
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  delay(10);
  digitalWrite(PA4, LOW);
  WAIT_BUSY_FLAG_W25Q64JV();
  digitalWrite(PA4, HIGH);
  Serial.println("Erased 4kb Block!");
}

// erases a block of 32Kb
void BLOCK_ERASE_32KB_W25Q64JV(uint32_t address){
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, HIGH);
  // send instruction
  SPI.transfer(WRITE_ENABLE);
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  delay(10);
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  SPI.transfer(BLOCK_ERASE_32KB);
  SPI.transfer( (uint8_t)(address>>16) );
  SPI.transfer( (uint8_t)(address>>8) );
  SPI.transfer( (uint8_t)(address) );
  digitalWrite(PA4, HIGH);
  delay(10);
  digitalWrite(PA4, LOW);
  WAIT_BUSY_FLAG_W25Q64JV();
  digitalWrite(PA4, HIGH);
  Serial.println("Erased 32kb Block!");
}

// erases a block of 64Kb
void BLOCK_ERASE_64KB_W25Q64JV(uint32_t address){
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  SPI.transfer(WRITE_ENABLE);
  // CS high, transmission finished
  digitalWrite(PA4, HIGH);
  delay(10);
  // CS low, SPI slave starts to listen
  digitalWrite(PA4, LOW);
  // send instruction
  SPI.transfer(BLOCK_ERASE_64KB);
  SPI.transfer( (uint8_t)(address>>16) );
  SPI.transfer( (uint8_t)(address>>8) );
  SPI.transfer( (uint8_t)(address) );
  digitalWrite(PA4, HIGH);
  delay(10);
  digitalWrite(PA4, LOW);
  WAIT_BUSY_FLAG_W25Q64JV();
  digitalWrite(PA4, HIGH);
  Serial.println("Erased 64kb Block!");
}

void reset_W25Q64JV(){
  digitalWrite(PA4, LOW);
  SPI.transfer(ENABLE_RESET);
  digitalWrite(PA4, HIGH);
  delay(10);
  digitalWrite(PA4, LOW);
  SPI.transfer(RESET_DEVICE);
  digitalWrite(PA4, HIGH);
}
