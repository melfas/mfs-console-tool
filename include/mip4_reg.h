/*
 * MELFAS MIP4 Input Device
 *
 * Copyright (C) 2000-2018 MELFAS Inc.
 *
 *
 * mip4_reg.h : Protocol information
 *
 * Version : 2018.11.26
 *
 * Protocol Version : MIP4 Version 6.0.4
 */

/* Address */
#define MIP4_R0_BOOT                        0x00
#define MIP4_R1_BOOT_MODE                   0x01
#define MIP4_R1_BOOT_BUF_ADDR               0x10
#define MIP4_R1_BOOT_STATUS                 0x20
#define MIP4_R1_BOOT_CMD                    0x30
#define MIP4_R1_BOOT_TARGET_ADDR            0x40
#define MIP4_R1_BOOT_SIZE                   0x44

#define MIP4_R0_INFO                        0x01
#define MIP4_R1_INFO_PRODUCT_NAME           0x00
#define MIP4_R1_INFO_RESOLUTION_X           0x10
#define MIP4_R1_INFO_RESOLUTION_Y           0x12
#define MIP4_R1_INFO_NODE_NUM_X             0x14
#define MIP4_R1_INFO_NODE_NUM_Y             0x15
#define MIP4_R1_INFO_KEY_NUM                0x16
#define MIP4_R1_INFO_PRESSURE_NUM           0x17
#define MIP4_R1_INFO_LENGTH_X               0x18
#define MIP4_R1_INFO_LENGTH_Y               0x1A
#define MIP4_R1_INFO_VERSION_BOOT           0x20
#define MIP4_R1_INFO_VERSION_FW             0x22
#define MIP4_R1_INFO_SECT_BOOT_START        0x30
#define MIP4_R1_INFO_SECT_BOOT_END          0x31
#define MIP4_R1_INFO_SECT_FW_START          0x32
#define MIP4_R1_INFO_SECT_FW_END            0x33
#define MIP4_R1_INFO_SECT_3_START           0x34
#define MIP4_R1_INFO_SECT_3_END             0x35
#define MIP4_R1_INFO_SECT_4_START           0x36
#define MIP4_R1_INFO_SECT_4_END             0x37
#define MIP4_R1_INFO_BUILD_DATE             0x40
#define MIP4_R1_INFO_BUILD_TIME             0x44
#define MIP4_R1_INFO_CHECKSUM_PRECALC       0x48
#define MIP4_R1_INFO_CHECKSUM_REALTIME      0x4A
#define MIP4_R1_INFO_PROTOCOL_NAME          0x50
#define MIP4_R1_INFO_PROTOCOL_VERSION       0x58
#define MIP4_R1_INFO_IC_ID                  0x70
#define MIP4_R1_INFO_IC_NAME                0x71
#define MIP4_R1_INFO_IC_VENDOR_ID           0x75
#define MIP4_R1_INFO_IC_HW_CATEGORY         0x77
#define MIP4_R1_INFO_CONTACT_THD_SCR        0x78
#define MIP4_R1_INFO_CONTACT_THD_KEY        0x7A
#define MIP4_R1_INFO_PID                    0x7C
#define MIP4_R1_INFO_VID                    0x7E
#define MIP4_R1_INFO_SLAVE_ADDR             0x80
#define MIP4_R1_INFO_PROXIMITY_THD          0x8E

#define MIP4_R0_EVENT                       0x02
#define MIP4_R1_EVENT_SUPPORTED_FUNC        0x00
#define MIP4_R1_EVENT_FORMAT                0x04
#define MIP4_R1_EVENT_SIZE                  0x06
#define MIP4_R1_EVENT_PACKET_INFO           0x10
#define MIP4_R1_EVENT_PACKET_DATA           0x11

#define MIP4_R0_CTRL                        0x06
#define MIP4_R1_CTRL_READY_STATUS           0x00
#define MIP4_R1_CTRL_EVENT_READY            0x01
#define MIP4_R1_CTRL_MODE                   0x10
#define MIP4_R1_CTRL_EVENT_TRIGGER_TYPE     0x11
#define MIP4_R1_CTRL_RECALIBRATE            0x12
#define MIP4_R1_CTRL_POWER_STATE            0x13
#define MIP4_R1_CTRL_GESTURE_TYPE           0x14
#define MIP4_R1_CTRL_DISABLE_ESD_ALERT      0x18
#define MIP4_R1_CTRL_CHARGER_MODE           0x19
#define MIP4_R1_CTRL_HIGH_SENS_MODE         0x1A
#define MIP4_R1_CTRL_WINDOW_MODE            0x1B
#define MIP4_R1_CTRL_PALM_REJECTION         0x1C
#define MIP4_R1_CTRL_EDGE_CORRECTION        0x1D
#define MIP4_R1_CTRL_ENTER_GLOVE_MODE       0x1E
#define MIP4_R1_CTRL_I2C_ON_LPM             0x1F
#define MIP4_R1_CTRL_GESTURE_DEBUG          0x20
#define MIP4_R1_CTRL_PALM_EVENT             0x22
#define MIP4_R1_CTRL_PROXIMITY_SENSING      0x23
#define MIP4_R1_CTRL_REBASELINE_KEY         0x24
#define MIP4_R1_CTRL_REBASELINE_GRIP        0x25
#define MIP4_R1_CTRL_ENABLE_SCREEN          0x26
#define MIP4_R1_CTRL_ENABLE_KEY             0x27
#define MIP4_R1_CTRL_ENABLE_GRIP            0x28
#define MIP4_R1_CTRL_PREPARE_FLASH          0x29
#define MIP4_R1_CTRL_RUN                    0x2A
#define MIP4_R1_CTRL_5POINT_TEST_MODE       0x2B
#define MIP4_R1_CTRL_WIRELESS_CHARGER_MODE  0x2F
#define MIP4_R1_CTRL_SYNC                   0x31
#define MIP4_R1_CTRL_WET_MODE               0x32

#define MIP4_R0_LED                         0x07
#define MIP4_R1_LED_PROTOCOL_ID             0x00
#define MIP4_R1_LED_NUM                     0x02
#define MIP4_R1_LED_MAX_BRIGHTNESS          0x03
#define MIP4_R1_LED_ON                      0x10
#define MIP4_R1_LED_BRIGHTNESS              0x14

#define MIP4_R0_PARAM                       0x08
#define MIP4_R1_PARAM_BUFFER_ADDR           0x00
#define MIP4_R1_PARAM_PROTOCOL              0x04
#define MIP4_R1_PARAM_MODE                  0x10

#define MIP4_R0_TEST                        0x0A
#define MIP4_R1_TEST_BUF_ADDR               0x00
#define MIP4_R1_TEST_PROTOCOL               0x02
#define MIP4_R1_TEST_TYPE                   0x10
#define MIP4_R1_TEST_DATA_FORMAT            0x20
#define MIP4_R1_TEST_ROW_NUM                0x20
#define MIP4_R1_TEST_COL_NUM                0x21
#define MIP4_R1_TEST_BUFFER_COL_NUM         0x22
#define MIP4_R1_TEST_COL_AXIS               0x23
#define MIP4_R1_TEST_KEY_NUM                0x24
#define MIP4_R1_TEST_DATA_TYPE              0x25
#define MIP4_R1_TEST_VECTOR_NUM             0x26
#define MIP4_R1_TEST_VECTOR_INFO            0x28

#define MIP4_R0_CAL                         0x0B
#define MIP4_R1_CAL_BUF_ADDR                0x00
#define MIP4_R1_CAL_PROTOCOL                0x02
#define MIP4_R1_CAL_VALIDITY                0x08
#define MIP4_R1_CAL_TYPE                    0x10
#define MIP4_R1_CAL_DATA_FORMAT             0x20
#define MIP4_R1_CAL_ROW_NUM                 0x20
#define MIP4_R1_CAL_COL_NUM                 0x21
#define MIP4_R1_CAL_BUFFER_COL_NUM          0x22
#define MIP4_R1_CAL_COL_AXIS                0x23
#define MIP4_R1_CAL_KEY_NUM                 0x24
#define MIP4_R1_CAL_DATA_TYPE               0x25
#define MIP4_R1_CAL_VECTOR_NUM              0x26
#define MIP4_R1_CAL_VECTOR_INFO             0x28

#define MIP4_R0_IMAGE                       0x0C
#define MIP4_R1_IMAGE_BUF_ADDR              0x00
#define MIP4_R1_IMAGE_PROTOCOL_ID           0x04
#define MIP4_R1_IMAGE_TYPE                  0x10
#define MIP4_R1_IMAGE_DATA_FORMAT           0x20
#define MIP4_R1_IMAGE_ROW_NUM               0x20
#define MIP4_R1_IMAGE_COL_NUM               0x21
#define MIP4_R1_IMAGE_BUFFER_COL_NUM        0x22
#define MIP4_R1_IMAGE_COL_AXIS              0x23
#define MIP4_R1_IMAGE_KEY_NUM               0x24
#define MIP4_R1_IMAGE_DATA_TYPE             0x25
#define MIP4_R1_IMAGE_VECTOR_NUM            0x26
#define MIP4_R1_IMAGE_VECTOR_INFO           0x28
#define MIP4_R1_IMAGE_FINGER_NUM            0x30
#define MIP4_R1_IMAGE_FINGER_AREA           0x31

#define MIP4_R0_CUSTOM                      0x0E
#define MIP4_R1_CUSTOM_INFO                 0x00

#define MIP4_R0_LOG                         0x10
#define MIP4_R1_LOG_TRIGGER                 0x14

#define MIP4_R0_LIB                         0xA0
#define MIP4_R1_LIB                         0x00
#define MIP4_LIB_ADDR_START                 0xA000
#define MIP4_LIB_ADDR_END                   0xA3FF

/* Value */
#define MIP4_BOOT_MODE_BOOT                 0x01
#define MIP4_BOOT_MODE_APP                  0x02

#define MIP4_BOOT_STATUS_BUSY               0x05
#define MIP4_BOOT_STATUS_ERROR              0x0E
#define MIP4_BOOT_STATUS_DONE               0xA0

#define MIP4_BOOT_CMD_MASS_ERASE            0x15
#define MIP4_BOOT_CMD_PROGRAM               0x54
#define MIP4_BOOT_CMD_ERASE                 0x8F
#define MIP4_BOOT_CMD_WRITE                 0xA5
#define MIP4_BOOT_CMD_READ                  0xC2

#define MIP4_EVENT_INPUT_TYPE_KEY       0
#define MIP4_EVENT_INPUT_TYPE_SCREEN    1
#define MIP4_EVENT_INPUT_TYPE_PROXIMITY 2

#define MIP4_EVENT_GESTURE_C            1
#define MIP4_EVENT_GESTURE_W            2
#define MIP4_EVENT_GESTURE_V            3
#define MIP4_EVENT_GESTURE_M            4
#define MIP4_EVENT_GESTURE_S            5
#define MIP4_EVENT_GESTURE_Z            6
#define MIP4_EVENT_GESTURE_O            7
#define MIP4_EVENT_GESTURE_E            8
#define MIP4_EVENT_GESTURE_V_90         9
#define MIP4_EVENT_GESTURE_V_180        10
#define MIP4_EVENT_GESTURE_FLICK_RIGHT  20
#define MIP4_EVENT_GESTURE_FLICK_DOWN   21
#define MIP4_EVENT_GESTURE_FLICK_LEFT   22
#define MIP4_EVENT_GESTURE_FLICK_UP     23
#define MIP4_EVENT_GESTURE_DOUBLE_TAP   24
#define MIP4_EVENT_GESTURE_ALL          0xFFFFFFFF

#define MIP4_ALERT_ESD                  1
#define MIP4_ALERT_WAKEUP               2
#define MIP4_ALERT_INPUT_TYPE           3
#define MIP4_ALERT_IMAGE                4
#define MIP4_ALERT_FLASH_FAILURE        5
#define MIP4_ALERT_SRAM_FAILURE         6
#define MIP4_ALERT_BOOT_SUCCEEDED       7
#define MIP4_ALERT_F1                   0xF1

#define MIP4_CTRL_STATUS_NONE           0x05
#define MIP4_CTRL_STATUS_READY          0xA0
#define MIP4_CTRL_STATUS_LOG            0x77

#define MIP4_CTRL_MODE_NORMAL           0
#define MIP4_CTRL_MODE_PARAM            1
#define MIP4_CTRL_MODE_TEST             2
#define MIP4_CTRL_MODE_CAL              16

#define MIP4_CTRL_TRIGGER_NONE          0
#define MIP4_CTRL_TRIGGER_INTR          1
#define MIP4_CTRL_TRIGGER_REG           2

#define MIP4_CTRL_POWER_ACTIVE          0
#define MIP4_CTRL_POWER_LOW             1

#define MIP4_TEST_TYPE_NONE             0
#define MIP4_TEST_TYPE_CM               1
#define MIP4_TEST_TYPE_CM_ABS           2
#define MIP4_TEST_TYPE_CM_JITTER        3
#define MIP4_TEST_TYPE_SHORT            4
#define MIP4_TEST_TYPE_GPIO_LOW         5
#define MIP4_TEST_TYPE_GPIO_HIGH        6
#define MIP4_TEST_TYPE_CM_DIFF_HOR      17
#define MIP4_TEST_TYPE_CM_DIFF_VER      18
#define MIP4_TEST_TYPE_CP_LOW           19
#define MIP4_TEST_TYPE_CP               20
#define MIP4_TEST_TYPE_CP_DIFF_HOR      21
#define MIP4_TEST_TYPE_CP_DIFF_VER      22
#define MIP4_TEST_TYPE_CP_JITTER        23
#define MIP4_TEST_TYPE_CP_HIGH          24
#define MIP4_TEST_TYPE_CP_SHORT         27
#define MIP4_TEST_TYPE_CP_LPM           28
#define MIP4_TEST_TYPE_PANEL_CONN       29
#define MIP4_TEST_TYPE_OPEN_SHORT       30

#define MIP4_CAL_TYPE_RUN_AUTO          10
#define MIP4_CAL_TYPE_READ              11
#define MIP4_CAL_TYPE_ERASE             12

#define MIP4_IMG_TYPE_NONE              0
#define MIP4_IMG_TYPE_INTENSITY         1
#define MIP4_IMG_TYPE_RAWDATA           2
#define MIP4_IMG_TYPE_F_INTENSITY       3
#define MIP4_IMG_TYPE_GESTURE           5
#define MIP4_IMG_TYPE_HSELF_RAWDATA     6
#define MIP4_IMG_TYPE_HSELF_INTENSITY   7
#define MIP4_IMG_TYPE_HSELF_F_INTENSITY 8
#define MIP4_IMG_TYPE_BASELINE          9
#define MIP4_IMG_TYPE_5POINT_INTENSITY  30
#define MIP4_IMG_TYPE_PROX_INTENSITY    31

#define MIP4_LOG_MODE_NONE              0
#define MIP4_LOG_MODE_TRIG              1
