#pragma once

#include <cstdint>
#include <map>
#include <string.h>
#include <string>
#include <iostream>
#include <iterator>
#include <regex>

#include "driver/uart.h"
#include "esp32/rom/crc.h"
#include "esp_log.h"
#include "freertos/queue.h"

#define MAX_LABEL_LENGTH 9
#define UART_BUFFER_SIZE 1024

static const char* TAG = "VEDirect";

static const uart_config_t uartConfig = {
    .baud_rate = 19200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122
};

static const std::regex lineRegEx("\r\n.+\t[0-9]+");
static const std::regex keyRegEx("\r\n(.+)(?=\t[0-9]+)");
static const std::regex DirtyLineRegEx(".+\t[0-9]+");
static const std::regex DirtyKeyRegEx("(.+)(?=\t[0-9]+)");

class VEDirectTable {
private:
    std::map<std::string, int32_t> m_values {
        { "V", 0 },
        { "V2", 0 },
        { "V3", 0 },
        { "VS", 0 },
        { "VM", 0 },
        { "DM", 0 },
        { "VPV", 0 },
        { "PPV", 0 },
        { "I", 0 },
        { "I2", 0 },
        { "I3", 0 },
        { "IL", 0 },
        { "LOAD", 0 },
        { "T", 0 },
        { "P", 0 },
        { "CE", 0 },
        { "SOC", 0 },
        { "TTG", 0 },
        { "Alarm", 0 },
        { "Relay", 0 },
        { "AR", 0 },
        { "OR", 0 },
        { "H1", 0 },
        { "H2", 0 },
        { "H3", 0 },
        { "H4", 0 },
        { "H5", 0 },
        { "H6", 0 },
        { "H7", 0 },
        { "H8", 0 },
        { "H9", 0 },
        { "H10", 0 },
        { "H11", 0 },
        { "H12", 0 },
        { "H13", 0 },
        { "H14", 0 },
        { "H15", 0 },
        { "H16", 0 },
        { "H17", 0 },
        { "H18", 0 },
        { "H19", 0 },
        { "H20", 0 },
        { "H21", 0 },
        { "H22", 0 },
        { "H23", 0 },
        { "ERR", 0 },
        { "CS", 0 },
        { "BMV", 0 },
        { "FW", 0 },
        { "FWE", 0 },
        { "PID", 0 },
        { "SER#", 0 },
        { "HSDS", 0 },
        { "MODE", 0 },
        { "AC_OUT_V", 0 },
        { "AC_OUT_I", 0 },
        { "AC_OUT_S", 0 },
        { "WARN", 0 },
        { "MPPT", 0 },
    };

public:
    int32_t operator[](std::string i_label) { return m_values[i_label]; }

    uint8_t parseMessage(std::string& i_raw, bool i_dirty = 0);
};

class VEDirect {
private:
    VEDirectTable m_Table;
    std::string readRaw();
    bool m_dirty = 0;
    uart_port_t m_uartPort;

public:
    void init(uart_port_t i_uartPort,
        int i_rxPin = UART_PIN_NO_CHANGE,
        int i_txPin = UART_PIN_NO_CHANGE);
    int read();

    VEDirectTable& table() { return m_Table; }
    int32_t operator[](std::string i_label) { return m_Table[i_label]; }
};
