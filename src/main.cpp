#include "driver/uart.h"
#include "button.hpp"
#include "led.hpp"
#include "uart.hpp"
#include "esp_log.h"
#include "config.hpp"
#include "lcd.hpp"
#include "freertos/FreeRTOS.h"

namespace
{
    const char *TAG = "MAIN";

    constexpr int uart_buffer_size = (1024 * 2);
    constexpr uart_port_t uart_num = UART_NUM_2;
    button::ButtonHandler buttonHandler;
    led::LedHandle ledHandle;
    lcd::LcdHandle lcdHandle;

    uart::UartHandler uartHandler;

    uint8_t convertRGBValueToBarHeight(uint8_t value)
    {
        return value * lcdConfig::maxBarHeight / 255;
    }

    void onSTMCommand(uart::RxCommand cmd)
    {
        ESP_LOGI(TAG, "onSTMCommand");
        lcd::Color colorToUpdate = lcd::Color::Red;

        switch (cmd.type)
        {
        case uart::CommandType::OFF:
            ledHandle.turnOff();
            break;
        case uart::CommandType::ON:
            ledHandle.turnOn();
            break;
        case uart::CommandType::R:
            ledHandle.setColor(led::Color::Red, cmd.value);
            colorToUpdate = lcd::Color::Red;
            break;
        case uart::CommandType::G:
            ledHandle.setColor(led::Color::Green, cmd.value);
            colorToUpdate = lcd::Color::Green;
            break;
        case uart::CommandType::B:
            ledHandle.setColor(led::Color::Blue, cmd.value);
            colorToUpdate = lcd::Color::Blue;
            break;
        default:
            break;
        }

        if (cmd.type != uart::CommandType::ON && cmd.type != uart::CommandType::OFF)
        {
            lcdHandle.updateBarData(colorToUpdate, convertRGBValueToBarHeight(cmd.value), cmd.value);
        }
    }

    void onButtonPress(void *arg, void *data)
    {
        uartHandler.sendCommandToQueue(uart::TxCommand::ButtonPress);
    }

    void OnButtonRelease(void *arg, void *data)
    {
        uartHandler.sendCommandToQueue(uart::TxCommand::ButtonRelease);
    }
}

void setup()
{
    ledHandle.init();
    buttonHandler.init(buttonConfig::pin, buttonConfig::shortPressTimeMs, buttonConfig::activeLevel);
    buttonHandler.setOnPressCallback(&onButtonPress);
    buttonHandler.setOnReleaseCallback(&OnButtonRelease);
    uartHandler.init(uartConfig::rxPin, uartConfig::txPin);
    uartHandler.setCallback(&onSTMCommand);
    lcdHandle.init();

    if (xTaskCreate(uartHandler.checkMessagesTask, "Check messages", 2048, &uartHandler, 2, NULL) != pdPASS)
    {
        abort();
    }

    if (xTaskCreate(uartHandler.sendMessagesTask, "Send messages", 2048, &uartHandler, 3, NULL) != pdPASS)
    {
        abort();
    }
}

void app()
{
}

extern "C" void app_main()
{
    setup();
}