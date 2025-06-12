/*
 * Copyright 2020-2022 Yuntu Microelectronics co.,ltd
 * All rights reserved.
 *
 * YUNTU Confidential. This software is owned or controlled by YUNTU and may
 * only be used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software. The production use license in
 * Section 2.3 is expressly granted for this software.
 */

#ifndef STATUS_H
#define STATUS_H

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Status return codes.
 * Common error codes will be a unified enumeration (C enum) that will contain all error codes
 * (common and specific). There will be separate "error values spaces" (or slots), each of 256
 * positions, allocated per functionality.
 */
typedef enum
{
    /* Generic error codes */
    STATUS_SUCCESS                         = 0x000U,    /*!< Generic operation success status */
    STATUS_ERROR                           = 0x001U,    /*!< Generic operation failure status */
    STATUS_BUSY                            = 0x002U,    /*!< Generic operation busy status */
    STATUS_TIMEOUT                         = 0x003U,    /*!< Generic operation timeout status */
    STATUS_UNSUPPORTED                     = 0x004U,    /*!< Generic operation unsupported status */
    STATUS_NOCLOCK                         = 0x005U,    /*!< Generic operation noclock status */
    /* MCU specific error codes */
    STATUS_MCU_GATED_OFF                   = 0x100U,  /*!< Module is gated off */
    STATUS_MCU_TRANSITION_FAILED           = 0x101U,  /*!< Error occurs during transition. */
    STATUS_MCU_INVALID_STATE               = 0x102U,  /*!< Unsupported in current state. */
    STATUS_MCU_NOTIFY_BEFORE_ERROR         = 0x103U,  /*!< Error occurs during send "BEFORE" notification. */
    STATUS_MCU_NOTIFY_AFTER_ERROR          = 0x104U,  /*!< Error occurs during send "AFTER" notification. */
    /* I2C specific error codes */
    STATUS_I2C_RECEIVED_NACK               = 0x200U,  /*!< NACK signal received  */
    STATUS_I2C_TX_UNDERRUN                 = 0x201U,  /*!< TX underrun error */
    STATUS_I2C_RX_OVERRUN                  = 0x202U,  /*!< RX overrun error */
    STATUS_I2C_ARBITRATION_LOST            = 0x203U,  /*!< Arbitration lost */
    STATUS_I2C_ABORTED                     = 0x204U,  /*!< A transfer was aborted */
    STATUS_I2C_BUS_BUSY                    = 0x205U,  /*!< I2C bus is busy, cannot start transfer */
    /* CAN specific error codes */
    STATUS_CAN_BUFF_OUT_OF_RANGE           = 0x300U,  /*!< The specified MB index is out of the configurable range */
    STATUS_CAN_NO_TRANSFER_IN_PROGRESS     = 0x301U,  /*!< There is no transmission or reception in progress */
    /* SPI specific error codes */
    STATUS_SPI_TX_UNDERRUN                 = 0x500U,  /*!< TX underrun error */
    STATUS_SPI_RX_OVERRUN                  = 0x501U,  /*!< RX overrun error */
    STATUS_SPI_ABORTED                     = 0x502U,  /*!< A transfer was aborted */
    /* UART specific error codes */
    STATUS_UART_TX_UNDERRUN                = 0x600U,  /*!< TX underrun error */
    STATUS_UART_RX_OVERRUN                 = 0x601U,  /*!< RX overrun error */
    STATUS_UART_ABORTED                    = 0x602U,  /*!< A transfer was aborted */
    STATUS_UART_FRAMING_ERROR              = 0x603U,  /*!< Framing error */
    STATUS_UART_PARITY_ERROR               = 0x604U,  /*!< Parity error */
    STATUS_UART_NOISE_ERROR                = 0x605U,  /*!< Noise error */
    STATUS_UART_IDLE_ERROR                 = 0x606U,  /*!< Line idle error */
    /* LINFlexD specific error codes */
    STATUS_LINFlexD_NO_HEADER_RECEIVED     = 0x700U,  /*!< LINFlexD no header received */
    STATUS_LINFlexD_ABORT_TIMEOUT          = 0x701U,  /*!< LINFlexD abort transfer timeout error*/
    /* SBC specific error codes */
    SBC_NVN_ERROR                          = 0x801U, /*!< Unsuccessful attempt writing to non volatile memory
                                                          (0x73 and 0x74). Set device to factory settings. */
    SBC_COMM_ERROR                         = 0x802U, /*!< Data transfer was aborted */
    SBC_CMD_ERROR                          = 0x804U, /*!< Wrong command. */
    SBC_ERR_NA                             = 0x808U, /*!< Feature/device not available */
    SBC_MTPNV_LOCKED                       = 0x810U, /*!< Unable to write MTPNV cells, NVMPS = 0 */

    /* EFM specific error codes */
    STATUS_EFM_ERROR_ENABLE                = 0x901U, /*!< It's impossible to enable an operation */
    STATUS_EFM_ERROR_NO_BLOCK              = 0x902U, /*!< No blocks have been enabled for Array Integrity check */
    STATUS_EFM_BUSY                        = 0x903U, /*!< EFM module current busy status */
    STATUS_EFM_PROG_SIZE_ERROR             = 0x904U, /*!< EFM program size error */
    STATUS_EFM_DATA_NOT_ALIGNED_ERROR      = 0x905U, /*!< EFM program data not aligned error */
    STATUS_EFM_ADDRESS_OUT_OF_RANGE        = 0x906U, /*!< EFM program address out of range error */
    STATUS_EFM_PROGRAM_EXECUTE_ERROR       = 0x907U, /*!< EFM program command execution error */
    STATUS_EFM_NOT_SUPPORTED_CMD_ERROR     = 0x908U, /*!< EFM program command not supported error */
    STATUS_EFM_CMD_EXECUTE_ERROR           = 0x909U, /*!< EFM program command execute error */

    STATUS_EdPROM_ID_OUT_OF_RANGE          = 0x980U, /*!< EFM Emulated EEPROM ID out of range error */
    STATUS_EdPROM_ID_NOT_FOUND             = 0x981U, /*!< EFM Emulated EEPROM ID not fould error */
    STATUS_EdPROM_MAKE_DEAD_ERROR          = 0x982U, /*!< EFM Emulated EEPROM Make sector dead error */
    STATUS_EdPROM_MAKE_DEAD_SUCCESS        = 0x983U, /*!< EFM Emulated EEPROM Make sector dead success */
    STATUS_EdPROM_SECTOR_ERASE_ERROR       = 0x984U, /*!< EFM Emulated EEPROM sector erase error */
    STATUS_EdPROM_PROG_VERIFY_ERROR        = 0x985U, /*!< EFM Emulated EEPROM program check error */
    /* SCU specific error codes */
    STATUS_SCU_CLK_SWITCH_TIMEOUT          = 0xA01U, /*!< SCU system clock switch timeout error*/
    STATUS_SCU_DIVIDER_SWITCH_TIMEOUT      = 0xA02U, /*!< SCU system clock divider switch timeout error*/
    STATUS_SCU_PLL_LOCK_TIMEOUT            = 0xA03U, /*!< SCU PLL lock timeout error*/
    STATUS_SCU_FIRC_READY_TIMEOUT          = 0xA04U, /*!< SCU FIRC clock ready timeout error*/
    STATUS_SCU_SIRC_READY_TIMEOUT          = 0xA05U, /*!< SCU SIRC clock ready timeout error*/
    STATUS_SCU_FXOSC_READY_TIMEOUT         = 0xA06U, /*!< SCU FXOSC clock ready timeout error*/
    STATUS_SCU_SXOSC_READY_TIMEOUT         = 0xA07U, /*!< SCU FXOSC clock ready timeout error*/
    /* HCU specific error codes */
    STATUS_HCU_KEY_SIZE_NOT_SUPPORTED      = 0xB01U, /*!< HCU key size not supported error*/
    STATUS_HCU_LOAD_KEY_WHEN_BUSY          = 0xB02U, /*!< HCU load key when busy error*/
    STATUS_HCU_CCM_TAG_SIZE_ERROR          = 0xB04U, /*!< HCU ccm tag size configuration error*/
    STATUS_HCU_CCM_NONCE_DATA_SIZE_ERROR   = 0xB08U, /*!< HCU ccm nonce size configuration error*/
    STATUS_HCU_CCM_NOT_CONFIGURED_ERROR    = 0xB0CU, /*!< HCU ccm nonce size configuration error*/
    STATUS_HCU_MAC_CHECK_ERROR             = 0xB10U, /*!< HCU ccm nonce size configuration error*/
    STATUS_HCU_HASH_CHECK_ERROR            = 0xB20U, /*!< HCU sha check error*/

    /* FLEXCAN specific error codes */
    STATUS_FLEXCAN_BUFF_OUT_OF_RANGE       = 0x1100U,  /*!< The specified MB index is out of the configurable range */
    STATUS_FLEXCAN_NO_TRANSFER_IN_PROGRESS = 0x1101U,  /*!< There is no transmission or reception in progress */
	STATUS_FLEXCAN_NO_CONFIG_pinmux        =  0x1102,   /*!< There is no configration of Flexcan_pinmux in progress */
	STATUS_FLEXCAN_OVERRUN_IDFILTER_FIFOCFG        =  0x1103   /*! flexcanmailboxÅäÖÃ³¬³ö·¶Î§*/
 
} status_t;

#endif /* STATUS_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
