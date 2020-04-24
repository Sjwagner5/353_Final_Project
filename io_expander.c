#include "io_expander.h"

//*****************************************************************************
// Write 1 byte of data to the IO expander
//*****************************************************************************
void io_expander_write_reg(uint8_t reg, uint8_t data) {
	
	// Before doing anything, make sure the I2C device is idle
	while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
		
	//set the slave address of the io expander
	i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_WRITE);
	
	// Send the register address
	i2cSendByte(IO_EXPANDER_I2C_BASE, reg, I2C_MCS_RUN | I2C_MCS_START);
	
  // Send the Byte of data to write
	i2cSendByte(IO_EXPANDER_I2C_BASE, data, I2C_MCS_RUN | I2C_MCS_STOP);
}

//*****************************************************************************
// Read 1 byte from the IO Expander
//*****************************************************************************
uint8_t io_expander_read_reg(uint8_t reg) {
	
	uint8_t data;
	i2c_status_t status;
	
	// Before doing anything, make sure the I2C device is idle
	while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
		
	//set the slave address of the io expander
	status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_WRITE);
	if ( status != I2C_OK ) {
		return 0x00;
	}
	
	//send the register to read from
	status = i2cSendByte(IO_EXPANDER_I2C_BASE, reg, I2C_MCS_START | I2C_MCS_RUN);
	if (status != I2C_OK) {
		return 0x00;
	}
	
	// Set the I2C slave address to be the EEPROM and in Read Mode
	status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_READ);
	if (status != I2C_OK) {
		return 0x00;
	}
	
	// Read the data
	status = i2cGetByte(IO_EXPANDER_I2C_BASE, &data, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
	if (status != I2C_OK) {
		return 0x00;
	}	
	
  return data;
}

//*****************************************************************************
// Initialize the I2C peripheral
//*****************************************************************************
bool io_expander_init(void) {
	
	//Configure I2C GPIO Pins
	if (gpio_enable_port(IO_EXPANDER_GPIO_BASE) == false) {
		return false;
	}
	
	//config scl pin
	if (gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN) == false) {
		return false;
	}
	if (gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN) == false) {
		return false;
	}
	if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PCTL_M, IO_EXPANDER_I2C_SCL_PIN_PCTL) == false){
    return false;
  }
	
	//config sda pin
	if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_open_drain(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PCTL_M, IO_EXPANDER_I2C_SDA_PIN_PCTL)== false)
  {
    return false;
  }
	
	//  Initialize the I2C peripheral
  if( initializeI2CMaster(IO_EXPANDER_I2C_BASE)!= I2C_OK)
  {
    return false;
  }
  
  return true;
}
