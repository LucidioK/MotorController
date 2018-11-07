#include <LiquidCrystal.h>
#define MENUSELECTORSIMPLE_BTN_RIGHT  50
#define MENUSELECTORSIMPLE_BTN_UP     150
#define MENUSELECTORSIMPLE_BTN_DOWN   300
#define MENUSELECTORSIMPLE_BTN_LEFT   450
#define MENUSELECTORSIMPLE_BTN_SELECT 850
#define MENUSELECTORSIMPLE_BTN_NONE   -1000

LiquidCrystal menuSelectorSimpleLCD(8, 9, 4, 5, 6, 7);

class MenuSelectorSimple {
  public:
    
    MenuSelectorSimple(String *items) {
      serialSSI("MenuSelectorSimple.ctor", "CONSTRUCTOR", millis()/1000);
      menuSelectorSimpleLCD.begin(16, 2);
      _items = items;
      _itemsCount = 0;
      for (int i = 0; _items[i].length() > 0; i++) {
        _itemsCount++;
        serialSSI("MenuSelectorSimple.ctor", _items[i], i);
      }
    };
    
    ~MenuSelectorSimple() {};

    int Select() {
      serialSSI("Select", _items[0], _itemsCount);
      printTitle(_items[0]);
      int pos = 1;
      int btn = MENUSELECTORSIMPLE_BTN_NONE;
      while (btn != MENUSELECTORSIMPLE_BTN_SELECT) {
        printOption(_items[pos]);  
        btn = read_LCD_buttons();
        if (btn != MENUSELECTORSIMPLE_BTN_NONE)
        {
          //serialSSI("Select", "btn", btn);
          switch (btn)
          {
            case MENUSELECTORSIMPLE_BTN_RIGHT: 
            case MENUSELECTORSIMPLE_BTN_UP   : pos = max(1, pos - 1); break;
            case MENUSELECTORSIMPLE_BTN_LEFT : 
            case MENUSELECTORSIMPLE_BTN_DOWN : pos = min(_itemsCount - 1, pos + 1); break;
          }
          serialSSI("Select", _items[pos], pos);
        }
     }
     serialSSS("Select", "returning", _items[pos]);
     return pos;
    }
    
    String SelectString() { 
     return _items[Select()];
    };

    void PrintAt(String s, int col, int row) {
      menuSelectorSimpleLCD.setCursor(col, row);
      while (s.length() < 16) { s += " "; }
      menuSelectorSimpleLCD.print(s);
    }
    
  private:
    String        *_items;
    int           _itemsCount;
    
    void printTitle(String title)   { PrintAt(title, 0, 0); }

    void printOption(String option) { PrintAt(option, 0, 1); }

    int read_LCD_buttons()
    {
     int adc_key_in = analogRead(0);      // read the value from the sensor 
    
     // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
     // we add approx 50 to those values and check to see if we are close
     if (adc_key_in > 1000) return MENUSELECTORSIMPLE_BTN_NONE; // We make this the 1st option for speed reasons since it will be the most likely result
     delay(300);
     // For V1.1 us this threshold
     if (adc_key_in < 50)   return MENUSELECTORSIMPLE_BTN_RIGHT;  
     if (adc_key_in < 150)  return MENUSELECTORSIMPLE_BTN_UP; 
     if (adc_key_in < 300)  return MENUSELECTORSIMPLE_BTN_DOWN; 
     if (adc_key_in < 450)  return MENUSELECTORSIMPLE_BTN_LEFT; 
     if (adc_key_in < 850)  return MENUSELECTORSIMPLE_BTN_SELECT;  
     return MENUSELECTORSIMPLE_BTN_NONE;  // when all others fail, return this...
    }
};
