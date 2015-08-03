#ifndef _VELIB_VECAN_PRODUCTS_H_
#define _VELIB_VECAN_PRODUCTS_H_

#include <velib/base/base.h>

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_PRODUCTS	Product IDs
 */
/// @{

/* !! DO NOT ADD NEW DEVICES BELOW 0xA000, RESERVED FOR VEBUS, to keep victron products ids unique !! */

// VE.Net products, taken from lib - venet_app_pri/venet_constants
#define VE_PROD_ID_VENET_VPN					0x0001	// VE.Net Panel
#define VE_PROD_ID_VENET_VBC					0x0002	// VE.Net Battery Controller
#define VE_PROD_ID_VENET_VVC					0x0003	// VE.Net to VE 9bit RS485 Converter (formerly known as VVU)
#define VE_PROD_ID_VENET_VCC					0x0004	// VE.Net VE.Net DC Converter
#define VE_PROD_ID_VENET_VCM					0x0005	// VE.Net Connectivity Module
#define VE_PROD_ID_VENET_VGM					0x0006	// VE.Net Generator Module
#define VE_PROD_ID_VENET_VRS					0x0007	// VE.Net Resistive Sensor
#define VE_PROD_ID_VENET_FT						0x0008	// Free Technics software
#define VE_PROD_ID_VENET_VBC_GMDSS				0x0009	// VE.Net Battery controller with GMDSS Software
#define VE_PROD_ID_VENET_VPN_GMDSS				0x000A	// VE.Net Panel with GMDSS Software
#define VE_PROD_ID_VENET_BPP					0x000B	// VE.Net Blue Power Panel
#define VE_PROD_ID_VENET_VBC_48					0x000C	// VE.Net Battery Controller (48V version)
#define VE_PROD_ID_VENET_VVC2					0x000D	// VE.Net to VE.Bus Converter
#define VE_PROD_ID_VENET_VBC_144				0x000E	// VE.Net Battery Controller (144V version)
#define VE_PROD_ID_VENET_VBC2					0x000F	// VE.Net Battery Controller (Combined 12/24/48V version)
#define VE_PROD_ID_VENET_VBC2_144				0x0010	// VE.Net Battery Controller (V2 hardware of the 144V model)
#define VE_PROD_ID_VENET_LYNX					0x0011	// VE.Net Lynx shunt

// VE.Can products (note add new products above 0xA000!)
#define VE_PROD_ID_VECAN_BMV					0x0100	// BMV CAN converter
#define VE_PROD_ID_VECAN_SRP					0x0101	// Skylla-i Remote Panel
#define VE_PROD_ID_VECAN_VHP					0x0102	// Hybrid Panel
#define VE_PROD_ID_VECAN_MK2NMEA				0x0103	// MK2NMEA - vebus to NMEA 2000
#define VE_PROD_ID_VECAN_SKYLLA_I_24V_100A_1OUT	0x0104	// Skylla-i charger 24V 100A 1+1 output
#define VE_PROD_ID_VECAN_SKYLLA_I_24V_80A_1OUT	0x0105	// Skylla-i charger 24V 80A 1+1 output
#define VE_PROD_ID_VECAN_SKYLLA_I_24V_100A_3OUT	0x0106	// Skylla-i charger 24V 100A 3 outputs
#define VE_PROD_ID_VECAN_SKYLLA_I_24V_80A_3OUT	0x0107	// Skylla-i charger 24V 80A 3 outputs
#define VE_PROD_ID_VECAN_BLUE_SOLAR_MPPT_70A	0x0108	// BlueSolar Charger MPPT 150/70 (1x 12V..48V 70A charge output / 1x 145V 50A panel input)
/* Reserved space */
#define VE_PROD_ID_VECAN_LIC					0x0140	// Ion Controller
#define VE_PROD_ID_VECAN_LYNX_SHUNT				0x0141	// Lynx Shunt
#define VE_PROD_ID_VECAN_LYNX_ION				0x0142	// Lynx Ion (MG Electronics)

#define VE_PROD_ID_BMV600S						0x0200
#define VE_PROD_ID_BMV602S						0x0201
#define VE_PROD_ID_BMV600HS						0x0202
#define VE_PROD_ID_BMV700						0x0203
#define VE_PROD_ID_BMV702						0x0204
#define VE_PROD_ID_BMV700H						0x0205
#define VE_PROD_ID_BMVTEST						0x0211

#define VE_PROD_ID_BLUE_SOLAR_MPPT_15A			VE_PROD_ID_BLUE_SOLAR_MPPT_70_15
#define VE_PROD_ID_BLUE_SOLAR_MPPT_70_15		0x0300	// BlueSolar Charger MPPT 70/15 (1x 12V..24V 15A charge output / 1x 75V panel input / 1x 15A load output / 16kB micro)

/* === this is a automatically generated === */
/*
 * note: Vebus itself uses decimal number, the BCD version is used to
 * let the velib based tools print the number as mentioned on the product
 */
#define VE_PROD_ID_VEBUS_PHOENIX_HF_9915                                                0x2101
#define VE_PROD_ID_VEBUS_REMOTE_PANEL_9912                                              0x1110
#define VE_PROD_ID_VEBUS_REMOTE_PANEL_20MHZ_9915                                        0x1120
#define VE_PROD_ID_VEBUS_REMOTE_PANEL_DONGLE_20MHZ_9915                                 0x1121
#define VE_PROD_ID_VEBUS_MK2_20MHZ_9915                                                 0x1130
#define VE_PROD_ID_VEBUS_MK2_DONGLE_20MHZ_9915                                          0x1131
#define VE_PROD_ID_VEBUS_VE_BUS_RESETTER__BASED_ON_MK2__20_MHZ_9915                     0x1132
#define VE_PROD_ID_VEBUS_MK2_TACHOMETER_20MHZ_9916                                      0x1140
#define VE_PROD_ID_VEBUS_SOLAR_SWITCH_BOX_9918                                          0x1150
#define VE_PROD_ID_VEBUS_USB_VEBUS_GRABBER_9919                                         0x1160
#define VE_PROD_ID_VEBUS_PHOENIX_CHARGER_12_100_9911                                    0x1421
#define VE_PROD_ID_VEBUS_PHOENIX_CHARGER_12_200_9911                                    0x1422
#define VE_PROD_ID_VEBUS_PHOENIX_CHARGER_24_50_9911                                     0x1431
#define VE_PROD_ID_VEBUS_PHOENIX_CHARGER_24_100_9911                                    0x1432
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_12V_FULL_POWER_9901                              0x1700
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_12V_HALF_POWER_9901                              0x1701
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_12V_FULL_POWER_9901                          0x1702
#define VE_PROD_ID_VEBUS_MULTICOMPACT_12V_LOW_POWER_9900                                0x1703
#define VE_PROD_ID_VEBUS_MULTICOMPACT_12V_MEDIUM_POWER_9900                             0x1704
#define VE_PROD_ID_VEBUS_MULTICOMPACT_12V_HIGH_POWER_9900                               0x1705
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_12V_HIGH_POWER_9900                           0x1706
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_12V_MEDIUM_POWER_9900                         0x1707
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_24V_FULL_POWER_9901                              0x1710
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_24V_HALF_POWER_9901                              0x1711
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_24V_FULL_POWER_9901                          0x1712
#define VE_PROD_ID_VEBUS_MULTICOMPACT_24V_LOW_POWER_9900                                0x1713
#define VE_PROD_ID_VEBUS_MULTICOMPACT_24V_MEDIUM_POWER_9900                             0x1714
#define VE_PROD_ID_VEBUS_MULTICOMPACT_24V_HIGH_POWER_9900                               0x1715
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_24V_HIGH_POWER_9900                           0x1716
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_24V_MEDIUM_POWER_9900                         0x1717
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_120_12V_FULL_POWER_9901                          0x1730
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_120_12V_HALF_POWER_9901                          0x1731
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_12V_FULL_POWER_9901                      0x1732
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_120_24V_FULL_POWER_9901                          0x1740
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_120_24V_HALF_POWER_9901                          0x1741
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_24V_FULL_POWER_9901                      0x1742
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_48V_FULL_POWER_9901                              0x1750
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_48V_HALF_POWER_9901                              0x1751
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_48V_FULL_POWER_9901                          0x1752
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_12V_FULL_POWER_9906                              0x1800
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_12V_HALF_POWER_9906                              0x1801
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_12V_FULL_POWER_9906                          0x1802
#define VE_PROD_ID_VEBUS_MULTICOMPACT_12V_LOW_POWER_9902                                0x1803
#define VE_PROD_ID_VEBUS_MULTICOMPACT_12V_MEDIUM_POWER_9902                             0x1804
#define VE_PROD_ID_VEBUS_MULTICOMPACT_12V_HIGH_POWER_9902                               0x1805
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_12V_HIGH_POWER_9902                           0x1806
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_12V_MEDIUM_POWER_9902                         0x1807
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_12V_2000_9906                                0x1808
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_12V_3000_9906                                0x1809
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_24V_FULL_POWER_9906                              0x1810
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_24V_HALF_POWER_9906                              0x1811
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_24V_FULL_POWER_9906                          0x1812
#define VE_PROD_ID_VEBUS_MULTICOMPACT_24V_LOW_POWER_9902                                0x1813
#define VE_PROD_ID_VEBUS_MULTICOMPACT_24V_MEDIUM_POWER_9902                             0x1814
#define VE_PROD_ID_VEBUS_MULTICOMPACT_24V_HIGH_POWER_9902                               0x1815
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_24V_HIGH_POWER_9902                           0x1816
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_24V_MEDIUM_POWER_9902                         0x1817
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_24V_2000_9906                                0x1818
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_120_12V_FULL_POWER_9906                          0x1830
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_120_12V_HALF_POWER_9906                          0x1831
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_12V_FULL_POWER_9906                      0x1832
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_120_12V_HIGH_POWER_9906                       0x1836
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_12V_3000_9906                            0x1839
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_120_24V_FULL_POWER_9906                          0x1840
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_120_24V_HALF_POWER_9906                          0x1841
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_24V_FULL_POWER_9906                      0x1842
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_120_24V_HIGH_POWER_9906                       0x1846
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_48V_FULL_POWER_9906                              0x1850
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_48V_HALF_POWER_9906                              0x1851
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_48V_FULL_POWER_9906                          0x1852
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_48V_HIGH_POWER_9906                           0x1856
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_48V_FULL_POWER_9906                      0x1862
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_12V_2K_9906                                   0x1871
#define VE_PROD_ID_VEBUS_MULTICOMPACTPLUS_24V_2K_9906                                   0x1872
#define VE_PROD_ID_VEBUS_MULTIPLUS_12_3000_120_50_9906                                  0x1900
#define VE_PROD_ID_VEBUS_MULTIPLUS_12_3000_120_30_9906                                  0x1901
//#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_12V_FULL_POWER_9906                          0x1902
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_COMPACT_12_800_35_16_9906                        0x1903
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_12_1600_70_16_9906                           0x1906
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_12_1200_50_16_9906                           0x1907
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_12_2000_80_30_9906                           0x1908
#define VE_PROD_ID_VEBUS_MULTIPLUS_12_3000_120_16_9906                                  0x1909
#define VE_PROD_ID_VEBUS_MULTIPLUS_24_3000_70_50_9906                                   0x1910
#define VE_PROD_ID_VEBUS_MULTIPLUS_24_3000_70_30_9906                                   0x1911
#define VE_PROD_ID_VEBUS_MULTIPLUS_24_3000_70_16_9906                                   0x1912
#define VE_PROD_ID_VEBUS_PHOENIX_MULTI_COMPACT_24_800_16_16_9906                        0x1913
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_24_1600_40_16_9906                           0x1916
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_24_1200_25_16_9906                           0x1917
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_24_2000_50_30_9906                           0x1918
#define VE_PROD_ID_VEBUS_MULTIPLUS_48_3000_35_50_9906                                   0x1920
#define VE_PROD_ID_VEBUS_MULTIPLUS_48_3000_35_30_9906                                   0x1921
#define VE_PROD_ID_VEBUS_MULTIPLUS_48_3000_35_16_9906                                   0x1922
#define VE_PROD_ID_VEBUS_QUATTRO_12_5000_200_2X30_9906                                  0x1930
#define VE_PROD_ID_VEBUS_QUATTRO_12_3000_120_50_30_9906                                 0x1931
#define VE_PROD_ID_VEBUS_QUATTRO_12_5000_200_50_30_9906                                 0x1932
#define VE_PROD_ID_VEBUS_QUATTRO_12_5000_220_2X75_9906                                  0x1933
#define VE_PROD_ID_VEBUS_QUATTRO_24_5000_120_2X30_9906                                  0x1940
#define VE_PROD_ID_VEBUS_QUATTRO_24_3000_70_50_30_9906                                  0x1941
#define VE_PROD_ID_VEBUS_QUATTRO_24_5000_120_50_30_9906                                 0x1942
#define VE_PROD_ID_VEBUS_QUATTRO_24_8000_200_2X100_9906                                 0x1943
#define VE_PROD_ID_VEBUS_QUATTRO_24_5000_120_2X100_9906                                 0x1948
#define VE_PROD_ID_VEBUS_MULTIPLUS_24_5000_120_50_9906                                  0x1949
#define VE_PROD_ID_VEBUS_QUATTRO_48_5000_70_2X30_9906                                   0x1950
#define VE_PROD_ID_VEBUS_QUATTRO_48_5000_70_50_30_9906                                  0x1952
#define VE_PROD_ID_VEBUS_QUATTRO_48_10000_140_2X100_9906                                0x1953
#define VE_PROD_ID_VEBUS_QUATTRO_48_8000_110_2X100_9906                                 0x1954
#define VE_PROD_ID_VEBUS_QUATTRO_48_5000_70_2X100_9906                                  0x1958
#define VE_PROD_ID_VEBUS_MULTIPLUS_48_5000_70_50_9906                                   0x1959
#define VE_PROD_ID_VEBUS_MULTIPLUS_12_3000_120_50_120V_9906                             0x2002
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_12_2000_80_50_120V_9906                      0x2008
//#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_12V_3000_9906                            0x2009
#define VE_PROD_ID_VEBUS_MULTIPLUS_24_3000_70_50_120V_9906                              0x2012
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_24_2000_50_50_120V_9906                      0x2018
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_48V_3000_9906                            0x2022
#define VE_PROD_ID_VEBUS_QUATTRO_24_5000_120_2X100_120V_9906                            0x2048
#define VE_PROD_ID_VEBUS_QUATTRO_48_3000_35_2X50_120V_9906                              0x2051
#define VE_PROD_ID_VEBUS_QUATTRO_48_5000_70_2X100_120V_9906                             0x2053
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_48V_5000_9906                            0x2059
#define VE_PROD_ID_VEBUS_PHOENIX_12_3000_120_50_120_240V_9906                           0x2060
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_12_2000_80_50_30__120_240V_9906              0x2062
#define VE_PROD_ID_VEBUS_QUATTRO_24_5000_120_2X60_120_240V_9906                         0x2071
#define VE_PROD_ID_VEBUS_MULTIPLUS_COMPACT_24_2000_50_50_30__120_240V_9906              0x2072
#define VE_PROD_ID_VEBUS_PHOENIX_MULTIPLUS_120_240_48V_3000_WITH_50A_AC_RELAIS_9906     0x2080
#define VE_PROD_ID_VEBUS_PHOENIX_12_800                                                 0x2403
#define VE_PROD_ID_VEBUS_PHOENIX_12_1200                                                0x2404
#define VE_PROD_ID_VEBUS_PHOENIX_24_800                                                 0x2413
#define VE_PROD_ID_VEBUS_PHOENIX_24_1200                                                0x2414
#define VE_PROD_ID_VEBUS_PHOENIX_48_800                                                 0x2423
#define VE_PROD_ID_VEBUS_PHOENIX_48_1200                                                0x2424
#define VE_PROD_ID_VEBUS_QUATTRO_12_3000_120_50_30                                      0x2631
#define VE_PROD_ID_VEBUS_QUATTRO_24_3000_70_50_30                                       0x2641
/* === end of generated list === */

/* Reserved space for Skylla-i chargers 0xA000..0xA03F: 64 items */
#define VE_PROD_ID_VECAN_SKYLLA_IP65						0xA000 // Skylla-IP65 control board (generic family id / bootloader mode)
#define VE_PROD_ID_VECAN_SKYLLA_IP65_12V_60A_1OUT_230V		0xA001 // Skylla-IP65 12V/60A/1 output, input voltage 180-265VAC/45-65Hz

/* Reserved space for Solar chargers 0xA040..0xA07F: 64 items */
#define	VE_PROD_ID_BLUE_SOLAR_MPPT_50A				VE_PROD_ID_BLUE_SOLAR_MPPT_75_50
#define VE_PROD_ID_BLUE_SOLAR_MPPT_75_50			0xA040	// BlueSolar Charger MPPT 75/50 (1x 12V..24V 50A charge output / 1x 75V panel input / 32kB micro)
#define	VE_PROD_ID_BLUE_SOLAR_MPPT_35A				VE_PROD_ID_BLUE_SOLAR_MPPT_150_35
#define VE_PROD_ID_BLUE_SOLAR_MPPT_150_35			0xA041	// BlueSolar Charger MPPT 150/35 (1x 12V..48V 35A charge output / 1x 150V panel input / 32kB micro)
#define	VE_PROD_ID_BLUE_SOLAR_MPPT_15A_MK2			VE_PROD_ID_BLUE_SOLAR_MPPT_75_15
#define VE_PROD_ID_BLUE_SOLAR_MPPT_75_15			0xA042	// BlueSolar Charger MPPT 75/15 (1x 12V..24V 15A charge output / 1x 75V panel input / 1x 15A load output / 32kB micro)
#define	VE_PROD_ID_BLUE_SOLAR_MPPT_15A_MK3			VE_PROD_ID_BLUE_SOLAR_MPPT_100_15
#define VE_PROD_ID_BLUE_SOLAR_MPPT_100_15			0xA043	// BlueSolar Charger MPPT 100/15 (1x 12V..24V 15A charge output / 1x 100V panel input / 1x 15A load output / 32kB micro)
#define	VE_PROD_ID_BLUE_SOLAR_MPPT_30A				VE_PROD_ID_BLUE_SOLAR_MPPT_100_30
#define VE_PROD_ID_BLUE_SOLAR_MPPT_100_30			0xA044	// BlueSolar Charger MPPT 100/30 (1x 12V..24V 30A charge output / 1x 100V panel input / 32kB micro)
#define	VE_PROD_ID_BLUE_SOLAR_MPPT_50A_MK2			VE_PROD_ID_BLUE_SOLAR_MPPT_100_50
#define VE_PROD_ID_BLUE_SOLAR_MPPT_100_50			0xA045	// BlueSolar Charger MPPT 100/50 (1x 12V..24V 50A charge output / 1x 100V panel input / 32kB micro)
#define	VE_PROD_ID_BLUE_SOLAR_MPPT_70A_MINI			VE_PROD_ID_BLUE_SOLAR_MPPT_150_70
#define VE_PROD_ID_BLUE_SOLAR_MPPT_150_70			0xA046	// BlueSolar Charger MPPT 150/70 (1x 12V..48V 70A charge output / 1x 150V panel input / 60kB micro)
#define VE_PROD_ID_BLUE_SOLAR_MPPT_150_100			0xA047	// BlueSolar Charger MPPT 150/100 (1x 12V..48V 100A charge output / 1x 150V panel input / 60kB micro)
#define VE_PROD_ID_BLUE_SOLAR_MPPT_75_50_REV2		0xA048	// BlueSolar Charger MPPT 75/50 (1x 12V..24V 50A charge output / 1x 75V panel input / 60kB micro)
#define VE_PROD_ID_BLUE_SOLAR_MPPT_100_50_REV2		0xA049	// BlueSolar Charger MPPT 100/50 (1x 12V..24V 50A charge output / 1x 100V panel input / 60kB micro)
#define VE_PROD_ID_BLUE_SOLAR_MPPT_100_30_REV2		0xA04A	// BlueSolar Charger MPPT 100/30 (1x 12V..24V 30A charge output / 1x 100V panel input / 60kB micro)
#define VE_PROD_ID_BLUE_SOLAR_MPPT_150_35_REV2		0xA04B	// BlueSolar Charger MPPT 150/35 (1x 12V..48V 35A charge output / 1x 150V panel input / 60kB micro)
#define VE_PROD_ID_BLUE_SOLAR_MPPT_75_10			0xA04C	// BlueSolar Charger MPPT 75/10 (1x 12V..24V 10A charge output / 1x 75V panel input / 1x 10A load output / 32kB micro)
#define VE_PROD_ID_BLUE_SOLAR_MPPT_150_45			0xA04D	// BlueSolar Charger MPPT 150/45 (1x 12V..48V 45A charge output / 1x 150V panel input / 60kB micro)
#define VE_PROD_ID_BLUE_SOLAR_MPPT_150_60			0xA04E	// BlueSolar Charger MPPT 150/60 (1x 12V..48V 60A charge output / 1x 150V panel input / 60kB micro)

// VE.Can products (continued)
#define VE_PROD_ID_VECAN_BSRP   				0xA100	// BlueSolar Remote Panel
/* Reserved space for Solar chargers 0xA101..0xA11F: 31 items */
#define VE_PROD_ID_VECAN_BLUE_SOLAR_MPPT_85A	0xA101	// BlueSolar Charger MPPT 150/85 (1x 12V..48V 85A charge output / 1x 145V 75A panel input)

#define VE_PROD_ID_VECAN_MK2NMEA_REV2			0xA120	// MK2NMEA - vebus to NMEA 2000 (version 2)
#define VE_PROD_ID_VECAN_VEDIRECT				0xA121	// VE.Direct CAN converter (version 2)

#define VE_PROD_ID_VECAN_LYNX_ION_SHUNT			0xA130	// Lynx Ion + Shunt (MG Electronics)

/* Photovoltaic Inverters, 0xA140..0xA15F  */
#define VE_PROD_ID_PV_INVERTER_QWACS 			0xA140	// Measured by Quby sensors
#define VE_PROD_ID_PV_INVERTER_VEBUS 			0xA141	// Measured by VE.Bus AC current sensors
#define VE_PROD_ID_PV_INVERTER_FRONIUS			0xA142	// Fronius solar inverters

#define VE_PROD_ID_TANK_SENSOR					0xA160	// General tank sensor

/* VE.Direct remote panels, 0xA170 .. 0xA17F: 16 items */
#define VE_PROD_ID_MPPT_CONTROL					0xA170	// VE.Direct MPPT control
#define VE_PROD_ID_MPPT_CONTROL_TEST			0xA171	// VE.Direct MPPT control test firmware

/* VE.Direct products */
#define VE_PROD_ID_VEDIRECT_PPP					0xA180	// Peak power pack

/* Phoenix inverters (new models with VE.Direct bus) */
#define VE_PROD_ID_PHOENIX_INVERTER				0xA200  // unspecified spec
#define VE_PROD_ID_PHOENIX_INVERTER_350VA		0xA201  // e.g.
/**/

#define VE_PROD_ID_VALENCE_XP_BATTERY			0xB000
#define VE_PROD_ID_VALENCE_BMS					0xB001
#define VE_PROD_ID_CARLO_GAVAZZI_EM				0xB002

#define VE_PROD_RESERVED						0xFFFE
#define VE_PROD_NOT_SET							0xFFFF

VE_DCL	const char *veProductGetName(un16 prodId);

/// @}

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_GROUPS	Group IDs
 */
/// @{

/// Solar battery chargers.
#define VE_GROUP_SOLAR_CHARGERS		10
/// Grid connected battery chargers (with input current limiting capabilities).
#define VE_GROUP_GRID_CHARGERS		20
/// Batteries
#define VE_GROUP_BATTERY			30

#define VE_GROUP_RESERVED			0xFE
#define VE_GROUP_NOT_SET			0xFF

/// @}

/**
 * @ingroup VELIB_VECAN
 * @defgroup VELIB_VECAN_CHARGER_ALGORTIHM_VERSIONS	Charger Algorithm Versions (parallel charging)
 */
/// @{
/// Charger algorithm version Blue Solar.
#define VE_CHARGER_ALGORITHM_VERSION_MPPT			10
/// Charger algorithm versions Skylla-i. Note that the triple output must have higher priority since
/// it needs to report the status of the additional outputs (single output lacks this functionality).
#define VE_CHARGER_ALGORITHM_VERSION_SCB_1_OUTPUT	20
#define VE_CHARGER_ALGORITHM_VERSION_SCB_3_OUTPUTS	25

#define VE_CHARGER_ALGORITHM_VERSION_RESERVED		0xFE
#define VE_CHARGER_ALGORITHM_VERSION_NOT_SET		0xFF

/// @}


#endif
