uint8_t display_begin() {
    pinMode(MISO, INPUT);
    pinMode(MOSI, OUTPUT);
    pinMode(SCK, OUTPUT);
    pinMode(CS, OUTPUT);
    pinMode(RESET, OUTPUT);
    pinMode(HRDY, INPUT);

    uint8_t err = IT8951_Init();

    return err == 0;
}

void display_buffer(uint8_t* addr, uint32_t x, uint32_t y, uint32_t w, uint32_t h, int mode = 2) {
    gpFrameBuf = addr;
    Serial.println("Displaying image");
    IT8951_BMP_Example(x, y, w, h); // needed to draw images at any positions other than 0,0
    IT8951DisplayArea(x, y, w, h, mode);
    Serial.println("done");
}
