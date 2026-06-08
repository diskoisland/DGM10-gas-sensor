#include "DGM10Sensor.h"
#include <stdint.h>

DGM10Sensor::DGM10Sensor(TwoWire &wirePort, uint8_t i2cAddress)
    : wire(wirePort), i2cAddress(i2cAddress) {}

bool DGM10Sensor::begin() {
    wire.begin();
    return true;
}

bool DGM10Sensor::readData(uint8_t registerAddr, uint8_t *data, uint8_t readLen) {
    wire.beginTransmission(i2cAddress);
    wire.write(registerAddr);

    if (wire.endTransmission(false) != 0) {
        return false;
    }

    wire.requestFrom((int)i2cAddress, (int)readLen);

    unsigned long startTime = millis();
    const unsigned long timeoutMs = 100;
    uint8_t index = 0;

    while ((millis() - startTime) < timeoutMs && index < readLen) {
        if (wire.available()) {
            data[index++] = wire.read();
        }
    }

    return index == readLen;
}

bool DGM10Sensor::writeData(uint8_t registerAddr, uint8_t *data, uint8_t writeLen) {
    wire.beginTransmission(i2cAddress);
    wire.write(registerAddr);
    wire.write(data, writeLen);
    return wire.endTransmission() == 0;
}

float DGM10Sensor::convertBytesToFloat(uint8_t *data) {
    if (data == nullptr) {
        return (float)NAN;
    }

    uint32_t temp = (uint32_t)data[0] << 24 |
                    (uint32_t)data[1] << 16 |
                    (uint32_t)data[2] << 8  |
                    (uint32_t)data[3];

    float result;
    memcpy(&result, &temp, sizeof(float));
    return result;
}

bool DGM10Sensor::setLEDState(bool state) {
    uint8_t data = state ? 1 : 0;
    return writeData(LED_STATE_REG, &data, 1);
}

bool DGM10Sensor::readLEDState(bool &state) {
    uint8_t data;

    if (readData(LED_STATE_REG, &data, 1)) {
        state = data != 0;
        return true;
    }

    return false;
}

String DGM10Sensor::translateGasType(uint16_t gasTypeCode) {
    switch (gasTypeCode) {
        case 0x17: return "Formaldehyde (HCHO)";
        case 0x18: return "Organic Volatiles (VOC)";
        case 0x19: return "Carbon Monoxide (CO)";
        case 0x1A: return "Chlorine (Cl2)";
        case 0x1B: return "Hydrogen (H2)";
        case 0x1C: return "Hydrogen Sulfide (H2S)";
        case 0x1D: return "Hydrogen Chloride (HCl)";
        case 0x1E: return "Hydrogen Cyanide (HCN)";
        case 0x1F: return "Hydrogen Fluoride (HF)";
        case 0x20: return "Ammonia (NH3)";
        case 0x21: return "Nitrogen Dioxide (NO2)";
        case 0x22: return "Oxygen (O2)";
        case 0x23: return "Ozone (O3)";
        case 0x24: return "Sulfur Dioxide (SO2)";
        case 0x25: return "Hydrogen Bromide (HBr)";
        case 0x26: return "Bromine (Br2)";
        case 0x32: return "SMELL";
        default:   return "Unknown Gas Type";
    }
}

bool DGM10Sensor::readS0GasSensorType(SafeString &gasType) {
    uint8_t data[2];

    if (readData(S0_GAS_SENSOR_TYPE_REG, data, 2)) {
        uint16_t sensorType = ((uint16_t)data[0] << 8) | (uint16_t)data[1];
        gasType.clear();
        gasType = translateGasType(sensorType).c_str();
        return true;
    }

    return false;
}

bool DGM10Sensor::readS1GasSensorType(SafeString &gasType) {
    uint8_t data[2];

    if (readData(S1_GAS_SENSOR_TYPE_REG, data, 2)) {
        uint16_t sensorType = ((uint16_t)data[0] << 8) | (uint16_t)data[1];
        gasType.clear();
        gasType = translateGasType(sensorType).c_str();
        return true;
    }

    return false;
}

bool DGM10Sensor::isPrintable(uint8_t character) {
    return character >= 32 && character <= 126;
}

bool DGM10Sensor::readS0SensorSerialNumber(SafeString &serialNum) {
    uint8_t data[16];

    if (readData(S0_SENSOR_SERIAL_NUMBER_REG, data, 16)) {
        serialNum.clear();

        for (int i = 0; i < 16; i++) {
            if (isPrintable(data[i])) {
                serialNum += (char)data[i];
            } else {
                break;
            }
        }

        return true;
    }

    return false;
}

bool DGM10Sensor::readS1SensorSerialNumber(SafeString &serialNum) {
    uint8_t data[16];

    if (readData(S1_SENSOR_SERIAL_NUMBER_REG, data, 16)) {
        serialNum.clear();

        for (int i = 0; i < 16; i++) {
            if (isPrintable(data[i])) {
                serialNum += (char)data[i];
            } else {
                break;
            }
        }

        return true;
    }

    return false;
}

bool DGM10Sensor::readSensorLifetimePerformance(uint8_t &s0Performance, uint8_t &s1Performance) {
    uint8_t data;

    if (!readData(SENSOR_LIFETIME_PERFORMANCE_REG, &data, 1)) {
        return false;
    }

    s0Performance = (data >> 4) & 0x0F;
    s1Performance = data & 0x0F;
    return true;
}

bool DGM10Sensor::readS0GasConcentration(float &concentration) {
    uint8_t data[4];

    if (readData(S0_GAS_CONCENTRATION_I2CREG, data, 4)) {
        concentration = convertBytesToFloat(data) * 1000.0f;  // ppb
        return true;
    }

    return false;
}

bool DGM10Sensor::readS1GasConcentration(float &concentration) {
    uint8_t data[4];

    if (readData(S1_GAS_CONCENTRATION_I2CREG, data, 4)) {
        concentration = convertBytesToFloat(data) * 1000.0f;  // ppb
        return true;
    }

    return false;
}

bool DGM10Sensor::readTemperature(float &temperature, bool isFahrenheit) {
    uint8_t data[4];

    if (readData(TEMPERATURE_REG, data, 4)) {
        temperature = convertBytesToFloat(data);

        if (isFahrenheit) {
            temperature = (temperature * 9.0f / 5.0f) + 32.0f;
        }

        return true;
    }

    return false;
}

bool DGM10Sensor::readHumidity(float &humidity) {
    uint8_t data[4];

    if (readData(HUMIDITY_REG, data, 4)) {
        humidity = convertBytesToFloat(data);
        return true;
    }

    return false;
}

bool DGM10Sensor::readS0GasSensorRange(uint32_t &range) {
    uint8_t data[4];

    if (readData(0x0E, data, 4)) {
        range = (uint32_t)data[0] << 24 |
                (uint32_t)data[1] << 16 |
                (uint32_t)data[2] << 8  |
                (uint32_t)data[3];
        return true;
    }

    return false;
}

bool DGM10Sensor::readS1GasSensorRange(uint32_t &range) {
    uint8_t data[4];

    if (readData(0x18, data, 4)) {
        range = (uint32_t)data[0] << 24 |
                (uint32_t)data[1] << 16 |
                (uint32_t)data[2] << 8  |
                (uint32_t)data[3];
        return true;
    }

    return false;
}
