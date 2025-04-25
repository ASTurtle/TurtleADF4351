/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * adf4351.h  ADF4350/ADF4351 SPI PLL driver 
 *
 * Copyright 2012-2013 Analog Devices Inc.
*
* UESTC_Auto-Centor4Makers Turtle PORTED
* 2025/4/18
 */

 #ifndef _ADF4351_H_
 #define _ADF4351_H_
 
// Include necessary headers
#include "main.h"
#include "spi.h"
#include "gpio.h"
#include "stdbool.h"
#include <stdint.h>

 /* Registers */
 #define ADF4350_REG0	0
 #define ADF4350_REG1	1
 #define ADF4350_REG2	2
 #define ADF4350_REG3	3
 #define ADF4350_REG4	4
 #define ADF4350_REG5	5
 
 /* REG0 Bit Definitions */
 #define ADF4350_REG0_FRACT(x)			(((x) & 0xFFF) << 3)
 #define ADF4350_REG0_INT(x)			(((x) & 0xFFFF) << 15)
 
 /* REG1 Bit Definitions */
 #define ADF4350_REG1_MOD(x)			(((x) & 0xFFF) << 3)
 #define ADF4350_REG1_PHASE(x)			(((x) & 0xFFF) << 15)
 #define ADF4350_REG1_PRESCALER			(1 << 27)
 
 /* REG2 Bit Definitions */
 #define ADF4350_REG2_COUNTER_RESET_EN		(1 << 3)
 #define ADF4350_REG2_CP_THREESTATE_EN		(1 << 4)
 #define ADF4350_REG2_POWER_DOWN_EN		(1 << 5)
 #define ADF4350_REG2_PD_POLARITY_POS		(1 << 6)
 #define ADF4350_REG2_LDP_6ns			(1 << 7)
 #define ADF4350_REG2_LDP_10ns			(0 << 7)
 #define ADF4350_REG2_LDF_FRACT_N		(0 << 8)
 #define ADF4350_REG2_LDF_INT_N			(1 << 8)
 #define ADF4350_REG2_CHARGE_PUMP_CURR_uA(x)	(((((x)-312) / 312) & 0xF) << 9)
 #define ADF4350_REG2_DOUBLE_BUFF_EN		(1 << 13)
 #define ADF4350_REG2_10BIT_R_CNT(x)		(((x) & 0x3FF) << 14)
 #define ADF4350_REG2_RDIV2_EN			(1 << 24)
 #define ADF4350_REG2_RMULT2_EN			(1 << 25)
 #define ADF4350_REG2_MUXOUT(x)			((x) << 26)
 #define ADF4350_REG2_NOISE_MODE(x)		(((unsigned)(x)) << 29)
 #define ADF4350_MUXOUT_THREESTATE		0
 #define ADF4350_MUXOUT_DVDD			1
 #define ADF4350_MUXOUT_GND			2
 #define ADF4350_MUXOUT_R_DIV_OUT		3
 #define ADF4350_MUXOUT_N_DIV_OUT		4
 #define ADF4350_MUXOUT_ANALOG_LOCK_DETECT	5
 #define ADF4350_MUXOUT_DIGITAL_LOCK_DETECT	6
 
 /* REG3 Bit Definitions */
 #define ADF4350_REG3_12BIT_CLKDIV(x)		((x) << 3)
 #define ADF4350_REG3_12BIT_CLKDIV_MODE(x)	((x) << 15)
 #define ADF4350_REG3_12BIT_CSR_EN		(1 << 18)
 #define ADF4351_REG3_CHARGE_CANCELLATION_EN	(1 << 21)
 #define ADF4351_REG3_ANTI_BACKLASH_3ns_EN	(1 << 22)
 #define ADF4351_REG3_BAND_SEL_CLOCK_MODE_HIGH	(1 << 23)
 
 /* REG4 Bit Definitions */
 #define ADF4350_REG4_OUTPUT_PWR(x)		((x) << 3)
 #define ADF4350_REG4_RF_OUT_EN			(1 << 5)
 #define ADF4350_REG4_AUX_OUTPUT_PWR(x)		((x) << 6)
 #define ADF4350_REG4_AUX_OUTPUT_EN		(1 << 8)
 #define ADF4350_REG4_AUX_OUTPUT_FUND		(1 << 9)
 #define ADF4350_REG4_AUX_OUTPUT_DIV		(0 << 9)
 #define ADF4350_REG4_MUTE_TILL_LOCK_EN		(1 << 10)
 #define ADF4350_REG4_VCO_PWRDOWN_EN		(1 << 11)
 #define ADF4350_REG4_8BIT_BAND_SEL_CLKDIV(x)	((x) << 12)
 #define ADF4350_REG4_RF_DIV_SEL(x)		((x) << 20)
 #define ADF4350_REG4_FEEDBACK_DIVIDED		(0 << 23)
 #define ADF4350_REG4_FEEDBACK_FUND		(1 << 23)
 
 /* REG5 Bit Definitions */
 #define ADF4350_REG5_LD_PIN_MODE_LOW		(0 << 22)
 #define ADF4350_REG5_LD_PIN_MODE_DIGITAL	(1 << 22)
 #define ADF4350_REG5_LD_PIN_MODE_HIGH		(3 << 22)

 
 /* Specifications */
 #define ADF4350_MAX_OUT_FREQ		4400000000ULL /* Hz */
 #define ADF4350_MIN_OUT_FREQ		137500000 /* Hz */
 #define ADF4351_MIN_OUT_FREQ		34375000 /* Hz */
 #define ADF4350_MIN_VCO_FREQ		2200000000ULL /* Hz */
 #define ADF4350_MAX_FREQ_45_PRESC	3000000000ULL /* Hz */
 #define ADF4350_MAX_FREQ_PFD		32000000 /* Hz */
 #define ADF4350_MAX_BANDSEL_CLK		125000 /* Hz */
 #define ADF4350_MAX_FREQ_REFIN		250000000 /* Hz */
 #define ADF4350_MAX_MODULUS		4095
 #define ADF4350_MAX_R_CNT		1023
 
 
 /**
  * struct adf4350_platform_data - platform specific information
  * @name:		Optional device name.
  * @clkin:		REFin frequency in Hz.
  * @channel_spacing:	Channel spacing in Hz (influences MODULUS).
  * @power_up_frequency:	Optional, If set in Hz the PLL tunes to the desired
  *			frequency on probe.
  * @ref_div_factor:	Optional, if set the driver skips dynamic calculation
  *			and uses this default value instead.
  * @ref_doubler_en:	Enables reference doubler.
  * @ref_div2_en:	Enables reference divider.
  * @r2_user_settings:	User defined settings for ADF4350/1 REGISTER_2.
  * @r3_user_settings:	User defined settings for ADF4350/1 REGISTER_3.
  * @r4_user_settings:	User defined settings for ADF4350/1 REGISTER_4.
  */
 
//  struct adf4350_platform_data {
//      char			name[32];
//      unsigned long		clkin;
//      unsigned long		channel_spacing;
//      unsigned long long	power_up_frequency;
 
//      unsigned short		ref_div_factor; /* 10-bit R counter */
//      bool			ref_doubler_en;
//      bool			ref_div2_en;
 
//      unsigned		r2_user_settings;
//      unsigned		r3_user_settings;
//      unsigned		r4_user_settings;
//  };

/****The following contents are implemented by Turtle@AutoC4M****/

struct adf4351_platform_data {
    uint32_t clkin; // Reference frequency in Hz
    uint32_t frq_res; // Frequency resolution in Hz,influences MODulus
    uint32_t pfd_frq; // Phase frequency detector frequency in Hz
    uint64_t outfrq_demand; // USER Desired output frequency in Hz
    uint64_t outfrq_fund; //vco output fundamental frequency in Hz
    uint64_t outfrq_div; //output divided frequency in Hz
    struct adf4351_settings *settings; // Pointer to ADF4351 settings structure
    uint32_t reg[6]; // Pointer to register values array
};

struct adf4351_settings {
    uint16_t integer_value; // 16bit Integer value for frequency calculation  min23 for PRESCALER = 4/5 and min75 for PRESCALER = 8/9
    uint16_t fractional_value; // 12bit Fractional value for frequency calculation
    uint16_t mod_value; // 12bit Modulus value for frequency calculation [DBR]
    uint16_t phase_value; // 12bit Phase value for frequency calculation [DBR]
    
    uint8_t prescaler : 1; // Prescaler value (0-1) 0: 4/5, 1: 8/9  when the frequency is above 3.6GHz, the prescaler must be set to 8/9.
    uint8_t phase_adjust : 1; // Phase adjustment (0-1) 0: No phase adjustment, 1: Phase adjustment enabled
    uint8_t counter_reset_en : 1; // Counter reset (0-1) 0: No reset, 1: Reset enabled
    uint8_t charge_pump_3state_en : 1; // Charge pump mode (0-1) 0: Normal, 1: Charge  Pump Three-state
    uint8_t power_down : 1; // Power down (0-1) 0: Power up, 1: Power down
    uint8_t pd_polarity : 1; // PD polarity (0-1) 0: Negative, 1: Positive
    uint8_t ldp :  1; // Lock Detect Precision (0-1) 0: 6ns, 1: 10ns
    uint8_t ldf : 1; // Loop divider feedback (0-1) 0: Fractional, 1: Integer

    uint16_t charge_pump_current_uA; // Charge pump current (uA), note that this value is converted to 4-bit value by the driver.
    uint16_t r_counter; // 10-bit R counter value(1-1023)

    uint8_t r_div2 : 1; // Reference divided-by-2 (0-1) 0: Disabled, 1: Enabled
    uint8_t r_mult2 : 1; // Reference multiplied-by-2 (0-1) 0: Disabled, 1: Enabled
    uint8_t muxout : 3; // MUXOUT mode (0-7) 0: Three-state, 1: DVDD, 2: GND, 3: R_DIV_OUT, 4: N_DIV_OUT, 5: Analog lock detect, 6: Digital lock detect
    uint8_t noise_mode : 2; // Noise mode (0-3) 0: Low noise, 1: Reserved, 2: Reserved, 3: Low spurious
    uint8_t double_buffer_r4 : 1; // Double buffer r4[db22:db20] (0-1) 0: Disabled, 1: Enabled 

    uint16_t clk_divider; // 12-bit clock divider value (0-4095) for ADF4351_REG3_CLKDIV
    
    uint8_t clk_divider_mode : 2; // Clock divider mode (0-3) 0: Off, 1: Fast Lock Enable, 2: Resync Enable, 3: Reserved
    uint8_t cycle_slip_reduction : 1; // Cycle slip reduction (0-1) 0: Disabled, 1: Enabled
    uint8_t charge_cancellation : 1; // Charge cancellation (0-1) 0: Disabled, 1: Enabled
    uint8_t anti_backlash : 1; // Anti-backlash pulse width (0-1) 0: 6ns(FRAC-N), 1: 3ns(INT-N)
    uint8_t band_sel_clock_mode : 1; // Band select clock mode (0-1) 0: Low, 1: High

    uint8_t output_power : 2; // OutA Power level (0-3)     0: -4dBm, 1: -1dBm, 2: +2dBm, 3: +5dBm
    uint8_t rf_output_enable : 1; // RF output enable (0-1) 1: Enable, 0: Disable
    uint8_t aux_output_power : 2; // OutB Auxiliary output Power level (0-3)
    uint8_t aux_output_enable : 1; // Auxiliary output enable (0-1)
    uint8_t aux_output_mode : 1; // Auxiliary output mode (0-1)   0: Divided, 1: Fundamental
    uint8_t mute_till_lock : 1; // Mute till lock (0-1)     1: Mute, 0: Unmute

    uint8_t band_sel_clk_div : 8; // Band select clock divider (0-255)

    uint8_t vco_power_down : 1; // VCO power down (0-1)   1: Power down, 0: Power up
    uint8_t feedback_divided : 1; // Feedback divided  or not (0-1)   0: Fundamental, 1: Divided
    uint8_t rf_divider : 3; // RF divider (0-6)   0: No division, 1: Divide by 2, 2: Divide by 4, etc.
    uint8_t ld_pin_mode : 2; // LD pin mode (0-3)   0: Low, 1: Digital LockDetect, 2: Low, 3: High
    
    
};

void ADF4351_setting2reg(struct adf4351_settings *settings, uint32_t *reg);
void ADF4351_WriteReg(uint8_t reg, uint32_t value);
void ADF4351_WriteAllReg(uint32_t *reg);
void ADF4351_Init(struct adf4351_platform_data *myadf,struct adf4351_settings *settings);
void ADF4351_SetFrequency_RF(uint64_t frequency,uint32_t res, struct adf4351_platform_data *myadf,struct adf4351_settings *settings);
void ADF4351_SetFrequency_FullBW(uint64_t frequency, uint32_t res, struct adf4351_platform_data *myadf,struct adf4351_settings *settings);
void mydelay(uint32_t t);
 

extern struct adf4351_platform_data myADF4351;
extern struct adf4351_settings myADF4351_settings;
extern uint32_t adf4351_reg[6];

 #endif /* IIO_PLL_ADF4350_H_ */
