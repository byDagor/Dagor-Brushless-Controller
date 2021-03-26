---
layout: default
title: Void setup - tab c
parent: Firmware
nav_order: 3
permalink: /void_setup
---

# Void Setup

```c++
void setup() {
    Serial.begin(115200);

    Serial.println("DAGOR: INIT");

    gpio_init();
    spi_init();

    _delay(250);
    drv_init();

    SimpleFOCinit();

    Serial.println("DAGOR: Ready BLDC.");
}
```
