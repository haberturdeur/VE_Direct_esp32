#include "VEDirect.hpp"

uint8_t VEDirectTable::parseMessage(std::string& i_raw, bool i_dirty)
{
    std::string raw = i_raw;
    std::smatch lineMatch, keyMatch, valueMatch;
    uint8_t o_counter = 0;
    if (i_dirty) {
        regex_search(raw, lineMatch, DirtyLineRegEx);
        std::string line = lineMatch.str();
        regex_search(line, keyMatch, DirtyKeyRegEx);
        std::string key = keyMatch.str();
        std::string value = line.substr(keyMatch.str().length());
        m_values[key] = std::stoi(value, nullptr, 0);
        raw = lineMatch.suffix();
        o_counter++;
    }
    while (regex_search(raw, lineMatch, lineRegEx)) {
        std::string line = lineMatch.str();
        regex_search(line, keyMatch, keyRegEx);
        std::string key = keyMatch.str().substr(1);
        std::string value = line.substr(keyMatch.str().length());
        m_values[key] = std::stoi(value, nullptr, 0);
        raw = lineMatch.suffix();
        o_counter++;
    }
    return o_counter;
}

void VEDirect::init(uart_port_t i_uartPort, int i_rxPin, int i_txPin)
{
    uart_param_config(i_uartPort, &uartConfig);
    uart_set_pin(i_uartPort, i_txPin, i_rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(i_uartPort, UART_BUFFER_SIZE * 2, 0, 0, nullptr, 0);
    uart_set_mode(i_uartPort, UART_MODE_UART);
    ESP_LOGI(TAG, "Initialized decoder.");
    m_uartPort = i_uartPort;
}

std::string VEDirect::readRaw()
{
    size_t length = 0;
    uart_get_buffered_data_len(m_uartPort, &length);
    if (length) {
        std::string receiveBuffer;
        receiveBuffer.reserve(UART_BUFFER_SIZE);
        receiveBuffer.resize(length + 1); //data + \0
        int read = uart_read_bytes(m_uartPort, receiveBuffer.data, UART_BUFFER_SIZE, (3000 / portTICK_RATE_MS));
        if (read > -1) {
            std::string data = receiveBuffer;
            bool cut = 1;
            if (!m_dirty) {
                if (data.substr(0, 2) != "\r\n") {
                    int i = data.find("\r\n");
                    if (i < data.length())
                        data = data.substr(i);
                    // else
                    ESP_LOGW(TAG, "No new line.");
                }
                cut = 1;
            } else
                cut = 0;
            if (data.substr(data.length() - 1, 2) != "\n\r") {
                uint8_t* readByte = 0;
                do {
                    read = uart_read_bytes(m_uartPort, readByte, 1, (100 / portTICK_RATE_MS));
                    if (!read) {
                        m_dirty = 0;
                        return "";
                    }
                    data.push_back(*reinterpret_cast<char*>(readByte));
                } while (data.substr(data.length() - 1, 2) != "\n\r");
            }
            std::string o_data = data.substr(cut, data.length() - 1);
            m_dirty = 1;
            return o_data;
        }
        ESP_LOGW(TAG, "No data read, but something was buffered.");
        return "";
    }
    ESP_LOGW(TAG, "No data available.");
    return "";
}

int VEDirect::read()
{
    std::string data = readRaw();
    if (!data.empty()) {
        m_Table.parseMessage(data, 1);
        return 1;
    }
    ESP_LOGE(TAG, "No data passable to table.");
    return -1;
}
