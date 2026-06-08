# DGM10-gas-sensor

Arduino library for the ECsense DGM10 dual-slot electrochemical gas sensor, developed for the EPA CH4 monitoring system.

## Overview

The DGM10 supports two independent gas sensor slots (S0 and S1) and communicates over I2C. It measures gas concentration, temperature, humidity, and sensor lifetime performance status.

## Features

- Read S0 and S1 gas concentration (ppb)
- Read sensor serial numbers and gas types
- Read sensor range
- Read temperature and humidity
- Read sensor lifetime performance status (working / close to failure / failure)
- Set and read LED state
- I2C address configurable (default `0x50`)
- Supports multiple Wire buses (`Wire`, `Wire1`, etc.)

## Dependencies

- [SafeString](https://github.com/PowerBroker2/SafeString) — used for serial number and gas type string handling

## Hardware

- **Interface:** I2C
- **Default address:** `0x50`
- **Tested with:** Teensy MicroMod (IMXRT1062)

## Usage

```cpp
#include <Wire.h>
#include <SafeString.h>
#include "DGM10Sensor.h"

DGM10Sensor dgm10(Wire);

createSafeString(s0Serial, 25);
createSafeString(s0Gas, 15);

void setup() {
    Wire.begin();
    dgm10.begin();
    dgm10.readS0SensorSerialNumber(s0Serial);
    dgm10.readS0GasSensorType(s0Gas);
}

void loop() {
    float concentration;
    dgm10.readS0GasConcentration(concentration);
    delay(5000);
}
```

## Sensor lifetime performance codes

| Value | Meaning |
|---|---|
| 0 | Working |
| 1 | Close to failure — prepare replacement |
| 2 | Failure — replace sensor |
| 3 | Unknown |

## License

MIT License — Copyright 2026 Ross Edwards. See [LICENSE](LICENSE) for details.
