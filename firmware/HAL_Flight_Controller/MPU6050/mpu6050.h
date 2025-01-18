#if !defined(mpu6050)
#define mpu6050

#include <stdbool.h>
#include "gpio.h"
#include "i2c.h"

#define FLOAT_TYPE float

// This macro is defined, because the outputs from MPU are reverse of what the rotation matrix expects
// If you want have output with the same sign as MPU output, define this macro to `false`
#define REVERSE_GYRO true

// remaps the output axes from the MPU to be aligned as expected in the paper
// to disable the remapping define this macro to `0`
#define GYRO_VARIANT 1 


typedef struct
{
    I2C_HandleTypeDef *hi2c;
    FLOAT_TYPE* mpu_acc_buff;
    FLOAT_TYPE* mpu_gyro_buff;
    bool acc_busy;
    bool gyro_busy;
} MPU6050_STRUCT;

typedef enum acc_range_t {
  AFS_2g  = 0x0U,
  AFS_4g  = 0x1U,
  AFS_8g  = 0x2U,
  AFS_16g = 0x3U
} acc_range_t;

typedef enum gyro_range_t {
  FS_250dps  = 0x0U,
  FS_500dps  = 0x1U,
  FS_1000dps = 0x2U,
  FS_2000dps = 0x3U
} gyro_range_t;

typedef enum mpu_clk_src_t {
  MPU_CLK_internal = 0,
  MPU_CLK_PLL_X = 1,
  MPU_CLK_PLL_Y = 2,
  MPU_CLK_PLL_Z = 3,
  MPU_CLK_external_32kHz = 4,
  MPU_CLK_external_19MHz = 5,
  MPU_CLK_STOP = 7
} mpu_clk_src_t;

typedef enum mpu_int_level_t {
  MPU_int_active_high = 0U,
  MPU_int_active_low = 1U
} mpu_int_level_t;

typedef enum mpu_dlpf_cfg_t {
  BAND_260HZ = 0U,
  BAND_184HZ = 1U,
  BAND_94Hz  = 2U,
  BAND_44HZ  = 3U,
  BAND_21HZ  = 4U,
  BAND_10HZ  = 5U,
  BAND_5HZ   = 6U
} mpu_dlpf_cfg_t;

// TODO: add self test values to struct
typedef struct MPU6050_config {
  uint8_t sample_rate_divider;
  gyro_range_t fs_sel;
  acc_range_t  afs_sel;
  // for CONFIG_REG
  uint8_t ext_sync_set;
  mpu_dlpf_cfg_t dlpf_cfg;
  // For INT_ENABLE
  bool data_rdy_en;
  bool i2c_mst_int_en;
  bool fifo_oflow_en;
  // For INT_PIN_CFG
  mpu_int_level_t int_level;
  bool int_open;       
  bool latch_int;   
  bool int_rd_clear;   
  bool fsync_int_level;
  bool fsync_int_en;   
  bool i2c_bypass_en;  
  // PWR_MGMT_1
  bool device_reset;
  bool sleep;
  bool cycle;
  bool temp_dis;
  mpu_clk_src_t clksel;
  // USER_CTRL
  bool fifo_en;
  bool i2c_mst_en;
  bool i2c_if_dis;
  bool fifo_reset;
  bool i2c_mst_reset;
  bool sig_cond_reset;
} MPU6050_config;

#define AD0 0
#define MPU6050_ADDR	(0x68 | (AD0))


/////////////////////////////////////
/////////// REGISTER MAP ////////////
/////////////////////////////////////

// Self test registers
#define SELF_TEST_X 0x0D
#define SELF_TEST_Y 0x0E
#define SELF_TEST_Z 0x0F
#define SELF_TEST_A 0x10

// Sample rate divider
#define SMPLRT_DIV  0x19

#define CONFIG_REG  0x1A

/**
  * @brief
  * Address of gyroscope config register.
  * Bit 7: XG_ST
  * Bit 6: YG_ST
  * Bit 5: ZG_ST
  * Bits 4-3: FS_SEL[1:0]
*/
#define GYRO_CONFIG_REG 0x1B
/**
  * @brief
  * Address of accelerometer config register.
  * Bit 7: XA_ST
  * Bit 6: YA_ST
  * Bit 5: ZA_ST
  * Bits 4-3: AFS_SEL[1:0]
*/
#define ACC_CONFIG_REG 0x1C
#define FIFO_EN        0x23

#define I2C_MST_CTRL   0x24

#define I2C_SLV0_ADDR  0x25
#define I2C_SLV0_REG   0x26
#define I2C_SLV0_CTRL  0x27

#define I2C_SLV1_ADDR  0x28
#define I2C_SLV1_REG   0x29
#define I2C_SLV1_CTRL  0x2a

#define I2C_SLV2_ADDR  0x2b
#define I2C_SLV2_REG   0x2c
#define I2C_SLV2_CTRL  0x2d

#define I2C_SLV3_ADDR  0x2e
#define I2C_SLV3_REG   0x2f
#define I2C_SLV3_CTRL  0x30

#define I2C_SLV4_ADDR  0x31
#define I2C_SLV4_REG   0x32
#define I2C_SLV4_DO    0x33
#define I2C_SLV4_CTRL  0x34
#define I2C_SLV4_DI    0x35

#define I2C_MST_STATUS 0x36
#define INT_PIN_CFG    0x37
#define INT_LEVEL_MASK        7
#define INT_OPEN_MASK         6
#define LATCH_INT_EN_MASK     5
#define INT_RD_CLEAR_MASK     4
#define FSYNC_INT_LEVEL_MASK  3
#define FSYNC_INT_EN_MASK     2
#define I2C_BYPASS_EN_MASK    1


#define INT_ENABLE     0x38
#define FIFO_OVLOW_EN_MASK  4
#define I2C_MST_INT_EN_MASK 3
#define DATA_RDY_EN_MASK    0
#define INT_STATUS     0x3A
/**
  * @brief
  * Address of the first register with accelerometer data.
  * The values are layed out in the order of X, Y, Z, with
  * 16 bits per axix in Big Endian (MSB first).
*/
#define ACC_REG_START 0x3B
#define ACCEL_XOUT_H  0x3B
#define ACCEL_XOUT_L  0x3C
#define ACCEL_YOUT_H  0x3D
#define ACCEL_YOUT_L  0x3E
#define ACCEL_ZOUT_H  0x3F
#define ACCEL_ZOUT_L  0x40

#define TEMP_OUT_H    0x41
#define TEMP_OUT_L    0x42

/**
  * @brief
  * Address of the first register with gyroscope data.
  * The values are layed out in the order of X, Y, Z, with
  * 16 bits per axix in Big Endian (MSB first).
*/
#define GYRO_REG_START 0x43
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48

#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4a
#define EXT_SENS_DATA_02 0x4b
#define EXT_SENS_DATA_03 0x4c
#define EXT_SENS_DATA_04 0x4d
#define EXT_SENS_DATA_05 0x4e
#define EXT_SENS_DATA_06 0x4f
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5a
#define EXT_SENS_DATA_18 0x5b
#define EXT_SENS_DATA_19 0x5c
#define EXT_SENS_DATA_20 0x5d
#define EXT_SENS_DATA_21 0x5e
#define EXT_SENS_DATA_22 0x5f
#define EXT_SENS_DATA_23 0x60

#define I2C_SLV0_DO      0x63
#define I2C_SLV1_DO      0x64
#define I2C_SLV2_DO      0x65
#define I2C_SLV3_DO      0x66

#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define USER_CTRL          0x6A
#define FIFO_EN_MASK          6
#define I2C_MST_EN_MASK       5
#define I2C_IF_DIS_MASK       4
#define FIFO_RESET_MASK       2
#define I2C_MST_RESET_MASK    1
#define SIG_COND_RESET_MASK   0


#define PWR_MGMT_1 0x6B
#define DEVICE_RESET_MASK 7
#define SLEEP_MASK        6
#define CYCLE_MASK        5
#define TEMP_DIS_MASK     3
#define CLKSEL_MASK       0

#define PWR_MGMT_2 0x6C

#define FIFO_COUNT 0x72
#define FIFO_R_W   0x74
#define WHO_AM_I   0x75


#define I2C_BYPASS_EN 1
#define I2C_MST_EN 5


HAL_StatusTypeDef mpu6050_read_byte(MPU6050_STRUCT *mpu, uint8_t addr, uint8_t *data);
HAL_StatusTypeDef MPU_init(MPU6050_STRUCT *mpu, MPU6050_config* cfg);
HAL_StatusTypeDef MPU_clear_int(MPU6050_STRUCT *mpu);
HAL_StatusTypeDef MPU_read_acc(MPU6050_STRUCT *mpu, FLOAT_TYPE output[]);
HAL_StatusTypeDef MPU_read_acc_DMA(MPU6050_STRUCT *mpu);
HAL_StatusTypeDef MPU_read_acc_DMA_complete(MPU6050_STRUCT *mpu);
HAL_StatusTypeDef MPU_read_gyro(MPU6050_STRUCT *mpu, FLOAT_TYPE output[]);
HAL_StatusTypeDef MPU_read_gyro_DMA(MPU6050_STRUCT *mpu);
HAL_StatusTypeDef MPU_read_gyro_DMA_complete(MPU6050_STRUCT *mpu);
HAL_StatusTypeDef MPU_set_acc_resolution(MPU6050_STRUCT *mpu, acc_range_t range);
HAL_StatusTypeDef MPU_set_gyro_resolution(MPU6050_STRUCT *mpu, gyro_range_t range);
HAL_StatusTypeDef MPU_read_acc_gyro_DMA(MPU6050_STRUCT *mpu);
HAL_StatusTypeDef MPU_read_acc_gyro_DMA_complete(MPU6050_STRUCT *mpu);
HAL_StatusTypeDef MPU_measure_gyro_offset(MPU6050_STRUCT* mpu, uint16_t samples);
HAL_StatusTypeDef MPU_measure_acc_offset(MPU6050_STRUCT* mpu, uint16_t samples) ;
// todo: add a function for sampling rate
MPU6050_config MPU_get_default_cfg(void);

// todo: add self test
// todo: add functions for calibrating acc and gyro


#endif // mpu6050
