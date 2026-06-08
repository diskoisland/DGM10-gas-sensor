/*!
 *  @file DGM10Sensor.h
 *  @brief Arduino library for the ECsense DGM10 dual-gas sensor
 *
 *  @author Ross Edwards
 *
 *  @license MIT License — Copyright 2026 Ross Edwards
 *
 *  @section VERSION HISTORY
 *
 *  1.0 - Dec, 2023 - Initial release.
 *  2.0 - Jun, 2026 - Removed ArduinoLog dependency. Added readS0/S1GasSensorRange().
 */

#ifndef DGM10SENSOR_H
#define DGM10SENSOR_H

#include <Wire.h>
#include <Arduino.h>
#include <SafeString.h>

class DGM10Sensor {
public:
    DGM10Sensor(TwoWire &wirePort, uint8_t i2cAddress = 0x50);

    bool begin();

    bool readTemperature(float &temperature, bool isFahrenheit = false);
    bool readHumidity(float &humidity);
    bool readS0GasConcentration(float &concentration);
    bool readS1GasConcentration(float &concentration);
    bool readS0SensorSerialNumber(SafeString &serialNum);
    bool readS1SensorSerialNumber(SafeString &serialNum);
    bool readS0GasSensorType(SafeString &gasType);
    bool readS1GasSensorType(SafeString &gasType);
    bool readS0GasSensorRange(uint32_t &range);
    bool readS1GasSensorRange(uint32_t &range);
    bool setLEDState(bool state);
    bool readLEDState(bool &state);
    bool readSensorLifetimePerformance(uint8_t &s0Performance, uint8_t &s1Performance);

private:
    TwoWire &wire;
    uint8_t i2cAddress;

    bool readData(uint8_t registerAddr, uint8_t *data, uint8_t readLen);
    bool writeData(uint8_t registerAddr, uint8_t *data, uint8_t writeLen);
    float convertBytesToFloat(uint8_t *data);
    String translateGasType(uint16_t gasTypeCode);
    bool isPrintable(uint8_t character);

    static const uint8_t SENSOR_LIFETIME_PERFORMANCE_REG = 0x00;
    static const uint8_t LED_STATE_REG                   = 0x01;
    static const uint8_t TEMPERATURE_REG                 = 0x02;
    static const uint8_t HUMIDITY_REG                    = 0x06;
    static const uint8_t S0_GAS_SENSOR_TYPE_REG          = 0x12;
    static const uint8_t S1_GAS_SENSOR_TYPE_REG          = 0x1C;
    static const uint8_t S0_SENSOR_SERIAL_NUMBER_REG     = 0x36;
    static const uint8_t S1_SENSOR_SERIAL_NUMBER_REG     = 0x46;
    static const uint8_t S0_GAS_CONCENTRATION_I2CREG     = 0x0A;
    static const uint8_t S1_GAS_CONCENTRATION_I2CREG     = 0x14;
};

#endif  // DGM10SENSOR_H
