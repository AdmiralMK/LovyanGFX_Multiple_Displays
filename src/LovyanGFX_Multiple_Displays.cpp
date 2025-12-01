#include <Arduino.h>
#define LGFX_USE_V1 // Use V1 API for better multi-display support
#include <LovyanGFX.hpp>

#define SCK 18  // SCL / SCLK
#define MOSI 23 // SDA
#define DC 2    // DC
#define CS_1 15 // CS
#define CS_2 16 // CS
#define RST_1 4 // RES
#define RST_2 5 // RES
#define DISPLAY_WIDTH 170
#define DISPLAY_HEIGHT 320
#define DISPLAY_OFFSET_X 35
#define DISPLAY_INVERT true

// Display 1 config (ST7789, CS=5)
class LGFX_Display1 : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7789 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX_Display1(void)
  {
    { // Bus config (shared SPI)
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;  // Or HSPI_HOST/VSPI_HOST
      cfg.freq_write = 80000000; // 40MHz SPI clock
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = SCK;
      cfg.pin_mosi = MOSI;
      cfg.pin_dc = DC;
      // cfg.pin_cs = CS_1;  // Unique CS for Display 1
      // cfg.pin_rst = RST;  // Shared RESET
      cfg.spi_3wire = true;
      cfg.use_lock = true;
      cfg.spi_mode = 0;
      // cfg.pin_miso = 19;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    { // Panel config
      auto cfg = _panel_instance.config();
      cfg.pin_cs = CS_1;   // Match bus CS
      cfg.pin_rst = RST_1; // Shared RESET
      cfg.panel_width = DISPLAY_WIDTH;
      cfg.panel_height = DISPLAY_HEIGHT;
      cfg.offset_x = DISPLAY_OFFSET_X;
      cfg.invert = DISPLAY_INVERT;
      cfg.memory_width = DISPLAY_WIDTH;
      cfg.memory_height = DISPLAY_HEIGHT;
      // cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;
      _panel_instance.config(cfg);
    }
    setPanel(&_panel_instance); // Assign panel to device
  }
};

// Display 2 config (ST7789, CS=16)
class LGFX_Display2 : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7789 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX_Display2(void)
  {
    { // Bus config (shared SPI)
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST; // Same host as Display 1
      cfg.freq_write = 80000000;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = SCK;
      cfg.pin_mosi = MOSI;
      cfg.pin_dc = DC;
      // cfg.pin_cs = CS_2;  // Unique CS for Display 2
      // cfg.pin_rst = RST;  // Shared RESET
      cfg.spi_3wire = true;
      cfg.use_lock = true;
      // cfg.pin_miso = 19;
      cfg.spi_mode = 0;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    { // Panel config (same as Display 1)
      auto cfg = _panel_instance.config();
      cfg.pin_cs = CS_2;
      cfg.pin_rst = RST_2; // Shared RESET
      cfg.panel_width = DISPLAY_WIDTH;
      cfg.panel_height = DISPLAY_HEIGHT;
      cfg.offset_x = DISPLAY_OFFSET_X;
      cfg.invert = DISPLAY_INVERT;
      cfg.memory_width = DISPLAY_WIDTH;
      cfg.memory_height = DISPLAY_HEIGHT;
      // cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;
      _panel_instance.config(cfg);
    }
    setPanel(&_panel_instance);
  }
};

static LGFX_Display1 tft1; // Instantiate Display 1
static LGFX_Display2 tft2; // Instantiate Display 2

void setup()
{
  Serial.begin(115200);
  tft1.init(); // Initialize Display 1
  tft2.init(); // Initialize Display 2
  // tft1.setRotation(0);  // Adjust rotation if needed
  // tft2.setRotation(0);
  // tft1.fillScreen(TFT_WHITE);  // Clear both screens
  // tft2.fillScreen(TFT_WHITE);
  Serial.println("Displays initialized!");
}

void loop()
{
  // Draw to Display 1: Red rectangle
  // tft1.fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, TFT_RED);
  // tft1.setTextColor(TFT_WHITE);
  // tft1.drawString("Display 1", 80, 110);

  tft1.clear();
  tft1.fillRect(0, 50, DISPLAY_WIDTH, DISPLAY_HEIGHT - 100 , TFT_RED);

  for (int i = 1; i <= 100; ++i)
  {
    tft1.scroll(0, -10);
  }

  /*
  // Draw to Display 2: Blue text
  tft2.fillScreen(TFT_BLACK);  // Clear for demo
  tft2.setTextColor(TFT_BLUE);
  tft2.setTextSize(2);
  tft2.drawString("Hello Display 2!", 20, 100);
  */

  delay(1000); // Update every second
}
