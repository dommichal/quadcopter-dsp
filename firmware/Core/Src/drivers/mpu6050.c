/**
 * @file mpu6050.c
 * @author Kacper Filipek
 * @brief 
 * @version 0.1
 * @date 2024-01-26
 */
#include <drivers/mpu6050.h>

#define HAL_error_check(status) if(status != HAL_OK) { return status; }

static FLOAT_TYPE ACC_SCALE_FACTOR = 0.0;
static FLOAT_TYPE GYRO_SCALE_FACTOR = 0.0;

static uint8_t mpu_acc_buf_raw[6];
static uint8_t mpu_gyro_buf_raw[6];
static uint8_t mpu_acc_gyro_buf_raw[14];
static FLOAT_TYPE* mpu_acc_buffer;
static FLOAT_TYPE* mpu_gyro_buffer;
static FLOAT_TYPE mpu_gyro_offset[3] = {0.0, 0.0, 0.0};
static FLOAT_TYPE mpu_acc_offset[3] = {0.0, 0.0, 0.0};

extern bool mpu_acc_read_available;
extern bool mpu_gyro_read_available;

MPU6050_config default_cfg = {
    .sample_rate_divider = 0,
    .ext_sync_set = 0,
    .int_level = MPU_int_active_low,
    .latch_int = true,
    .dlpf_cfg = BAND_94Hz,
    .data_rdy_en = true,
    .i2c_mst_int_en = false,
    .fifo_oflow_en = false,
    .fs_sel = FS_500dps,
    .afs_sel = AFS_4g,
    .i2c_bypass_en = true,
    // pwr mgmt
    .device_reset = false,
    .sleep        = false,
    .cycle        = false,
    .temp_dis     = false,
    .clksel       = MPU_CLK_internal,
    // User CTRL
    .fifo_en        = false,
    .i2c_mst_en     = false,
    .i2c_if_dis     = false,
    .fifo_reset     = false,
    .i2c_mst_reset  = false,
    .sig_cond_reset = false
};

MPU6050_config MPU_get_default_cfg(void) {
    return default_cfg;
}

HAL_StatusTypeDef mpu6050_read_byte(MPU6050_STRUCT *mpu, uint8_t addr, uint8_t *data)
{
    return HAL_I2C_Mem_Read(mpu->hi2c, MPU6050_ADDR<<1, addr, 1, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MPU_init(MPU6050_STRUCT *mpu, MPU6050_config* cfg) {
    HAL_StatusTypeDef status = HAL_OK;

    mpu_acc_buffer = mpu->mpu_acc_buff;
    mpu_gyro_buffer = mpu->mpu_gyro_buff;

    mpu->acc_busy = false;
    mpu->gyro_busy = false;
    
    if(cfg == NULL) {
        return HAL_ERROR;
    }

    uint8_t i2c_buffer[2];


    *i2c_buffer = cfg->sample_rate_divider;
    status = HAL_I2C_Mem_Write(mpu->hi2c, MPU6050_ADDR << 1, 
                            SMPLRT_DIV, 1,
                            i2c_buffer, 1,
                            HAL_MAX_DELAY);
    HAL_error_check(status);

    *i2c_buffer = (cfg->device_reset << DEVICE_RESET_MASK) |
                  (cfg->sleep        << SLEEP_MASK)        |
                  (cfg->cycle        << CYCLE_MASK)        |
                  (cfg->temp_dis     << TEMP_DIS_MASK)     |
                  (cfg->clksel       << CLKSEL_MASK);
    status = HAL_I2C_Mem_Write(mpu->hi2c, MPU6050_ADDR << 1, 
                            PWR_MGMT_1, 1,
                            i2c_buffer, 1,
                            HAL_MAX_DELAY);
    HAL_error_check(status);

    *i2c_buffer = (cfg->ext_sync_set << 3) | (cfg->dlpf_cfg);
    status = HAL_I2C_Mem_Write(mpu->hi2c, MPU6050_ADDR << 1, 
                            CONFIG_REG, 1,
                            i2c_buffer, 1,
                            HAL_MAX_DELAY);
    HAL_error_check(status);

    status = MPU_set_acc_resolution(mpu, cfg->afs_sel);
    status = MPU_set_gyro_resolution(mpu, cfg->fs_sel);

    *i2c_buffer = (cfg->data_rdy_en    << DATA_RDY_EN_MASK)    |
                  (cfg->i2c_mst_int_en << I2C_MST_INT_EN_MASK) |
                  (cfg->fifo_oflow_en  << FIFO_OVLOW_EN_MASK);
    status = HAL_I2C_Mem_Write(mpu->hi2c, MPU6050_ADDR << 1, 
                            INT_ENABLE, 1,
                            i2c_buffer, 1,
                            HAL_MAX_DELAY);
    HAL_error_check(status);

    *i2c_buffer = (cfg->int_level       << INT_LEVEL_MASK)        |
                  (cfg->int_open        << INT_OPEN_MASK)         |
                  (cfg->latch_int       << LATCH_INT_EN_MASK)     |
                  (cfg->int_rd_clear    << INT_RD_CLEAR_MASK)     |
                  (cfg->fsync_int_level << FSYNC_INT_LEVEL_MASK)  |
                  (cfg->fsync_int_en    << FSYNC_INT_EN_MASK)     |
                  (cfg->i2c_bypass_en   << I2C_BYPASS_EN_MASK);
    status = HAL_I2C_Mem_Write(mpu->hi2c, MPU6050_ADDR << 1, 
                            INT_PIN_CFG, 1,
                            i2c_buffer, 1,
                            HAL_MAX_DELAY);
    HAL_error_check(status);

    *i2c_buffer = (cfg->fifo_en        <<  FIFO_EN_MASK)       |
                  (cfg->i2c_mst_en     <<  I2C_MST_EN_MASK)    |
                  (cfg->i2c_if_dis     <<  I2C_IF_DIS_MASK)    |
                  (cfg->fifo_reset     <<  FIFO_RESET_MASK)    |
                  (cfg->i2c_mst_reset  <<  I2C_MST_RESET_MASK) |
                  (cfg->sig_cond_reset <<  SIG_COND_RESET_MASK);


    status = HAL_I2C_Mem_Write(mpu->hi2c, MPU6050_ADDR << 1, 
                            USER_CTRL, 1,
                            i2c_buffer, 1,
                            HAL_MAX_DELAY);
    HAL_error_check(status);
    

    return HAL_OK;
}

HAL_StatusTypeDef MPU_measure_gyro_offset(MPU6050_STRUCT* mpu, uint16_t samples) {
    FLOAT_TYPE gyro_data[3];
    HAL_StatusTypeDef status = HAL_OK;

    for(uint16_t i = 0; i < samples; i++) {
        status = MPU_read_gyro(mpu, gyro_data);

        mpu_gyro_offset[0] += gyro_data[0] / samples;
        mpu_gyro_offset[1] += gyro_data[1] / samples;
        mpu_gyro_offset[2] += gyro_data[2] / samples;
    }
    return status;
}

HAL_StatusTypeDef MPU_measure_acc_offset(MPU6050_STRUCT* mpu, uint16_t samples) {
    FLOAT_TYPE acc_data[3];
    HAL_StatusTypeDef status = HAL_OK;

    for(uint16_t i = 0; i < samples; i++) {
        status = MPU_read_acc(mpu, acc_data);

        mpu_acc_offset[0] += acc_data[0] / samples;
        mpu_acc_offset[1] += acc_data[1] / samples;
        mpu_acc_offset[2] += (acc_data[2] - 1) / samples;
    }
    return status;
}

HAL_StatusTypeDef MPU_clear_int(MPU6050_STRUCT *mpu){

    uint8_t i2c_read_buffer[1];
    return HAL_I2C_Mem_Read(mpu->hi2c, MPU6050_ADDR << 1,
                            INT_STATUS, 1,
                            i2c_read_buffer, 1,
                            HAL_MAX_DELAY);

} 

HAL_StatusTypeDef MPU_read_acc(MPU6050_STRUCT *mpu, FLOAT_TYPE output[]) {
    HAL_StatusTypeDef status;
    uint8_t i2c_buffer[6];

    status = HAL_I2C_Mem_Read(mpu->hi2c, MPU6050_ADDR << 1,
                            ACC_REG_START, 1,
                            i2c_buffer, 6,
                            HAL_MAX_DELAY);
    HAL_error_check(status);

    for(uint8_t i = 0; i < 3; i++) {
        output[i] = ACC_SCALE_FACTOR * (int16_t)((i2c_buffer[2 * i] << 8) | i2c_buffer[2 * i + 1]);
    }

    return HAL_OK;
}

HAL_StatusTypeDef MPU_read_gyro(MPU6050_STRUCT *mpu, FLOAT_TYPE output[]) {
    HAL_StatusTypeDef status;
    uint8_t i2c_buffer[6];

    status = HAL_I2C_Mem_Read(mpu->hi2c, MPU6050_ADDR << 1,
                            GYRO_REG_START, 1,
                            i2c_buffer, 6,
                            HAL_MAX_DELAY);
    HAL_error_check(status);

    for(uint8_t i = 0; i < 3; i++) {
        output[i] = GYRO_SCALE_FACTOR * (int16_t)((i2c_buffer[2 * i] << 8) | i2c_buffer[2 * i + 1]);
    }

    return HAL_OK;
}

HAL_StatusTypeDef MPU_set_acc_resolution(MPU6050_STRUCT *mpu, acc_range_t range) {
    HAL_StatusTypeDef status;

    uint8_t i2c_buffer[1] = {0};

    *i2c_buffer = (range << 3);
    status = HAL_I2C_Mem_Write(mpu->hi2c, MPU6050_ADDR << 1, 
                            ACC_CONFIG_REG, 1,
                            i2c_buffer, 1,
                            HAL_MAX_DELAY);
    if(status == HAL_OK) {
        ACC_SCALE_FACTOR = ((1 << range) / 16384.0);
    }

    return status;
}

HAL_StatusTypeDef MPU_set_gyro_resolution(MPU6050_STRUCT *mpu, gyro_range_t range) {
    HAL_StatusTypeDef status;

    uint8_t i2c_buffer[1] = {0};

    *i2c_buffer = (range << 3);
    status = HAL_I2C_Mem_Write(mpu->hi2c, MPU6050_ADDR << 1, 
                            GYRO_CONFIG_REG, 1,
                            i2c_buffer, 1,
                            HAL_MAX_DELAY);
    if(status == HAL_OK) {
        GYRO_SCALE_FACTOR = ((1 << range) / 131.0);
        
        // reverse gyro outputs
        #if REVERSE_GYRO
            GYRO_SCALE_FACTOR *= -1;
        #endif
    }

    return status;
}


HAL_StatusTypeDef MPU_read_acc_DMA(MPU6050_STRUCT *mpu) {
    mpu->acc_busy = true;
    return HAL_I2C_Mem_Read_DMA(mpu->hi2c, MPU6050_ADDR << 1,
                            ACC_REG_START, 1,
                            mpu_acc_buf_raw, 6);
}

HAL_StatusTypeDef MPU_read_acc_DMA_complete(MPU6050_STRUCT *mpu) {
    for(uint8_t i = 0; i < 3; i++) {
        mpu_acc_buffer[i] = ACC_SCALE_FACTOR * (int16_t)((mpu_acc_buf_raw[2 * i] << 8) | mpu_acc_buf_raw[2 * i + 1]);
    }
    mpu->acc_busy = false;
    return MPU_clear_int(mpu);
}

HAL_StatusTypeDef MPU_read_gyro_DMA(MPU6050_STRUCT *mpu) {
    mpu->gyro_busy = true;
    return HAL_I2C_Mem_Read_DMA(mpu->hi2c, MPU6050_ADDR << 1,
                            GYRO_REG_START, 1,
                            mpu_gyro_buf_raw, 6);
}

HAL_StatusTypeDef MPU_read_gyro_DMA_complete(MPU6050_STRUCT *mpu) {
    for(uint8_t i = 0; i < 3; i++) {
        mpu_gyro_buffer[i] = GYRO_SCALE_FACTOR 
                           * (int16_t)((mpu_gyro_buf_raw[2 * i] << 8) | mpu_gyro_buf_raw[2 * i + 1])
                           - mpu_gyro_offset[i];
    }
    mpu->gyro_busy = false;
    return MPU_clear_int(mpu);
}

HAL_StatusTypeDef MPU_read_acc_gyro_DMA(MPU6050_STRUCT *mpu) {
    mpu->gyro_busy = true;
    mpu->acc_busy = true;
    return HAL_I2C_Mem_Read_DMA(mpu->hi2c, MPU6050_ADDR << 1,
                            ACC_REG_START, 1,
                            mpu_acc_gyro_buf_raw, 14);
}

HAL_StatusTypeDef MPU_read_acc_gyro_DMA_complete(MPU6050_STRUCT *mpu) {
    for(uint8_t i = 0; i < 3; i++) {
        mpu_acc_buffer[i] = ACC_SCALE_FACTOR * (int16_t)((mpu_acc_gyro_buf_raw[2 * i] << 8) |
                                                          mpu_acc_gyro_buf_raw[2 * i + 1]) - mpu_acc_offset[i];
    }
    for(uint8_t i = 0; i < 3; i++) {
        mpu_gyro_buffer[i] = GYRO_SCALE_FACTOR * (int16_t)((mpu_acc_gyro_buf_raw[8 + 2 * i] << 8) |
                                                            mpu_acc_gyro_buf_raw[8 + 2 * i + 1]) - mpu_gyro_offset[i];
    }
    mpu->acc_busy = false;
    mpu->acc_busy = false;
    return MPU_clear_int(mpu);
}