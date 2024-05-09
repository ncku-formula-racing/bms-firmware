/*!
LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter

Graphs G37 - G44 in the LTC3335 datasheet show the typical error in the
coulomb counter and battery current measurement error for various IPEAK,
VOUT, and battery voltage conditions.  The LTC3335 accuracy is impressive
when configured for high values of IPEAK.  When configured with low values
of IPEAK, however, there can be up to 40% error.

This file contains the typical error for all IPEAK, VOUT, and battery voltage
conditions encoded as a signed 16bit integer.  An array of these errors will
build into the firmware as LTC3335_Software_Correction_Table[] for the IPEAK
and VOUT values specified in LTC3335_Config.h.  The index into this table
will be the battery voltage, which may vary over life of a product using the
LTC3335.

To use these correction factors to adjust the coulomb count and battery current
measurement, multiply the unadjusted value by the correction factor and shift
right by 16.  For example:

  If IPEAK = 10mA (LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_10MA),
  VOUT = 3.3V (LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V), and
  VBAT = 4.5V (vbat = 4500mV), then the LTC3335_Get_Software_Correction_Factor(uint16_t vbat)
  function will return 8826 as the software correction factor.  This value translates to
  8826/65536 = 13.4%, which matches the error shown in G43 of the datasheet.

  If a prescaler of 5 was selected for this example, then one count of the
  accumulator should represent 31638 mAs (0.0088 Ah) of charge.  To adjust for
  the expected error, we should add the fraction returned by the
  LTC3335_Get_Software_Correction_Factor(uint16_t vbat) function to the
  unadjusted coulomb count:

  31638mAs + (31638mAs * 8826)  >> 16 = 35899 mAs.

Note! - These correction factors are only valid at room temperature and for the recommended
Coilcraft LPS5030-xxxMRB inductor listed in the datasheet.  Contact LTC applications if
interested in generating the software correction factors for other tempeature conditions
and inductors.

@verbatim

The LTC®3335 is a high efficiency, low quiescent current
(680nA) buck-boost DC/DC converter with an integrated
precision coulomb counter which monitors accumulated
battery discharge in long life battery powered applications.
The buck-boost can operate down to 1.8V on its input and
provides eight pin-selectable output voltages with up to
50mA of output current.

The coulomb counter stores the accumulated battery discharge
in an internal register accessible via an I2C interface.
The LTC3335 features a programmable discharge alarm
threshold. When the threshold is reached, an interrupt is
generated at the IRQ pin.

To accommodate a wide range of battery types and sizes,
the peak input current can be selected from as low as 5mA
to as high as 250mA and the full-scale coulomb counter
has a programmable range of 32,768:1.

@endverbatim

http://www.linear.com/product/LTC3335

http://www.linear.com/product/LTC3335#demoboards


Copyright 2018(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! @file
    @ingroup LTC3335
    Coulomb Counter and Current Correction Tables for LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter
*/

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Includes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "LTC3335.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define LTC3335_VBAT_MIN 1800 //! Battery voltage (in mV) corresponding to first index in table.
#define LTC3335_VBAT_STEP 100 //! Battery voltage (in mV) step between each index in table.
#define LTC3335_VBAT_NUM 38   //! Number of battery voltages in table.
#define LTC3335_VBAT_MAX (LTC3335_VBAT_MIN + LTC3335_VBAT_STEP * (LTC3335_VBAT_NUM - 1)) //! Battery voltage (in mV) corresponding to last index in table.
#define LTC3335_VBAT_TO_TABLE_INDEX(vbat) ((uint8_t)((vbat - LTC3335_VBAT_MIN) / LTC3335_VBAT_STEP)) //! Translates a battery voltage to a table index.

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//! Tables of typical error of LTC3335 at a given battery voltage, output voltage, and iPeak.
//! See Graphs G37 - G44 in LTC3335 datasheet.
#ifdef LTC3335_USE_SOFTWARE_CORRECTION
#if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_5MA
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {10886, 11274, 11652, 12022, 12387, 12748, 13108, 13467, 13826, 14187, 14549, 14915, 15282, 15653, 16027, 16403, 16781, 17162, 17545, 17929, 18314, 18700, 19087, 19473, 19860, 20247, 20634, 21021, 21409, 21798, 22188, 22581, 22979, 23381, 23791, 24210, 24640, 25085};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {9622, 9958, 10287, 10609, 10927, 11241, 11553, 11864, 12176, 12488, 12802, 13119, 13438, 13761, 14087, 14416, 14750, 15087, 15428, 15772, 16120, 16472, 16827, 17185, 17546, 17909, 18276, 18645, 19016, 19390, 19766, 20145, 20527, 20911, 21298, 21690, 22085, 22485};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {9114, 9461, 9794, 10116, 10430, 10737, 11040, 11340, 11638, 11937, 12237, 12540, 12845, 13153, 13465, 13781, 14102, 14427, 14756, 15089, 15427, 15768, 16113, 16461, 16813, 17167, 17524, 17883, 18244, 18608, 18974, 19343, 19714, 20088, 20466, 20848, 21235, 21627};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {8819, 9164, 9494, 9813, 10123, 10426, 10724, 11020, 11313, 11606, 11901, 12197, 12495, 12797, 13103, 13412, 13726, 14044, 14366, 14693, 15023, 15358, 15696, 16038, 16382, 16730, 17081, 17435, 17790, 18149, 18509, 18872, 19238, 19607, 19979, 20355, 20736, 21122};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {8426, 8759, 9080, 9392, 9696, 9994, 10288, 10579, 10868, 11157, 11446, 11736, 12029, 12324, 12622, 12923, 13228, 13537, 13850, 14167, 14488, 14813, 15141, 15473, 15809, 16149, 16491, 16837, 17185, 17537, 17891, 18248, 18607, 18969, 19335, 19703, 20075, 20451};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {8058, 8385, 8701, 9009, 9310, 9606, 9898, 10186, 10473, 10759, 11045, 11331, 11619, 11908, 12200, 12495, 12793, 13094, 13399, 13708, 14020, 14337, 14657, 14982, 15310, 15642, 15977, 16316, 16659, 17004, 17353, 17705, 18059, 18416, 18776, 19138, 19503, 19869};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {6891, 7319, 7709, 8068, 8401, 8713, 9010, 9294, 9569, 9839, 10106, 10372, 10639, 10908, 11181, 11457, 11738, 12024, 12315, 12610, 12910, 13215, 13523, 13835, 14151, 14469, 14789, 15111, 15436, 15763, 16092, 16424, 16760, 17101, 17447, 17802, 18166, 18542};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {6630, 6964, 7289, 7605, 7914, 8216, 8512, 8803, 9090, 9374, 9655, 9933, 10210, 10487, 10763, 11039, 11316, 11595, 11876, 12159, 12445, 12734, 13027, 13323, 13624, 13929, 14239, 14553, 14872, 15197, 15526, 15861, 16200, 16544, 16894, 17247, 17606, 17968};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
#endif // #if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_5MA
#if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_10MA
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {4951, 5152, 5354, 5559, 5766, 5977, 6191, 6410, 6633, 6860, 7092, 7328, 7569, 7815, 8065, 8320, 8580, 8844, 9112, 9384, 9661, 9941, 10225, 10512, 10803, 11097, 11394, 11694, 11997, 12302, 12610, 12921, 13234, 13550, 13869, 14191, 14516, 14844};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {4225, 4386, 4552, 4722, 4897, 5077, 5262, 5451, 5645, 5843, 6046, 6253, 6464, 6679, 6898, 7121, 7348, 7578, 7812, 8050, 8291, 8535, 8783, 9034, 9288, 9547, 9809, 10074, 10344, 10618, 10897, 11180, 11469, 11763, 12064, 12371, 12686, 13008};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {3832, 4025, 4213, 4398, 4580, 4762, 4944, 5127, 5312, 5500, 5691, 5885, 6083, 6285, 6491, 6702, 6917, 7136, 7360, 7588, 7820, 8056, 8295, 8539, 8786, 9037, 9291, 9549, 9810, 10075, 10344, 10617, 10895, 11177, 11466, 11761, 12063, 12373};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {3603, 3812, 4011, 4203, 4389, 4572, 4752, 4932, 5112, 5294, 5478, 5666, 5857, 6052, 6252, 6456, 6664, 6878, 7095, 7318, 7545, 7775, 8010, 8249, 8491, 8737, 8986, 9239, 9495, 9754, 10017, 10284, 10556, 10832, 11114, 11403, 11699, 12004};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {3312, 3533, 3741, 3937, 4126, 4308, 4486, 4662, 4837, 5013, 5191, 5371, 5555, 5742, 5934, 6131, 6332, 6538, 6748, 6964, 7183, 7407, 7634, 7866, 8101, 8339, 8581, 8826, 9074, 9325, 9580, 9839, 10103, 10372, 10646, 10928, 11219, 11519};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {3058, 3283, 3493, 3690, 3879, 4060, 4237, 4411, 4584, 4757, 4932, 5109, 5288, 5471, 5658, 5850, 6046, 6246, 6450, 6659, 6872, 7089, 7310, 7534, 7762, 7993, 8227, 8464, 8705, 8949, 9197, 9449, 9706, 9969, 10238, 10516, 10802, 11100};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {2148, 2424, 2678, 2913, 3132, 3338, 3534, 3723, 3905, 4084, 4261, 4438, 4614, 4792, 4972, 5154, 5340, 5529, 5722, 5918, 6118, 6321, 6529, 6739, 6953, 7171, 7391, 7614, 7841, 8071, 8304, 8540, 8781, 9026, 9276, 9532, 9796, 10068};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {1588, 1914, 2209, 2478, 2725, 2953, 3167, 3369, 3562, 3748, 3930, 4109, 4287, 4465, 4644, 4825, 5008, 5195, 5385, 5578, 5775, 5975, 6179, 6386, 6597, 6811, 7028, 7248, 7472, 7698, 7928, 8162, 8400, 8642, 8891, 9146, 9410, 9683};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
#endif // #if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_10MA
#if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_15MA
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {3046, 3189, 3325, 3457, 3587, 3717, 3850, 3986, 4126, 4272, 4425, 4583, 4749, 4921, 5100, 5286, 5478, 5675, 5878, 6086, 6298, 6514, 6734, 6956, 7182, 7410, 7640, 7873, 8109, 8349, 8592, 8841, 9095, 9358, 9630, 9914, 10211, 10526};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {2510, 2635, 2754, 2869, 2981, 3093, 3207, 3323, 3442, 3567, 3696, 3832, 3973, 4120, 4274, 4434, 4599, 4770, 4946, 5127, 5312, 5502, 5695, 5891, 6091, 6294, 6499, 6707, 6918, 7133, 7352, 7575, 7804, 8039, 8283, 8536, 8802, 9082};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {2346, 2455, 2561, 2666, 2772, 2879, 2988, 3100, 3216, 3337, 3462, 3592, 3728, 3869, 4015, 4167, 4323, 4484, 4651, 4821, 4996, 5174, 5357, 5543, 5732, 5925, 6121, 6321, 6524, 6731, 6942, 7158, 7380, 7608, 7843, 8087, 8340, 8606};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {2187, 2304, 2416, 2525, 2632, 2740, 2849, 2960, 3074, 3192, 3315, 3441, 3573, 3710, 3851, 3998, 4150, 4306, 4467, 4632, 4802, 4976, 5153, 5334, 5518, 5706, 5897, 6091, 6289, 6491, 6698, 6909, 7126, 7349, 7579, 7819, 8068, 8330};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {1888, 2037, 2173, 2299, 2418, 2533, 2645, 2756, 2869, 2983, 3101, 3222, 3348, 3479, 3615, 3756, 3902, 4053, 4208, 4369, 4533, 4702, 4874, 5049, 5228, 5409, 5594, 5782, 5973, 6167, 6366, 6570, 6779, 6996, 7221, 7457, 7705, 7968};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {1567, 1753, 1918, 2065, 2199, 2323, 2441, 2555, 2668, 2780, 2895, 3013, 3135, 3262, 3394, 3531, 3673, 3821, 3973, 4130, 4291, 4456, 4625, 4796, 4970, 5147, 5326, 5508, 5693, 5881, 6072, 6269, 6472, 6684, 6905, 7139, 7388, 7655};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {904, 1094, 1267, 1426, 1576, 1717, 1852, 1983, 2112, 2239, 2366, 2493, 2622, 2752, 2885, 3021, 3159, 3300, 3444, 3590, 3740, 3892, 4046, 4204, 4363, 4526, 4692, 4860, 5032, 5208, 5389, 5576, 5770, 5971, 6182, 6404, 6640, 6892};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {245, 579, 861, 1099, 1302, 1477, 1631, 1769, 1896, 2017, 2133, 2249, 2367, 2487, 2611, 2740, 2874, 3013, 3157, 3306, 3459, 3615, 3774, 3935, 4097, 4261, 4425, 4590, 4757, 4924, 5095, 5270, 5451, 5641, 5844, 6062, 6301, 6565};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
#endif // #if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_15MA
#if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_25MA
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {2264, 2228, 2213, 2216, 2236, 2272, 2321, 2383, 2456, 2539, 2631, 2731, 2838, 2952, 3071, 3196, 3325, 3458, 3596, 3736, 3880, 4027, 4177, 4330, 4486, 4645, 4808, 4973, 5142, 5315, 5491, 5672, 5858, 6049, 6245, 6447, 6656, 6871};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {1958, 1938, 1931, 1936, 1953, 1981, 2018, 2065, 2121, 2185, 2257, 2336, 2421, 2512, 2609, 2711, 2818, 2930, 3046, 3166, 3289, 3416, 3546, 3680, 3817, 3956, 4099, 4245, 4395, 4547, 4703, 4863, 5027, 5195, 5367, 5545, 5728, 5916};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {1648, 1689, 1727, 1765, 1802, 1842, 1885, 1933, 1985, 2043, 2107, 2178, 2255, 2338, 2427, 2523, 2624, 2731, 2843, 2959, 3080, 3204, 3332, 3462, 3595, 3730, 3868, 4007, 4149, 4293, 4440, 4590, 4744, 4903, 5068, 5240, 5421, 5614};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {1498, 1559, 1612, 1660, 1705, 1750, 1795, 1843, 1895, 1951, 2013, 2080, 2154, 2234, 2320, 2413, 2511, 2615, 2724, 2837, 2955, 3077, 3202, 3330, 3460, 3592, 3727, 3863, 4001, 4141, 4284, 4430, 4580, 4735, 4896, 5065, 5243, 5434};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {1359, 1418, 1471, 1519, 1566, 1612, 1659, 1708, 1761, 1817, 1879, 1946, 2018, 2096, 2179, 2268, 2363, 2462, 2567, 2676, 2789, 2905, 3025, 3148, 3274, 3402, 3533, 3665, 3800, 3937, 4077, 4220, 4366, 4517, 4674, 4837, 5008, 5190};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {1267, 1307, 1347, 1388, 1430, 1475, 1524, 1575, 1631, 1691, 1755, 1823, 1897, 1974, 2057, 2144, 2236, 2331, 2432, 2536, 2643, 2755, 2870, 2988, 3109, 3233, 3360, 3490, 3623, 3758, 3897, 4038, 4183, 4332, 4484, 4642, 4804, 4972};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {644, 746, 837, 920, 998, 1071, 1142, 1211, 1280, 1350, 1421, 1494, 1570, 1649, 1731, 1817, 1906, 1998, 2095, 2194, 2297, 2402, 2511, 2622, 2736, 2852, 2971, 3091, 3214, 3339, 3466, 3596, 3729, 3866, 4007, 4153, 4304, 4463};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {539, 591, 649, 712, 779, 850, 923, 1000, 1079, 1159, 1242, 1326, 1411, 1498, 1586, 1675, 1766, 1858, 1952, 2047, 2145, 2244, 2345, 2449, 2556, 2666, 2778, 2894, 3013, 3136, 3263, 3394, 3529, 3668, 3811, 3958, 4109, 4264};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
#endif // #if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_25MA
#if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_50MA
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {252, 168, 104, 60, 32, 20, 22, 36, 62, 98, 143, 196, 256, 323, 395, 472, 554, 639, 728, 820, 915, 1012, 1113, 1216, 1322, 1430, 1541, 1656, 1774, 1895, 2021, 2151, 2286, 2427, 2574, 2727, 2888, 3057};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-195, -220, -236, -245, -245, -237, -222, -199, -170, -133, -90, -41, 14, 74, 139, 208, 282, 359, 440, 525, 612, 702, 794, 889, 987, 1086, 1189, 1293, 1400, 1511, 1624, 1742, 1863, 1990, 2123, 2262, 2409, 2565};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-391, -375, -362, -348, -334, -317, -297, -273, -245, -211, -172, -128, -78, -23, 38, 103, 173, 247, 326, 407, 493, 580, 671, 764, 858, 955, 1054, 1154, 1257, 1362, 1470, 1582, 1698, 1820, 1948, 2084, 2231, 2389};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-510, -474, -445, -421, -398, -376, -352, -327, -298, -265, -227, -184, -136, -83, -25, 38, 107, 179, 256, 336, 420, 506, 595, 686, 779, 874, 971, 1069, 1169, 1271, 1376, 1485, 1599, 1718, 1844, 1979, 2126, 2285};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-672, -618, -574, -537, -505, -475, -446, -416, -384, -350, -311, -268, -221, -169, -112, -50, 16, 87, 161, 239, 321, 405, 491, 579, 670, 762, 855, 951, 1048, 1147, 1250, 1356, 1467, 1583, 1708, 1841, 1986, 2146};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-824, -762, -710, -665, -625, -587, -551, -515, -478, -438, -396, -350, -301, -248, -191, -129, -64, 5, 77, 153, 231, 312, 396, 482, 569, 659, 750, 844, 939, 1037, 1138, 1243, 1353, 1468, 1591, 1722, 1865, 2020};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-1329, -1232, -1145, -1066, -993, -926, -863, -802, -744, -687, -630, -574, -516, -458, -399, -337, -274, -209, -142, -72, 0, 74, 151, 230, 312, 396, 483, 573, 665, 761, 860, 963, 1070, 1181, 1298, 1422, 1551, 1689};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-1585, -1457, -1346, -1248, -1161, -1082, -1010, -942, -878, -815, -754, -693, -632, -569, -506, -441, -374, -306, -236, -165, -92, -18, 58, 135, 213, 292, 373, 456, 542, 630, 722, 818, 920, 1029, 1146, 1274, 1415, 1570};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
#endif // #if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_50MA
#if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_100MA
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {549, 287, 77, -88, -212, -302, -363, -399, -414, -411, -393, -362, -322, -274, -218, -157, -92, -23, 50, 125, 204, 285, 369, 455, 545, 638, 735, 834, 937, 1044, 1153, 1266, 1380, 1496, 1612, 1727, 1839, 1948};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-470, -604, -705, -778, -827, -855, -864, -858, -839, -809, -770, -724, -671, -613, -550, -484, -415, -344, -270, -194, -116, -35, 47, 132, 220, 311, 405, 502, 602, 706, 813, 923, 1037, 1153, 1271, 1391, 1512, 1632};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-763, -831, -882, -917, -938, -946, -944, -932, -911, -882, -846, -804, -756, -703, -645, -584, -518, -449, -376, -300, -221, -139, -53, 35, 126, 219, 316, 415, 517, 621, 727, 836, 946, 1057, 1170, 1282, 1395, 1507};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-935, -970, -995, -1010, -1016, -1013, -1003, -985, -960, -929, -893, -851, -803, -752, -695, -634, -570, -501, -429, -353, -274, -192, -106, -18, 73, 167, 264, 363, 464, 567, 672, 779, 887, 997, 1107, 1218, 1329, 1439};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-1163, -1170, -1171, -1165, -1152, -1134, -1111, -1082, -1048, -1010, -967, -920, -869, -814, -756, -693, -628, -559, -487, -412, -334, -253, -169, -83, 6, 98, 192, 288, 387, 487, 590, 695, 802, 910, 1020, 1132, 1244, 1358};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-1362, -1359, -1348, -1330, -1305, -1274, -1238, -1197, -1151, -1102, -1049, -993, -934, -873, -809, -742, -674, -603, -531, -456, -379, -300, -219, -136, -51, 36, 126, 219, 314, 412, 512, 615, 721, 830, 943, 1058, 1176, 1297};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-1905, -1859, -1809, -1753, -1695, -1633, -1569, -1503, -1436, -1367, -1297, -1226, -1155, -1083, -1010, -937, -863, -787, -711, -634, -555, -474, -392, -308, -222, -133, -42, 51, 147, 245, 346, 449, 555, 662, 771, 882, 994, 1106};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-2470, -2317, -2180, -2057, -1946, -1845, -1752, -1665, -1584, -1507, -1433, -1361, -1290, -1220, -1149, -1077, -1004, -930, -853, -775, -694, -612, -526, -439, -349, -258, -164, -69, 28, 126, 225, 326, 427, 529, 631, 734, 837, 941};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
#endif // #if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_100MA
#if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_150MA
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {476, 251, 58, -106, -244, -357, -449, -520, -573, -611, -633, -643, -641, -629, -609, -580, -544, -503, -456, -405, -351, -293, -232, -169, -104, -37, 32, 102, 174, 247, 322, 399, 478, 558, 641, 727, 816, 907};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-306, -464, -596, -704, -793, -862, -915, -953, -978, -991, -994, -988, -973, -951, -922, -888, -848, -804, -756, -704, -649, -590, -529, -465, -399, -330, -260, -187, -113, -38, 39, 116, 194, 272, 350, 427, 502, 575};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-492, -607, -707, -792, -864, -924, -971, -1006, -1031, -1045, -1051, -1047, -1036, -1017, -991, -959, -921, -878, -831, -780, -725, -666, -605, -542, -476, -409, -340, -269, -197, -123, -48, 28, 105, 185, 266, 349, 436, 525};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-625, -720, -803, -875, -935, -985, -1024, -1054, -1074, -1085, -1088, -1083, -1071, -1051, -1026, -994, -957, -916, -869, -819, -765, -709, -649, -587, -522, -456, -387, -318, -246, -173, -98, -22, 57, 137, 221, 307, 396, 490};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-845, -922, -986, -1038, -1081, -1113, -1136, -1152, -1159, -1159, -1153, -1140, -1122, -1099, -1071, -1038, -1001, -960, -916, -868, -817, -763, -707, -647, -585, -520, -453, -384, -312, -239, -162, -84, -4, 78, 163, 249, 338, 428};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-1068, -1133, -1183, -1219, -1244, -1258, -1264, -1263, -1255, -1242, -1224, -1201, -1175, -1146, -1114, -1079, -1041, -1001, -957, -912, -863, -812, -758, -701, -641, -578, -511, -442, -370, -295, -217, -137, -55, 28, 111, 194, 275, 355};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-1501, -1502, -1501, -1498, -1492, -1483, -1472, -1458, -1440, -1420, -1397, -1370, -1340, -1307, -1271, -1232, -1190, -1146, -1098, -1048, -995, -940, -883, -823, -762, -698, -633, -566, -498, -428, -357, -284, -210, -136, -60, 18, 96, 175};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-1790, -1791, -1784, -1769, -1747, -1720, -1688, -1653, -1615, -1574, -1531, -1487, -1441, -1395, -1349, -1301, -1254, -1205, -1157, -1107, -1057, -1007, -955, -901, -846, -790, -731, -670, -605, -538, -468, -394, -316, -234, -148, -57, 38, 138};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
#endif // #if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_150MA
#if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_250MA
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {1007, 754, 530, 332, 159, 10, -117, -224, -311, -381, -434, -473, -498, -511, -513, -505, -489, -466, -435, -400, -360, -316, -269, -219, -168, -115, -61, -6, 50, 107, 165, 224, 285, 348, 414, 484, 559, 639};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {308, 39, -184, -366, -514, -632, -724, -793, -844, -880, -901, -912, -914, -908, -897, -879, -858, -833, -804, -773, -739, -702, -662, -619, -574, -525, -473, -419, -361, -300, -236, -169, -101, -32, 37, 105, 171, 233};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {90, -128, -314, -473, -607, -717, -807, -879, -933, -974, -1001, -1017, -1023, -1020, -1010, -993, -971, -943, -912, -877, -840, -800, -757, -712, -666, -618, -567, -515, -461, -404, -345, -283, -218, -149, -77, 1, 83, 171};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-64, -247, -408, -548, -668, -769, -853, -922, -975, -1016, -1044, -1062, -1069, -1068, -1059, -1043, -1021, -994, -963, -928, -889, -848, -805, -760, -714, -665, -615, -564, -511, -456, -398, -337, -273, -203, -129, -48, 41, 139};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-301, -443, -570, -681, -778, -860, -930, -987, -1032, -1067, -1091, -1106, -1113, -1112, -1104, -1089, -1069, -1044, -1014, -981, -944, -905, -863, -818, -772, -724, -673, -621, -567, -511, -452, -390, -324, -254, -179, -97, -7, 91};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-518, -637, -742, -831, -907, -971, -1023, -1065, -1098, -1122, -1138, -1146, -1148, -1144, -1135, -1121, -1102, -1078, -1051, -1021, -986, -949, -909, -866, -820, -771, -720, -666, -610, -550, -488, -423, -354, -283, -208, -130, -48, 38};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-874, -988, -1082, -1160, -1223, -1272, -1309, -1336, -1352, -1361, -1361, -1355, -1343, -1326, -1305, -1279, -1251, -1219, -1184, -1146, -1107, -1065, -1021, -976, -928, -879, -827, -774, -718, -660, -600, -537, -472, -405, -334, -261, -185, -106};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
const int16_t PROGMEM LTC3335_Software_Correction_Table[LTC3335_VBAT_NUM] = {-1252, -1277, -1299, -1319, -1335, -1349, -1358, -1364, -1366, -1365, -1359, -1350, -1337, -1321, -1302, -1279, -1254, -1227, -1197, -1165, -1131, -1095, -1058, -1019, -978, -936, -892, -846, -798, -747, -693, -636, -574, -507, -434, -354, -265, -167};
#endif // #if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
#endif // #if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_250MA
#endif // #if LTC3335_USE_SOFTWARE_CORRECTION

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Prototypes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//! Returns the software correction factor for a specified LTC3335_IPEAK_CONFIGURATION, LTC3335_OUTPUT_VOLTAGE, and battery voltage.
//! Note! - These corrections factors are only valid at room temperature and for the recommended Coilcraft LPS5030-xxxMRB inductor.
//! @return TRUE if the LTC3335 communication was successful.
int16_t LTC3335_Get_Software_Correction_Factor(uint16_t vbat)
{
  if (vbat <= LTC3335_VBAT_MIN)
  {
    return pgm_read_word(&LTC3335_Software_Correction_Table[LTC3335_VBAT_TO_TABLE_INDEX(LTC3335_VBAT_MIN)]);
  }
  else if (vbat >= LTC3335_VBAT_MAX)
  {
    return pgm_read_word(&LTC3335_Software_Correction_Table[LTC3335_VBAT_TO_TABLE_INDEX(LTC3335_VBAT_MAX)]);
  }
  else
  {
    uint8_t index = LTC3335_VBAT_TO_TABLE_INDEX(vbat);
    int16_t factor1 = pgm_read_word(&LTC3335_Software_Correction_Table[index]);
    int16_t factor2 = pgm_read_word(&LTC3335_Software_Correction_Table[index + 1]);
    return factor1 + ((int32_t) (factor2 - factor1) * (vbat - (LTC3335_VBAT_MIN + index * LTC3335_VBAT_STEP))  + LTC3335_VBAT_STEP/2) / (LTC3335_VBAT_STEP);
  }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


