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
      cfg.pin_miso = -1;
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
      cfg.bus_shared = false;
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

static LGFX_Display1 lcd;  // Instantiate Display 1
static LGFX_Display2 tft2; // Instantiate Display 2

LGFX_Sprite sprite(&lcd); // спрайт

int x = 20; // текущая позиция по X
int y = 50; // по Y
int dx = 3; // скорость по X
int dy = 2; // скорость по Y
int sprite_height = 200;
int sprite_width = 120;
int x_centre = DISPLAY_WIDTH / 2 - sprite_width / 2; 
int y_centre = DISPLAY_HEIGHT / 2 - sprite_height / 2; 

void setup()
{
  lcd.init();
  lcd.setRotation(0);
  lcd.fillScreen(TFT_BLACK);

  sprite.createSprite(sprite_width, sprite_height);
  sprite.fillSprite(TFT_BLACK);
  sprite.fillRect(0, 0, sprite_width, sprite_height, TFT_GREEN);
  sprite.drawRect(0, 0, sprite_width, sprite_height, TFT_RED);
  sprite.pushSprite(x_centre, y_centre);
  delay(1000); 

  //sprite.setPaletteColor(1, lcd.color888( 0, 0, count & 0xFF));
  //sprite.setPaletteColor(2, lcd.color888( 0,~count & 0xFF, 0));
  //sprite.setPaletteColor(3, lcd.color888( count & 0xFF, 0, 0));
  
  sprite.pushRotateZoom(x_centre, y_centre, 45, 1, 1, 0);

  lcd.startWrite();
}

void loop()
{
  // Стираем предыдущее положение (перерисовываем чёрным квадратом)
  // Если не стирать — будет оставаться след!
  // lcd.fillRect(x, y, 40, 40, TFT_BLACK);

  // Выводим спрайт в новой позиции
  // sprite.pushSprite(x, y);         // без третьего параметра — без прозрачности
  // или так, если хотите прозрачный фон (например, цвет магента будет прозрачным):
  // sprite.pushSprite(x, y, TFT_MAGENTA);

  // Двигаем координаты
  x += dx;
  y += dy;

  // Отскок от краёв экрана
  if (x <= 0 || x >= lcd.width() - 40)
    dx = -dx;
  if (y <= 0 || y >= lcd.height() - 40)
    dy = -dy;

  delay(20); // ~50 кадров в секунду
}

// static LGFX_Sprite sprite(&lcd); // スプライトを使う場合はLGFX_Spriteのインスタンスを作成。

// もし現在 TFT_eSPI を使用中で、ソースをなるべく変更したくない場合は、こちらのヘッダを利用できます。
// #include <LGFX_TFT_eSPI.hpp>
// static TFT_eSPI lcd;               // TFT_eSPIがLGFXの別名として定義されます。
// static TFT_eSprite sprite(&lcd);   // TFT_eSpriteがLGFX_Spriteの別名として定義されます。

// 対応機種に無い構成で使う場合は、 examples/HowToUse/2_user_setting.ino を参照してください。
// また設定例はsrc/lgfx_userフォルダにもあります。

void setup111(void)
{
  // 最初に初期化関数を呼び出します。
  lcd.init();
  // lcd.clear(); // 画面全体をバックグラウンドカラーで塗り潰します。

  // 回転方向を 0～3 の4方向から設定します。(4～7を使用すると上下反転になります。)
  // lcd.setRotation(1);

  // バックライトの輝度を 0～255 の範囲で設定します。
  // lcd.setBrightness(255);

  // 必要に応じてカラーモードを設定します。（初期値は16）
  // 16の方がSPI通信量が少なく高速に動作しますが、赤と青の諧調が5bitになります。
  // 24の方がSPI通信量が多くなりますが、諧調表現が綺麗になります。
  // lcd.setColorDepth(16);  // RGB565の16ビットに設定
  // lcd.setColorDepth(24); // RGB888の24ビットに設定(表示される色数はパネル性能によりRGB666の18ビットになります)

  uint32_t red = 0xFF0000;
  uint16_t green = 0x07E0;
  uint8_t blue = 0x03;

  // 基本的な図形の描画関数は以下の通りです。
  /*
    fillScreen    (                color);  // 画面全体の塗り潰し
    drawPixel     ( x, y         , color);  // 点
    drawFastVLine ( x, y   , h   , color);  // 垂直線
    drawFastHLine ( x, y, w      , color);  // 水平線
    drawRect      ( x, y, w, h   , color);  // 矩形の外周
    fillRect      ( x, y, w, h   , color);  // 矩形の塗り
    drawRoundRect ( x, y, w, h, r, color);  // 角丸の矩形の外周
    fillRoundRect ( x, y, w, h, r, color);  // 角丸の矩形の塗り
    drawCircle    ( x, y      , r, color);  // 円の外周
    fillCircle    ( x, y      , r, color);  // 円の塗り
    drawEllipse   ( x, y, rx, ry , color);  // 楕円の外周
    fillEllipse   ( x, y, rx, ry , color);  // 楕円の塗り
    drawLine      ( x0, y0, x1, y1        , color); // ２点間の直線
    drawTriangle  ( x0, y0, x1, y1, x2, y2, color); // ３点間の三角形の外周
    fillTriangle  ( x0, y0, x1, y1, x2, y2, color); // ３点間の三角形の塗り
    drawBezier    ( x0, y0, x1, y1, x2, y2, color); // ３点間のベジエ曲線
    drawBezier    ( x0, y0, x1, y1, x2, y2, x3, y3, color); // ４点間のベジエ曲線
    drawArc       ( x, y, r0, r1, angle0, angle1, color);   // 円弧の外周
    fillArc       ( x, y, r0, r1, angle0, angle1, color);   // 円弧の塗り
  */

  /*
  // 例えばdrawPixelで点を書く場合は、引数は X座標,Y座標,色 の３つ。
    lcd.drawPixel(0, 0, 0xFFFF); // 座標0:0に白の点を描画


  // カラーコードを生成する関数が用意されており、色の指定に使用できます。
  // 引数は、赤,緑,青をそれぞれ 0～255で指定します。
  // 色情報の欠落を防ぐため、color888を使う事を推奨します。
    lcd.drawFastVLine(2, 0, 100, lcd.color888(255,   0,   0)); // 赤で垂直の線を描画
    lcd.drawFastVLine(4, 0, 100, lcd.color565(  0, 255,   0)); // 緑で垂直の線を描画
    lcd.drawFastVLine(6, 0, 100, lcd.color332(  0,   0, 255)); // 青で垂直の線を描画


  // カラーコード生成関数を使用しない場合は以下のようになります。
  // RGB888 24ビットで指定 uint32_t型
  // RGB565 16ビットで指定 uint16_t型、int32_t型
  // RGB332  8ビットで指定 uint8_t型

  // uint32_t型を使用すると、RGB888の24ビットとして扱われます。
  // 16進数2桁で赤緑青の順に記述できます。
  // uint32_t型の変数を使うか、末尾にUを付けるか、uint32_t型にキャストして使用します。

    lcd.drawFastHLine(0, 2, 100, red);            // 赤で水平の線を描画
    lcd.drawFastHLine(0, 4, 100, 0x00FF00U);      // 緑で水平の線を描画
    lcd.drawFastHLine(0, 6, 100, (uint32_t)0xFF); // 青で水平の線を描画


  // uint16_t型およびint32_t型を使用すると、RGB565の16ビットとして扱われます。
  // 特別な書き方をしない場合はint32_t型として扱われるので、この方式になります。
  // （AdafruitGFX や TFT_eSPI との互換性のために、このようにしています。）

    lcd.drawRect(10, 10, 50, 50, 0xF800);         // 赤で矩形の外周を描画
    lcd.drawRect(12, 12, 50, 50, green);          // 緑で矩形の外周を描画
    lcd.drawRect(14, 14, 50, 50, (uint16_t)0x1F); // 青で矩形の外周を描画


  // int8_t型、uint8_t型を使用すると、RGB332の8ビットとして扱われます。

    lcd.fillRect(20, 20, 20, 20, (uint8_t)0xE0);  // 赤で矩形の塗りを描画
    lcd.fillRect(30, 30, 20, 20, (uint8_t)0x1C);  // 緑で矩形の塗りを描画
    lcd.fillRect(40, 40, 20, 20, blue);           // 青で矩形の塗りを描画


  // 描画関数の引数の色は省略できます。
  // 省略した場合、setColor関数で設定した色 または最後に使用した色を描画色として使用します。
  // 同じ色で繰り返し描画する場合は、省略した方がわずかに速く動作します。
    lcd.setColor(0xFF0000U);                        // 描画色に赤色を指定
    lcd.fillCircle ( 40, 80, 20    );               // 赤色で円の塗り
    lcd.fillEllipse( 80, 40, 10, 20);               // 赤色で楕円の塗り
    lcd.fillArc    ( 80, 80, 20, 10, 0, 90);        // 赤色で円弧の塗り
    lcd.fillTriangle(80, 80, 60, 80, 80, 60);       // 赤色で三角の塗り
    lcd.setColor(0x0000FFU);                        // 描画色に青色を指定
    lcd.drawCircle ( 40, 80, 20    );               // 青色で円の外周
    lcd.drawEllipse( 80, 40, 10, 20);               // 青色で楕円の外周
    lcd.drawArc    ( 80, 80, 20, 10, 0, 90);        // 青色で円弧の外周
    lcd.drawTriangle(60, 80, 80, 80, 80, 60);       // 青色で三角の外周
    lcd.setColor(0x00FF00U);                        // 描画色に緑色を指定
    lcd.drawBezier( 60, 80, 80, 80, 80, 60);        // 緑色で二次ベジエ曲線
    lcd.drawBezier( 60, 80, 80, 20, 20, 80, 80, 60);// 緑色で三次ベジエ曲線

  // グラデーションの線を描画するdrawGradientLine は色の指定を省略できません。
    lcd.drawGradientLine( 0, 80, 80, 0, 0xFF0000U, 0x0000FFU);// 赤から青へのグラデーション直線

    delay(1000);

    */

  /*
// clearまたはfillScreenで画面全体を塗り潰せます。
// fillScreenはfillRectの画面全体を指定したのと同じで、色の指定は描画色の扱いになります。
lcd.fillScreen(0xFFFFFFu); // 白で塗り潰し
lcd.setColor(0x00FF00u);   // 描画色に緑色を指定
lcd.fillScreen();          // 緑で塗り潰し

// clearは描画系の関数とは別で背景色という扱いで色を保持しています。
// 背景色は出番が少ないですが、スクロール機能使用時の隙間を塗る色としても使用されます。
lcd.clear(0xFFFFFFu);        // 背景色に白を指定して塗り潰し
lcd.setBaseColor(0x000000u); // 背景色に黒を指定
lcd.clear();                 // 黒で塗り潰し

// SPIバスの確保と解放は描画関数を呼び出した時に自動的に行われますが、
// 描画スピードを重視する場合は、描画処理の前後に startWriteとendWriteを使用します。
// SPIバスの確保と解放が抑制され、速度が向上します。
// 電子ペーパー(EPD)の場合、startWrite()以降の描画は、endWrite()を呼ぶ事で画面に反映されます。
lcd.drawLine(0, 1, 39, 40, red);       // SPIバス確保、線を描画、SPIバス解放
lcd.drawLine(1, 0, 40, 39, blue);      // SPIバス確保、線を描画、SPIバス解放
lcd.startWrite();                      // SPIバス確保
lcd.drawLine(38, 0, 0, 38, 0xFFFF00U); // 線を描画
lcd.drawLine(39, 1, 1, 39, 0xFF00FFU); // 線を描画
lcd.drawLine(40, 2, 2, 40, 0x00FFFFU); // 線を描画
lcd.endWrite();                        // SPIバス解放

// startWriteとendWriteは呼出し回数を内部でカウントしており、
// 繰り返し呼び出した場合は最初と最後のみ動作します。
// startWriteとendWriteは必ず対になるように使用してください。
// (SPIバスを占有して構わない場合は、最初にstartWriteを一度呼び、endWriteしない使い方も可能です。)
lcd.startWrite(); // カウント+1、SPIバス確保
lcd.startWrite(); // カウント+1
lcd.startWrite(); // カウント+1
lcd.endWrite();   // カウント-1
lcd.endWrite();   // カウント-1
lcd.endWrite();   // カウント-1、SPIバス解放
lcd.endWrite();   // 何もしない
                  // なお過剰にendWriteを呼び出した場合は何も行わず、カウントがマイナスになることもありません。

// startWriteのカウントの状態に依らず、強制的にSPIバスを解放・確保したい場合は、
// endTransaction・beginTransactionを使用します。
// カウントはクリアされないので、辻褄が合わなくならないよう注意してください。
lcd.startWrite();     // カウント+1、SPIバス確保
lcd.startWrite();     // カウント+1
lcd.drawPixel(0, 0);  // 描画
lcd.endTransaction(); // SPIバス解放
// ここで他のSPIデバイスの使用が可能
// 同じSPIバスの別のデバイス(SDカード等)を使う場合、
// 必ずSPIバスが解放された状態で行ってください。
lcd.beginTransaction(); // SPIバスの確保
lcd.drawPixel(0, 0);    // 描画
lcd.endWrite();         // カウント-1
lcd.endWrite();         // カウント-1、SPIバス解放

// drawPixelとは別に、writePixelという点を描画する関数があります。
// drawPixelは必要に応じてSPIバスの確保を行うのに対し、
// writePixelはSPIバスの状態をチェックしません。
lcd.startWrite(); // SPIバス確保
for (uint32_t x = 0; x < 128; ++x)
{
  for (uint32_t y = 0; y < 128; ++y)
  {
    lcd.writePixel(x, y, lcd.color888(x * 2, x + y, y * 2));
  }
}
lcd.endWrite(); // SPIバス解放
                // 名前が write～ で始まる関数は全て明示的にstartWriteを呼び出しておく必要があります。
                // writePixel、writeFastVLine、writeFastHLine、writeFillRect が該当します。

delay(1000);
*/

  // スプライト（オフスクリーン）への描画も同様の描画関数が使えます。
  // 最初にスプライトの色深度をsetColorDepthで指定します。（省略した場合は16として扱われます。）
  // sprite.setColorDepth(1);   // 1ビット( 2色)パレットモードに設定
  // sprite.setColorDepth(2);   // 2ビット( 4色)パレットモードに設定
  // sprite.setColorDepth(4);   // 4ビット(16色)パレットモードに設定
  // sprite.setColorDepth(8);   // RGB332の8ビットに設定
  // sprite.setColorDepth(16);  // RGB565の16ビットに設定
  sprite.setColorDepth(24); // RGB888の24ビットに設定

  // ※ setColorDepth(8);を設定後に createPalette()を呼ぶ事で、256色パレットモードになります
  // sprite.createPalette();

  // createSpriteで幅と高さを指定してメモリを確保します。
  // 消費するメモリは色深度と面積に比例します。大きすぎるとメモリ確保に失敗しますので注意してください。
  sprite.createSprite(65, 65); // 幅65、高さ65でスプライトを作成。

  // for (uint32_t x = 0; x < 64; ++x) {
  //   for (uint32_t y = 0; y < 64; ++y) {
  //     sprite.drawPixel(x, y, lcd.color888(3 + x*4, (x + y)*2, 3 + y*4));  // スプライトに描画
  //   }
  // }
  sprite.fillRect(0, 0, 65, 65, green);

  // 作成したスプライトはpushSpriteで任意の座標に出力できます。
  // 出力先はインスタンス作成時に引数で渡したLGFXになります。
  sprite.pushSprite(0, 0); // lcdの座標64,0にスプライトを描画
  delay(500);
  sprite.pushSprite(0, 70);
  delay(500);
  sprite.pushSprite(0, 70);

  delay(500);

  // spriteのインスタンス作成時に描画先のポインタを渡していない場合や、
  // 複数のLGFXがある場合などは、出力先を第一引数に指定してpushSpriteすることもできます。
  //  sprite.pushSprite(&lcd, 0, 64);  // lcdの座標0,64にスプライトを描画

  // delay(1000);

  // // pushRotateZoomでスプライトを回転拡大縮小して描画できます。
  // // setPivotで設定した座標が回転中心として扱われ、描画先の座標に回転中心が位置するように描画されます。
  // sprite.setPivot(32, 32);    // 座標32,32を中心として扱う
  // int32_t center_x = lcd.width()/2;
  // int32_t center_y = lcd.height()/2;
  // lcd.startWrite();

  // for (int angle = 0; angle <= 360; ++angle) {
  //   sprite.pushRotateZoom(center_x, center_y, angle, 2.5, 3); // 画面中心に角度angle、幅2.5倍、高さ3倍で描画

  //   if ((angle % 36) == 0) lcd.display(); // 電子ペーパーの場合の表示更新を 36回に一度行う
  // }
  // lcd.endWrite();

  delay(1000);

  /*
    // 使用しなくなったスプライトのメモリを解放するには deleteSprite を使用します。
    sprite.deleteSprite();

    // deleteSprite の後でも、同じインスタンスの再利用が可能です。
    sprite.setColorDepth(4);     // 4ビット(16色)パレットモードに設定
    sprite.createSprite(65, 65);

    // パレットモードのスプライトでは、描画関数の引数の色をパレット番号として扱います。
    // pushSprite等で描画する際に、パレットを参照して実際の描画色が決まります。

    // 4ビット(16色)パレットモードの場合、パレット番号は0～15が使用可能です。
    // パレットの初期色は、0が黒,末尾のパレットが白で、0から末尾にかけてグラデーションになっています。
    // パレットの色を設定するには setPaletteColor を使用します。
    sprite.setPaletteColor(1, 0x0000FFU);    // パレット1番を青に設定
    sprite.setPaletteColor(2, 0x00FF00U);    // パレット2番を緑に設定
    sprite.setPaletteColor(3, 0xFF0000U);    // パレット3番を赤に設定

    sprite.fillRect(10, 10, 45, 45, 1);             // パレット1番で矩形の塗り
    sprite.fillCircle(32, 32, 22, 2);               // パレット2番で円の塗り
    sprite.fillTriangle(32, 12, 15, 43, 49, 43, 3); // パレット3番で三角の塗り

    // pushSpriteの最後の引数で、描画しない色を指定することができます。
    sprite.pushSprite( 0,  0, 0);  // パレット0を透過扱いでスプライトを描画
    sprite.pushSprite(65,  0, 1);  // パレット1を透過扱いでスプライトを描画
    sprite.pushSprite( 0, 65, 2);  // パレット2を透過扱いでスプライトを描画
    sprite.pushSprite(65, 65, 3);  // パレット3を透過扱いでスプライトを描画

    delay(5000);
    */

  lcd.startWrite(); // ここでstartWrite()することで、SPIバスを占有したままにする。
}

void loop111(void)
{
  static int count = 0;
  static int a = 0;
  static int x = 0;
  static int y = 0;
  static float zoom = 3;
  ++count;
  if ((a += 1) >= 360)
    a -= 360;
  if ((x += 2) >= lcd.width())
    x -= lcd.width();
  if ((y += 1) >= lcd.height())
    y -= lcd.height();
  // sprite.setPaletteColor(1, lcd.color888( 0, 0, count & 0xFF));
  // sprite.setPaletteColor(2, lcd.color888( 0,~count & 0xFF, 0));
  // sprite.setPaletteColor(3, lcd.color888( count & 0xFF, 0, 0));

  // sprite.pushRotateZoom(x, 0, 0, 1, 1, 0);
  // sprite.pushSprite(64, 0);

  if ((count % 100) == 0)
    lcd.display(); // 電子ペーパーの場合の表示更新を 100回に一度行う
}

/*
void setup555()
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
*/

/*
void loop555()
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


  // Draw to Display 2: Blue text
  //tft2.fillScreen(TFT_BLACK);  // Clear for demo
  //tft2.setTextColor(TFT_BLUE);
  //tft2.setTextSize(2);
  //tft2.drawString("Hello Display 2!", 20, 100);

  delay(1000); // Update every second
}
*/