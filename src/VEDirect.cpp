#include "VEDirect.hpp"

uint8_t VEDirectTable::parseMessage(std::string& i_raw){
    std::string raw = i_raw;
    std::smatch lineMatch, keyMatch, valueMatch;
    uint8_t o_counter = 0;
    while(regex_search(raw, lineMatch, lineRegEx))
    {
        regex_search(lineMatch, keyMatch, keyRegEx);
        regex_search(lineMatch, valueMatch, valueRegEx);
        m_values[keyMatch.str().c_str()] = std::stoi(valueMatch.str(), nullptr, 0);
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

std::string* VEDirect::readRaw()
{
    static uint8_t* receiveBuffer = static_cast<uint8_t*>(malloc(UART_BUFFER_SIZE));
    int read = uart_read_bytes(m_uartPort, receiveBuffer, UART_BUFFER_SIZE, (100 / portTICK_RATE_MS));
    if (read > -1) {
        static std::string data(reinterpret_cast<char*>(receiveBuffer), read);
        bool cut = 1;
        if (!m_dirty) {
            if (data.front() != '\n')
                data = data.substr(data.find('\n'), data.length());
            cut = 1;
        } else
            cut = 0;
        if (data.back() != '\n') {
            uint8_t* readByte = 0;
            do {
                read = uart_read_bytes(m_uartPort, readByte, 1, (100 / portTICK_RATE_MS));
                if (!read) {
                    m_dirty = 0;
                    return nullptr;
                }
                data.push_back(*reinterpret_cast<char*>(readByte));
            } while (data.back() != '\n');
        }
        static std::string o_data = (data.substr(cut, data.length() - 1));
        m_dirty = 1;
        return &o_data;
    }
    ESP_LOGW(TAG, "No data read.");
    return nullptr;
}

void VEDirect::read()
{
    std::string* data = readRaw();
    if (data)
        m_Table.parseMessage(*data);
    else
        ESP_LOGE(TAG, "No data read.");
}
