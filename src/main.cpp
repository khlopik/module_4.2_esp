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

    button::ButtonHandler buttonHandler;
    led::LedHandle ledHandle;
    lcd::LcdHandle lcdHandle;
    uart::UartHandler uartHandler;

    uint8_t convertRGBValueToBarHeight(uint8_t value)
    {
        return value * lcdConfig::maxBarHeight / 255; // 255 is max color value
    }

    void onSTMCommand(uart::RxCommand cmd)
    {
        ESP_LOGI(TAG, "onSTMCommand");
        lcd::Color colorToUpdate = lcd::Color::Unknown;

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

        if (colorToUpdate != lcd::Color::Unknown)
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
}

void app()
{
}

extern "C" void app_main()
{
    setup();
}