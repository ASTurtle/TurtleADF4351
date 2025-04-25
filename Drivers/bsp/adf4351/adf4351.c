/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * adf4351.c  ADF4350/ADF4351 SPI PLL driver
 * uses ADI's ADF4351 datasheet as reference. 
 * uses ADI's  macros in "adf4351.h" to set the registers.
 * Powered by  UESTC_Auto-Centor4Makers Turtle 
 * 2025/4/18
 */

#include "adf4351.h"

// static struct adf4350_platform_data default_pdata = {
// 	.channel_spacing = 10000,
// 	.r2_user_settings = ADF4350_REG2_PD_POLARITY_POS |
// 			    ADF4350_REG2_CHARGE_PUMP_CURR_uA(2500),
// 	.r3_user_settings = ADF4350_REG3_12BIT_CLKDIV_MODE(0),
// 	.r4_user_settings = ADF4350_REG4_OUTPUT_PWR(3) |
// 			    ADF4350_REG4_MUTE_TILL_LOCK_EN,
// };

static struct adf4351_settings default_settings = {
    .integer_value = 90 ,
    .fractional_value = 13,
    .mod_value = 25,
    .phase_adjust = 0,
    .phase_value = 1,
    .prescaler = 0, // 0: 4/5, 1: 8/9
    .counter_reset_en = 0,
    .charge_pump_3state_en = 0,
    .power_down = 0,
    .pd_polarity = 1,
    .ldp = 1, // 0: 6ns, 1: 10ns
    .ldf = 0, // 0: Fractional, 1: Integer
    .charge_pump_current_uA = 2500, // Charge pump current in uA
    .r_counter = 1, // 10-bit R counter value (1-1023)
    .r_div2 = 1, // 0: No divide by 2, 1: Divide by 2
    .r_mult2 = 0, // 0: No multiply by 2, 1: Multiply by 2
    .muxout = ADF4350_MUXOUT_DVDD, // MUXOUT setting
    .noise_mode = 0, // 0: Low noise, 1: Reserved, 2: Reserved, 3: Low spurious
    .double_buffer_r4 = 0, // 0: No double buffer, 1: Double buffer
    .clk_divider = 30, // 12-bit clock divider value (0-4095) for ADF4351_REG3_CLKDIV
    .clk_divider_mode = 2, // 0,1,2
    .cycle_slip_reduction = 1, // 0: Disabled, 1: Enabled
    .charge_cancellation = 0, // 0: Disabled, 1: Enabled
    .band_sel_clock_mode = 1, // 0: Low, 1: High
    .anti_backlash = 0, // 0: 6ns(FRAC-N), 1: 3ns(INT-N)

    .output_power = 3, // 0: -4dBm, 1: -1dBm, 2: +2dBm, 3: +5dBm
    .aux_output_power = 3, // 0: -4dBm, 1: -1dBm, 2: +2dBm, 3: +5dBm
    .rf_output_enable = 1, // 0: Disable, 1: Enable
    .aux_output_enable = 1, // 0: Disable, 1: Enable
    .aux_output_mode = 1, // 0: Divided, 1: Fundamental
    .mute_till_lock = 1, // 0: Unmute, 1: Mute
    .band_sel_clk_div = 2, // 8-bit band select clock divider value (0-255)
    .rf_divider = 0, // RF divider value (0-6) 0: 1, 1: 2, 2: 4, 3: 8, 4: 16, 5: 32, 6: 64,
    .feedback_divided = 0, // Feedback divided  (0-1) 1: divided, 0: fundamental
    .ld_pin_mode = 1, //  LD pin mode (0-3)   0: Low, 1: Digital LockDetect, 2: Low, 3: High

};

struct adf4351_platform_data myADF4351;
struct adf4351_settings myADF4351_settings;




/**
  * @brief  converts the ADF4351 settings to register values
  *
  * @note   This function converts the ADF4351 settings structure to register values.
  *
  * @param  settings specifies the ADF4351 settings structure.
  *         This parameter is a pointer to the adf4351_settings structure.
  *         
  * @param  reg specifies the register values to be set.
  *         This parameter is a pointer to an array of 6 uint32_t values.
  * @retval None
  */
void ADF4351_setting2reg(struct adf4351_settings *settings, uint32_t *reg){
    // Convert the settings to register values
    reg[0] = ADF4350_REG0_FRACT(settings->fractional_value) |
             ADF4350_REG0_INT(settings->integer_value);
    reg[1] = ADF4350_REG1_MOD(settings->mod_value) |
             ADF4350_REG1_PHASE(settings->phase_value) |
             ((settings->prescaler) ? ADF4350_REG1_PRESCALER : 0);

    reg[2] = ((settings->pd_polarity) ? ADF4350_REG2_PD_POLARITY_POS : 0) |
             ((settings->ldp) ? ADF4350_REG2_LDP_10ns : ADF4350_REG2_LDP_6ns) |
             ((settings->ldf) ? ADF4350_REG2_LDF_INT_N : ADF4350_REG2_LDF_FRACT_N) |
             ADF4350_REG2_CHARGE_PUMP_CURR_uA(settings->charge_pump_current_uA) |
             ADF4350_REG2_10BIT_R_CNT(settings->r_counter) |
             ((settings->r_div2) ? ADF4350_REG2_RDIV2_EN : 0) |
             ((settings->r_mult2) ? ADF4350_REG2_RMULT2_EN : 0) |
             ADF4350_REG2_MUXOUT(settings->muxout) |
             ADF4350_REG2_NOISE_MODE(settings->noise_mode);
    reg[3] = ADF4350_REG3_12BIT_CLKDIV(settings->clk_divider) |
             ADF4350_REG3_12BIT_CLKDIV_MODE(settings->clk_divider_mode) |
             ((settings->cycle_slip_reduction) ? ADF4350_REG3_12BIT_CSR_EN : 0) |
             ((settings->charge_cancellation) ? ADF4351_REG3_CHARGE_CANCELLATION_EN : 0) |
             ((settings->anti_backlash) ? ADF4351_REG3_ANTI_BACKLASH_3ns_EN : 0) |
             ((settings->band_sel_clock_mode) ? ADF4351_REG3_BAND_SEL_CLOCK_MODE_HIGH : 0);

    reg[4] = ADF4350_REG4_OUTPUT_PWR(settings->output_power) |
             ((settings->rf_output_enable) ? ADF4350_REG4_RF_OUT_EN : 0) |
             ADF4350_REG4_AUX_OUTPUT_PWR(settings->aux_output_power) |
             ((settings->aux_output_enable) ? ADF4350_REG4_AUX_OUTPUT_EN : 0) |
             ((settings->aux_output_mode) ? ADF4350_REG4_AUX_OUTPUT_FUND : ADF4350_REG4_AUX_OUTPUT_DIV) |
             ((settings->mute_till_lock) ? ADF4350_REG4_MUTE_TILL_LOCK_EN : 0) |
             ADF4350_REG4_8BIT_BAND_SEL_CLKDIV(settings->band_sel_clk_div) |
             ADF4350_REG4_RF_DIV_SEL(settings->rf_divider) |
             ((settings->feedback_divided) ? ADF4350_REG4_FEEDBACK_DIVIDED : ADF4350_REG4_FEEDBACK_FUND) |
             ((settings->vco_power_down) ? ADF4350_REG4_VCO_PWRDOWN_EN : 0);

    reg[5] = ((settings->ld_pin_mode == 0) || (settings->ld_pin_mode == 2)) ? ADF4350_REG5_LD_PIN_MODE_LOW :
             (settings->ld_pin_mode == 1) ? ADF4350_REG5_LD_PIN_MODE_DIGITAL :
             (settings->ld_pin_mode == 3) ? ADF4350_REG5_LD_PIN_MODE_HIGH : 0;
}

/**
  * @brief  SPI write function to send data to the ADF4351 register
  *
  * @note   This function uses SPI communication to send the register address and value to the ADF4351.
  *
  * @param  regid specifies the register address to be written.
  *         This parameter can be any value from 0 to 5 (ADF4351_REG0 to ADF4351_REG5).
  * @param  value specifies the value to be written to the register.
  *         This parameter can be any 32-bit value.
  *         the low 3 bits of the value are used to specify the register address,which should be masked out.(000)
  *         The remaining bits are used to specify the register value.
  * @retval None
  */
void ADF4351_WriteReg(uint8_t regid, uint32_t value)
{
    // Write the specified value to the ADF4351 register
    // Example: Use SPI communication to send the register address and value
    value&=~0x7; // Mask out the low 3 bits for register address
    value |= regid; // Set the register address in the low 3 bits
    uint8_t data[4]={value>>24,value>>16,value>>8,value}; // Prepare the data to be sent
    HAL_GPIO_WritePin(ADF_LE_GPIO_Port, ADF_LE_Pin, GPIO_PIN_RESET); // Set LE low to start transmission
    mydelay(2); // Small delay to ensure LE is low
    HAL_SPI_Transmit_DMA(&hspi1, data, 4); // Send value
    HAL_GPIO_WritePin(ADF_LE_GPIO_Port, ADF_LE_Pin, GPIO_PIN_SET); // Set LE high to latch the value
    mydelay(2); // Small delay to ensure the value is latched
}
/**
  * @brief  Write  all 6 registers  of ADF4351
  *
  * @note   This function writes all 6 registers of ADF4351 in order.
  *
  * @param  reg specifies the register value to be written
  *         This parameter should be a pointer to an array of 6 uint32_t values.
  * @retval None
  */
void ADF4351_WriteAllReg(uint32_t *reg)
{
    // Write all registers to the ADF4351
    for (uint8_t i = 0; i < 6; i++) {
        ADF4351_WriteReg(i, reg[i]); // Write each register value
        mydelay(10); // Small delay between register writes
    }
}


void ADF4351_Init(struct adf4351_platform_data *myadf,struct adf4351_settings *settings)
{
    // Initialize the ADF4351 PLL here
    // Set the initial frequency, power level, etc.

    /**Initialize reg values.**/
    *(settings) = default_settings;
    myadf->clkin = 50000000; // Reference frequency in Hz (50MHz)
    //myadf->frq_res = 200000; // Frequency resolution in Hz (200kHz)
    //myadf->outfrq_fund = 2400000000; // VCO output fundamental frequency in Hz (2.4GHz)

    // reg[0] = ADF4350_REG0_FRACT(0) |
    //          ADF4350_REG0_INT(24);
    // reg[1] = ADF4350_REG1_MOD(2) |
    //          ADF4350_REG1_PHASE(1);
    // reg[2] = ADF4350_REG2_PD_POLARITY_POS |
    //          ADF4350_REG2_LDP_10ns |
    //          ADF4350_REG2_LDF_INT_N |
    //          ADF4350_REG2_CHARGE_PUMP_CURR_uA(2500) |
    //          ADF4350_REG2_10BIT_R_CNT(1) | 
    //         // ADF4350_REG2_RDIV2_EN |
    //          ADF4350_REG2_RMULT2_EN |
    //          ADF4350_REG2_MUXOUT(ADF4350_MUXOUT_DVDD)|
    //          ADF4350_REG2_NOISE_MODE(0);
    // reg[3] = ADF4350_REG3_12BIT_CLKDIV(0) |
    //          ADF4350_REG3_12BIT_CLKDIV_MODE(0) |
    //          ADF4350_REG3_12BIT_CSR_EN |
    //          ADF4351_REG3_CHARGE_CANCELLATION_EN |
    //          ADF4351_REG3_ANTI_BACKLASH_3ns_EN |
    //          ADF4351_REG3_BAND_SEL_CLOCK_MODE_HIGH;
    // reg[4] = ADF4350_REG4_OUTPUT_PWR(3) |
    //          ADF4350_REG4_RF_OUT_EN |
    //          ADF4350_REG4_AUX_OUTPUT_PWR(3) |
    //          ADF4350_REG4_AUX_OUTPUT_EN |
    //          ADF4350_REG4_AUX_OUTPUT_FUND |
    //         // ADF4350_REG4_MUTE_TILL_LOCK_EN |
    //          ADF4350_REG4_8BIT_BAND_SEL_CLKDIV(1) |
    //          ADF4350_REG4_RF_DIV_SEL(0) |
    //          ADF4350_REG4_FEEDBACK_FUND 
    //          ;
    // reg[5] = ADF4350_REG5_LD_PIN_MODE_DIGITAL;

    HAL_GPIO_WritePin(ADF_CE_GPIO_Port, ADF_CE_Pin, GPIO_PIN_SET); // Enable the PLL
    //ADF4351_SetFrequency_RF(2400000000, 200000, myadf,settings); // Set frequency to 2.4GHz,resolution 200kHz
    ADF4351_SetFrequency_FullBW(2400000000, 200000, myadf,settings);
    HAL_GPIO_WritePin(ADF_PDRF_GPIO_Port, ADF_PDRF_Pin, GPIO_PIN_SET); // Power up the RF output
}
void ADF4351_SetFrequency_RF(uint64_t frequency,uint32_t res, struct adf4351_platform_data *myadf,struct adf4351_settings *settings){
    // Set the RF frequency of the ADF4351 PLL (2.2GHz~4.4GHz)
    // Example: Use the frequency value to calculate the register values and write them to the PLL
    if(frequency < 2200000000 || frequency > 4400000000){
        // Frequency out of adf4351 vco range, return or handle error
        return;
    }
    if(frequency > 3600000000){
        settings->prescaler = 1; // Set prescaler to 8/9 for frequencies above 3.6GHz
    }else{
        settings->prescaler = 0; // Set prescaler to 4/5 for frequencies below 3.6GHz
    }
    myadf->pfd_frq = (myadf->clkin*(1+settings->r_mult2))/(settings->r_counter*(1+settings->r_div2)); // Calculate the PFD frequency
    // PFD frequency should below 32MHz when using Fractional-N mode
    //remember to change r_div2 & r_counter to get the PFD frequency below 32MHz
    settings->mod_value = myadf->clkin / res; // Calculate the modulus value
    settings->integer_value = frequency / myadf->pfd_frq; // Calculate the integer value
    settings->fractional_value = (frequency % myadf->pfd_frq) * settings->mod_value ; // Calculate the fractional value

    ADF4351_setting2reg(settings, myadf->reg); // Convert settings to register values
    ADF4351_WriteAllReg(myadf->reg); // Write all registers to the PLL
    ADF4351_WriteReg(ADF4350_REG0, (myadf->reg)[0]); // Write to REG0,ensure the double buffered regs are  correctly written

    myadf->frq_res = res;
    myadf->outfrq_fund = frequency; // Set the output frequency
    myadf->outfrq_div = frequency; // Set the output divided frequency
    
}

uint8_t rf_div=1;
void ADF4351_SetFrequency_FullBW(uint64_t frequency, uint32_t res, struct adf4351_platform_data *myadf,struct adf4351_settings *settings){
    // Set the RF frequency of the ADF4351 PLL  with full bandwidth (35MHz~4.4GHz)
    // Example: Use the frequency value to calculate the register values and write them to the PLL
    if(frequency < 35000000 || frequency > 4400000000){
        // Frequency out of adf4351 full BandWidth range, return or handle error
        return;
    }

    if(frequency < 2200000000){
        settings->prescaler = 0; // Set prescaler to 4/5 for frequencies below 2.2GHz (<3.6GHz)
        settings->feedback_divided = 0; // Fundamental feedback for frequencies below 2.2GHz
        if(frequency > 1100000000){
            settings->rf_divider = 1; // Divide by 2 for frequencies between 1.1GHz and 2.2GHz
            rf_div = 2;
        }else if(frequency > 550000000){
            settings->rf_divider = 2; // Divide by 4 for frequencies between 550MHz and 1.1GHz
            rf_div = 4;
        }else if(frequency > 275000000){
            settings->rf_divider = 3; // Divide by 8 for frequencies between 275MHz and 550MHz
            rf_div = 8;
        }else if(frequency > 137500000){
            settings->rf_divider = 4; // Divide by 16 for frequencies between 137.5MHz and 275MHz
            rf_div = 16;
        }else if(frequency > 68750000){
            settings->rf_divider = 5; // Divide by 32 for frequencies between 68.75MHz and 137.5MHz
            rf_div = 32;
        }else if(frequency > 34375000){
            settings->rf_divider = 6; // Divide by 64 for frequencies between 34.375MHz and 68.75MHz
            rf_div = 64;
        }
        myadf->pfd_frq = (myadf->clkin*(1+settings->r_mult2))/(settings->r_counter*(1+settings->r_div2)); // Calculate the PFD frequency
        // PFD frequency should below 32MHz when using Fractional-N mode
        //remember to change r_div2 & r_counter to get the PFD frequency below 32MHz
        settings->mod_value = myadf->clkin / (res*rf_div); // Calculate the modulus value
        settings->integer_value = frequency / (myadf->pfd_frq / rf_div); // Calculate the integer value
        settings->fractional_value = (frequency % (myadf->pfd_frq/rf_div)) * settings->mod_value ; // Calculate the fractional value

        ADF4351_setting2reg(settings, myadf->reg); // Convert settings to register values
        ADF4351_WriteAllReg(myadf->reg); // Write all registers to the PLL
        ADF4351_WriteReg(ADF4350_REG0, (myadf->reg)[0]); // Write to REG0,ensure the double buffered regs are  correctly written
        
        myadf->frq_res = res;
        myadf->outfrq_fund = frequency*rf_div; // Set the output frequency
        myadf->outfrq_div = frequency; // Set the output divided frequency
    }
    else {  //for frequencies above 2.2GHz, no need to use RFout divider
        settings->rf_divider = 0; // No division for frequencies above 2.2GHz
        settings->feedback_divided = 0; // Fundamental feedback for frequencies above 2.2GHz
        ADF4351_SetFrequency_RF(frequency, res, myadf,settings); // Set frequency to the desired value
    }
    
}

void mydelay(uint32_t t){
    // Custom delay function
    // Example: Use a loop to create a delay of t milliseconds
    for(uint32_t i = 0; i < t; i++){
        for(uint32_t j = 0; j < 10; j++){
            __NOP(); // No operation, just waste time
        }
    }
}