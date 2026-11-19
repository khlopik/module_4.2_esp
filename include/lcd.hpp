#pragma once

#include "esp_lvgl_port.h"
#include "config.hpp"

namespace lcd
{
    struct ColorBar
    {
        lv_obj_t *rect = nullptr;
        lv_obj_t *valueLabel = nullptr;
        uint8_t width = 10;
        uint8_t maxHeight = lcdConfig::maxBarHeight;
        uint8_t posX = 0;
        uint8_t posY = 0;
        const char *label = nullptr;
    };

    enum class Color
    {
        Red,
        Green,
        Blue,
        Unknown,
    };

    class LcdHandle
    {
    public:
        void init();
        void example();
        void initBar(ColorBar &bar);
        void updateBarData(Color color, uint8_t height, uint8_t value);
        // void updateBarHeight(uint8_t height);
        static void updateBarHeightTask(void *pvParams);
        void updateBarHeightLoop();

    private:
        lv_disp_t *disp_ = nullptr;
        i2c_master_bus_handle_t i2cBus_ = nullptr;
        lv_obj_t *rect_ = nullptr;

        ColorBar redBar_ = {.posX = 20, .posY = 5, .label = "R"};
        ColorBar greenBar_ = {.posX = 65, .posY = 5, .label = "G"};
        ColorBar blueBar_ = {.posX = 110, .posY = 5, .label = "B"};
    };
}