
/*
   driver de la pantalla SPI
*/

#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_clock.h"
#include "lcd_driver.h"
#include "rtos_spi.h"


static LCD5110_t* configLCD;

// initialize the LCD
uint8_t LCD_begin(){

	//configure the pins to control the LCD

	gpio_pin_config_t gpio_output_config = {
				        kGPIO_DigitalOutput,
				        1,
				    };

	configLCD = LCD5110_make(0,0);

	// USING PTC AS GPIO FOR THE LCD 5110
	configLCD->rstPin = 4;
	configLCD->kCePin = 0;
	configLCD->kDcPin = 12;

	//clocking to portC
	CLOCK_EnableClock(kCLOCK_PortC);

	// set pin mux to port c
	PORT_SetPinMux(PORTC, configLCD->rstPin, kPORT_MuxAsGpio); // RESET
	PORT_SetPinMux(PORTC, configLCD->kDcPin, kPORT_MuxAsGpio); // D~C

	// init the pins
	GPIO_PinInit(GPIOC, configLCD->rstPin, &gpio_output_config);  // rst pin
	GPIO_PinInit(GPIOC, configLCD->kDcPin, &gpio_output_config);  // DC pin
	//end pin config

	// SET the pins
	GPIO_PinWrite(GPIOD,configLCD->kCePin, FALSE);
	GPIO_PinWrite(GPIOC,configLCD->kDcPin, FALSE);
	GPIO_PinWrite(GPIOC,configLCD->rstPin, TRUE);

	// RESET THE LCD
	GPIO_PinWrite(GPIOC,configLCD->rstPin, FALSE);
	GPIO_PinWrite(GPIOC,configLCD->rstPin, TRUE);


	// send the commands
	uint8_t cmdToTransfer[6] = {EXTENDED_CMD,VOP_CONTRAST,TEMP_COF,LCD_BIAS,FUNCT_SET,NORMAL_MODE};
	GPIO_PinWrite(GPIOC,configLCD->kDcPin, FALSE);
    //SPI_SendData(cmdToTransfer, 6);
    rtosSPI_send(0, cmdToTransfer,6);
    return 0;
}
// configure the LCD
void setContrast(uint8_t contrast){

	GPIO_PinWrite(GPIOC,configLCD->kDcPin,0); // is command
	uint8_t contrastConfig[3] = {EXTENDED_CMD, (0x80 | contrast), FUNCT_SET};
	rtosSPI_send(0, contrastConfig,3);
    //SPI_SendData(contrastConfig, 3);
}

// draw in display
uint8_t draw(uint8_t *toDraw, uint32_t size){
	GPIO_PinWrite(GPIOC,configLCD->kDcPin, TRUE); //is data
	rtosSPI_send(0, toDraw,size);
	//SPI_SendData(toDraw, size);
	GPIO_PinWrite(GPIOC,configLCD->kDcPin, FALSE);
	return 0;
}

//set cursor
uint8_t setCursor(uint8_t x, uint8_t y){
    if(x >= MAX_WIDTH || y >= TOTAL_ROWS){
        return FALSE;
    }

    configLCD->cursorX = x;
    configLCD->cursorY = y;
    GPIO_PinWrite(GPIOC, configLCD->kDcPin, 0); // is command

    uint8_t xCommand = X_ADDR | x;
    uint8_t yCommand = Y_ADDR | y;

    rtosSPI_send(0, &xCommand,1);
    rtosSPI_send(0, &yCommand,1);
    //SPI_SendData(&xCommand, 1); // rows
   // SPI_SendData(&yCommand, 1); // column

    return TRUE;
}

LCD5110_t* LCD5110_make(uint8_t cursorX, uint8_t cursorY){

	static LCD5110_t configLCD;

	configLCD.cursorX = cursorX;
	configLCD.cursorY = cursorY;

	return &configLCD;
}

void clearLCD() {
    GPIO_PinWrite(GPIOC, configLCD->kDcPin, FALSE);
    for (uint8_t y = 0; y < TOTAL_ROWS; y++) {
        setCursor(0, y);
        GPIO_PinWrite(GPIOC, configLCD->kDcPin, 1);
        for (uint8_t x = 0; x < MAX_WIDTH; x++) {
            uint8_t empty = 0x00;
            //SPI_SendData(&empty, 1);
            rtosSPI_send(0, &empty,1);
        }
    }
    setCursor(0, 0);
}

void clearLCDLine(uint8_t line) {
    if (line < 48) {
        GPIO_PinWrite(GPIOC, configLCD->kDcPin, FALSE);
        setCursor(0, line);
        GPIO_PinWrite(GPIOC, configLCD->kDcPin, 1);
        for (uint8_t x = 0; x < 84; x++) {
            uint8_t empty = 0x00;
            //SPI_SendData(&empty, 1);
            rtosSPI_send(0, &empty, 1);
        }
    }
}
void setPixel(uint8_t x, uint8_t y)
{
    if ((x >= 0) && (x < MAX_WIDTH) && (y >= 0) && (y < MAX_HEIGHT))
    {
        uint8_t shift = y % 8;
        uint8_t index = x + (y / 8) * MAX_WIDTH;
        displayMap[index] |= 1 << shift;

        setCursor(x, y / 8);
        draw(&displayMap[index], 1);
    }
}

void setChar(char character, int x, int y)
{
	setCursor(x, y / 8);
    for (int i = 0; i < 5; i++) {
        draw(&arrayDefault[character-0x20][i],1);
    }
}

void setStr(char * dString, int x, int y)
{
	while (*dString != 0x00)
	{
		setChar(*dString++, x, y);
		x+=5;
		if (x > (MAX_WIDTH - 5))
		{
			x = 0;
			y += 8;
		}
	}
}


void print_clock_lcd(clock clock_lcd, int x, int y) {
    char timeString[9];

    timeString[0] = (clock_lcd.hrs / 10) + 48U;
    timeString[1] = (clock_lcd.hrs % 10) + 48U;
    timeString[2] = ':';
    timeString[3] = (clock_lcd.min / 10) + 48U;
    timeString[4] = (clock_lcd.min % 10) + 48U;
    timeString[5] = ':';
    timeString[6] = (clock_lcd.seg / 10) + 48U;
    timeString[7] = (clock_lcd.seg % 10) + 48U;
    timeString[8] = '\0';

    setStr(timeString, x, y);
}

void printImage(uint8_t* image)
{
	for (uint8_t  z = 0; z < MAX_HEIGHT / 8; z++)
	{
		setCursor(0, z);
		draw(&image[z * MAX_WIDTH], MAX_WIDTH);
	}
}
