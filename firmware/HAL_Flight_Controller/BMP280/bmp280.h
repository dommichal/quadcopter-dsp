#if !defined(BMP280)
#define BMP280
#include <main.h>
#include <stdbool.h>

/**
 * Mode of BMP280 module operation.
 * Forced - Measurement is initiated by user.
 * Normal - Continues measurement.
 */
typedef enum {
    BMP280_MODE_SLEEP = 0,
    BMP280_MODE_FORCED = 1,
    BMP280_MODE_NORMAL = 3
} BMP280_Mode;

typedef enum {
    BMP280_FILTER_OFF = 0,
    BMP280_FILTER_2 = 1,
    BMP280_FILTER_4 = 2,
    BMP280_FILTER_8 = 3,
    BMP280_FILTER_16 = 4
} BMP280_Filter;

/**
 * Pressure oversampling settings
 */
typedef enum {
    BMP280_SKIPPED = 0,          /* no measurement  */
    BMP280_ULTRA_LOW_POWER = 1,  /* oversampling x1 */
    BMP280_LOW_POWER = 2,        /* oversampling x2 */
    BMP280_STANDARD = 3,         /* oversampling x4 */
    BMP280_HIGH_RES = 4,         /* oversampling x8 */
    BMP280_ULTRA_HIGH_RES = 5    /* oversampling x16 */
} BMP280_Oversampling;

/**
 * Stand by time between measurements in normal mode
 */
typedef enum {
    BMP280_STANDBY_05 = 0,      /* stand by time 0.5ms */
    BMP280_STANDBY_62 = 1,      /* stand by time 62.5ms */
    BMP280_STANDBY_125 = 2,     /* stand by time 125ms */
    BMP280_STANDBY_250 = 3,     /* stand by time 250ms */
    BMP280_STANDBY_500 = 4,     /* stand by time 500ms */
    BMP280_STANDBY_1000 = 5,    /* stand by time 1s */
    BMP280_STANDBY_2000 = 6,    /* stand by time 2s BMP280, 10ms BME280 */
    BMP280_STANDBY_4000 = 7,    /* stand by time 4s BMP280, 20ms BME280 */
} BMP280_StandbyTime;


typedef struct
{
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
    int32_t t_fine;
} bmp280_calib_param;

typedef struct 
{
    BMP280_Mode mode;
    BMP280_Filter filter;
    BMP280_StandbyTime standby_time;
    BMP280_Oversampling oversampling_pressure;
    BMP280_Oversampling oversampling_temperature;
} bmp280_config;

typedef struct 
{
    float temp;
    float press;
    bool defoult_conf;
    bmp280_calib_param calib_param;
    bmp280_config conf;

    bool bmp280_busy;
    uint8_t data[6]; 
    I2C_HandleTypeDef *hi2c;
} bmp280_dev;

#define BMP280_I2C_ADDR        0x77
#define BMP280_CHIP_ID_1       0x56
#define BMP280_CHIP_ID_2       0x57
#define BMP280_CHIP_ID_3       0x58

//-------------BMP280 registers--------------------
#define BMP280_REG_TEMP_XLSB   0xFC /* bits: 7-4 */
#define BMP280_REG_TEMP_LSB    0xFB
#define BMP280_REG_TEMP_MSB    0xFA
#define BMP280_REG_TEMP        (BMP280_REG_TEMP_MSB)
#define BMP280_REG_PRESS_XLSB  0xF9 /* bits: 7-4 */
#define BMP280_REG_PRESS_LSB   0xF8
#define BMP280_REG_PRESS_MSB   0xF7
#define BMP280_REG_PRESSURE    (BMP280_REG_PRESS_MSB)
#define BMP280_REG_CONFIG      0xF5 /* bits: 7-5 t_sb; 4-2 filter; 0 spi3w_en */
#define BMP280_REG_CTRL        0xF4 /* bits: 7-5 osrs_t; 4-2 osrs_p; 1-0 mode */
#define BMP280_REG_STATUS      0xF3 /* bits: 3 measuring; 0 im_update */
#define BMP280_REG_CTRL_HUM    0xF2 /* bits: 2-0 osrs_h; */
#define BMP280_REG_RESET       0xE0
#define BMP280_REG_ID          0xD0

/*Calibration parameter register addresses*/
#define BMP280_DIG_T1_LSB_ADDR               0x88
#define BMP280_DIG_T1_MSB_ADDR               0x89
#define BMP280_DIG_T2_LSB_ADDR               0x8A
#define BMP280_DIG_T2_MSB_ADDR               0x8B
#define BMP280_DIG_T3_LSB_ADDR               0x8C
#define BMP280_DIG_T3_MSB_ADDR               0x8D
#define BMP280_DIG_P1_LSB_ADDR               0x8E
#define BMP280_DIG_P1_MSB_ADDR               0x8F
#define BMP280_DIG_P2_LSB_ADDR               0x90
#define BMP280_DIG_P2_MSB_ADDR               0x91
#define BMP280_DIG_P3_LSB_ADDR               0x92
#define BMP280_DIG_P3_MSB_ADDR               0x93
#define BMP280_DIG_P4_LSB_ADDR               0x94
#define BMP280_DIG_P4_MSB_ADDR               0x95
#define BMP280_DIG_P5_LSB_ADDR               0x96
#define BMP280_DIG_P5_MSB_ADDR               0x97
#define BMP280_DIG_P6_LSB_ADDR               0x98
#define BMP280_DIG_P6_MSB_ADDR               0x99
#define BMP280_DIG_P7_LSB_ADDR               0x9A
#define BMP280_DIG_P7_MSB_ADDR               0x9B
#define BMP280_DIG_P8_LSB_ADDR               0x9C
#define BMP280_DIG_P8_MSB_ADDR               0x9D
#define BMP280_DIG_P9_LSB_ADDR               0x9E
#define BMP280_DIG_P9_MSB_ADDR               0x9F

#define BMP280_RESET_VALUE     0xB6


HAL_StatusTypeDef bmp280_write(bmp280_dev *dev, uint8_t reg_addr, uint8_t data);

HAL_StatusTypeDef bmp280_read(bmp280_dev *dev, uint8_t reg_addr, uint8_t *data, size_t data_len);

HAL_StatusTypeDef bmp280_init(bmp280_dev *dev);

HAL_StatusTypeDef bmp280_burst_read(bmp280_dev *dev);

HAL_StatusTypeDef bmp280_burst_read_DMA(bmp280_dev *dev);

void bmp280_burst_read_DMA_complete(bmp280_dev *dev);

#endif // BMP280

