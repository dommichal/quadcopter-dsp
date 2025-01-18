#include <drivers/bmp280.h>

void bmp280_init_default_params(bmp280_dev *dev) {
	dev->conf.mode = BMP280_MODE_NORMAL;
	dev->conf.filter = BMP280_FILTER_OFF;
	dev->conf.oversampling_pressure = BMP280_STANDARD;
	dev->conf.oversampling_temperature = BMP280_STANDARD;
	dev->conf.standby_time = BMP280_STANDBY_250;
}

HAL_StatusTypeDef bmp280_write(bmp280_dev *dev, uint8_t reg_addr, uint8_t data){
    return HAL_I2C_Mem_Write(dev->hi2c, BMP280_I2C_ADDR << 1, reg_addr, 1, &data,
                             1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef bmp280_read(bmp280_dev *dev, uint8_t reg_addr, uint8_t *data, size_t data_len){
	return HAL_I2C_Mem_Read(dev->hi2c, BMP280_I2C_ADDR << 1, reg_addr, 1, data, 
							data_len, HAL_MAX_DELAY);
}

void bmp280_read_calibration(bmp280_dev *dev){
    uint8_t data1, data0;

    bmp280_read(dev, BMP280_DIG_T1_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_T1_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_t1 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_T2_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_T2_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_t2 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_T3_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_T3_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_t3 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_P1_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_P1_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_p1 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_P2_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_P2_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_p2 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_P3_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_P3_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_p3 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_P4_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_P4_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_p4 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_P5_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_P5_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_p5 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_P6_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_P6_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_p6 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_P7_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_P7_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_p7 = (uint16_t) data0 << 8 | data1;
    
    bmp280_read(dev, BMP280_DIG_P8_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_P8_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_p8 = (uint16_t) data0 << 8 | data1;

    bmp280_read(dev, BMP280_DIG_P9_LSB_ADDR, &data1, 1);
    bmp280_read(dev, BMP280_DIG_P9_MSB_ADDR, &data0, 1);
    dev->calib_param.dig_p9 = (uint16_t) data0 << 8 | data1;

}

//Get fine_temp for pressure compensation
//Return value in degrees Celsius.
static inline int32_t compensate_temperature(bmp280_dev *dev, int32_t adc_temp,
		int32_t *fine_temp) {
	int32_t var1, var2;

	var1 = ((((adc_temp >> 3) - ((int32_t) dev->calib_param.dig_t1 << 1)))
			* (int32_t) dev->calib_param.dig_t2) >> 11;
	var2 = (((((adc_temp >> 4) - (int32_t) dev->calib_param.dig_t1)
			* ((adc_temp >> 4) - (int32_t) dev->calib_param.dig_t1)) >> 12)
			* (int32_t) dev->calib_param.dig_t3) >> 14;

	*fine_temp = var1 + var2;
	return (*fine_temp * 5 + 128) >> 8;
}

//Return value is in Pa, 24 integer bits and 8 fractional bits.
static inline uint32_t compensate_pressure(bmp280_dev *dev, int32_t adc_press,
		                                   int32_t fine_temp) 
{
	int64_t var1, var2, p;

	var1 = (int64_t) fine_temp - 128000;
	var2 = var1 * var1 * (int64_t) dev->calib_param.dig_p6;
	var2 = var2 + ((var1 * (int64_t) dev->calib_param.dig_p5) << 17);
	var2 = var2 + (((int64_t) dev->calib_param.dig_p4) << 35);
	var1 = ((var1 * var1 * (int64_t) dev->calib_param.dig_p3) >> 8)
			+ ((var1 * (int64_t) dev->calib_param.dig_p2) << 12);
	var1 = (((int64_t) 1 << 47) + var1) * ((int64_t) dev->calib_param.dig_p1) >> 33;

	if (var1 == 0) {
		return 0;  // avoid exception caused by division by zero
	}

	p = 1048576 - adc_press;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = ((int64_t) dev->calib_param.dig_p9 * (p >> 13) * (p >> 13)) >> 25;
	var2 = ((int64_t) dev->calib_param.dig_p8 * p) >> 19;

	p = ((p + var1 + var2) >> 8) + ((int64_t) dev->calib_param.dig_p7 << 4);
	return p;
}

HAL_StatusTypeDef bmp280_init(bmp280_dev *dev){
    uint8_t data;
    int ret;
    
    bmp280_read(dev, BMP280_REG_ID, &data, 1);
    if(!(data == BMP280_CHIP_ID_1 || data == BMP280_CHIP_ID_2 || data == BMP280_CHIP_ID_3)){
        return HAL_ERROR;
    }

    //Reset device
    ret = bmp280_write(dev, BMP280_REG_RESET, BMP280_RESET_VALUE);
    if(ret != HAL_OK){
        return HAL_ERROR;
    }

    //Wait for device reboot
    bmp280_read(dev, BMP280_REG_STATUS, &data, 1);
    if(data && 1 == 0){
        return HAL_ERROR;
    }

    //Read calibration register
    bmp280_read_calibration(dev);

    //note: set the default parameters if they are not set
    if(dev->defoult_conf == true){
        bmp280_init_default_params(dev);
    }

    //Set config register
    uint8_t conf = dev->conf.filter << 2 | dev->conf.standby_time << 5;
    ret = bmp280_write(dev, BMP280_REG_CONFIG, conf);
    if(ret != HAL_OK){
        return HAL_ERROR;
    }

    //Set control register
    uint8_t ctrl = dev->conf.mode | dev->conf.oversampling_pressure << 2
                   | dev->conf.oversampling_temperature << 5;
    ret = bmp280_write(dev, BMP280_REG_CTRL, ctrl);
    if(ret != HAL_OK){
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef bmp280_burst_read_raw(bmp280_dev *dev, int32_t *temp, uint32_t *press){
    //The BMP280 does not have a humidity reading,
    //so we need to read 6 bytes of data.
    int ret;
    uint8_t data[6];
    ret = bmp280_read(dev, BMP280_REG_PRESS_MSB, data, 6);
    if(ret != HAL_OK){
        return HAL_ERROR;
    }

    *press = data[0] << 12 | data[1] << 4 | data[2] >> 4;
	*temp = data[3] << 12 | data[4] << 4 | data[5] >> 4;

    int32_t fine_temp;
	*temp = compensate_temperature(dev, *temp, &fine_temp);
	*press = compensate_pressure(dev, *press, fine_temp);

    return HAL_OK;
}

HAL_StatusTypeDef bmp280_burst_read(bmp280_dev *dev){
    int32_t temp;
    uint32_t press;
    int ret = bmp280_burst_read_raw(dev, &temp, &press);
    dev->temp = (float) temp / 100;
    dev->press = (float) press / 256;
    return ret;
}


HAL_StatusTypeDef bmp280_burst_read_DMA(bmp280_dev *dev){
   dev->bmp280_busy = true;
   return HAL_I2C_Mem_Read_DMA(dev->hi2c, BMP280_I2C_ADDR, BMP280_REG_PRESS_MSB, 1,
                                dev->data, 6); 
}   

void bmp280_burst_read_DMA_complete(bmp280_dev *dev){
    dev->bmp280_busy = false;
    int32_t temp; 
    uint32_t press;

    press = dev->data[0] << 12 | dev->data[1] << 4 | dev->data[2] >> 4;
	temp = dev->data[3] << 12 | dev->data[4] << 4 | dev->data[5] >> 4;

    int32_t fine_temp;
	temp = compensate_temperature(dev, temp, &fine_temp);
	press = compensate_pressure(dev, press, fine_temp);

    dev->temp = (float) temp / 100;
    dev->press = (float) press / 256;
}