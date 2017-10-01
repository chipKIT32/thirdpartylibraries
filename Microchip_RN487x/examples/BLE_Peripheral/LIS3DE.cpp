#include "LIS3DE.h"

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

LIS3DE::LIS3DE(TwoWire& wire, uint8_t address): _wire(wire), _address(address), _scale(Scale2g)
{

}

int8_t LIS3DE::getTemperatureDelta()
{
    setRegisterBits(CTRL_REG4, _BV(BDU));
    
    readRegister(OUT_ADC3_L); // low byte can be thrown away, but must be read!
    int8_t value = readRegister(OUT_ADC3_H);
    
    unsetRegisterBits(CTRL_REG4, _BV(BDU));

    return value;
}

void LIS3DE::setScale(Scale scale)
{
    writeRegister(CTRL_REG4, (scale << FS0));
    _scale = scale;
}

double LIS3DE::getGsFromScaledValue(int8_t value)
{
    int8_t scaleMax = getScaleMax(_scale);
    return mapDouble(value, INT8_MIN, INT8_MAX, -scaleMax, scaleMax);
}

int8_t LIS3DE::getScaledValueFromGs(double gValue)
{
    int8_t scaleMax = getScaleMax(_scale);
    return trunc(mapDouble(gValue, -scaleMax, scaleMax, INT8_MIN, INT8_MAX));
}

int8_t LIS3DE::getScaleMax(Scale scale)
{
    return (1 << (scale + 1));
}

void LIS3DE::enable(bool isLowPowerEnabled, ODR odr, Axes axes, Scale scale, bool isTemperatureOn)
{
    // set odr, mode, enabled axes
    writeRegister(CTRL_REG1, (odr << ODR0) | (isLowPowerEnabled << LPen) | axes);
    
    setScale(scale);

    if (isTemperatureOn) {
        // enable aux ADC and temperature sensor
        writeRegister(TEMP_CFG_REG, _BV(ADC_PD) | _BV(TEMP_EN));
    }
    else {
        // disable aux ADC and temperature sensor
        writeRegister(TEMP_CFG_REG, 0);
    }
}

void LIS3DE::disable()
{
    enable(true, PowerDown, NoAxis, _scale, false);
}

void LIS3DE::reboot()
{
    writeRegister(CTRL_REG5, _BV(BOOT));
}

void LIS3DE::setRegisterBits(Register reg, uint8_t byteValue)
{
    uint8_t value = readRegister(reg);
    value |= byteValue;
    writeRegister(reg, value);
}

void LIS3DE::unsetRegisterBits(Register reg, uint8_t byteValue)
{
    uint8_t value = readRegister(reg);
    value &= ~byteValue;
    writeRegister(reg, value);
}

void LIS3DE::enableInterrupt1(uint8_t axesEvents, double threshold, uint8_t duration, InterruptMode interruptMode)
{
    // setup the interrupt
    writeRegister(IG1_CFG, interruptMode | (axesEvents & 0b00111111));
    writeRegister(IG1_THS, getScaledValueFromGs(threshold));
    writeRegister(IG1_DURATION, duration);

    // disable latching
    unsetRegisterBits(CTRL_REG5, _BV(LIR_IG1));

    // enable interrupt generator 1 on INT1
    setRegisterBits(CTRL_REG3, _BV(INT1_IG1));
}

void LIS3DE::disableInterrupt1()
{
    // disable interrupt generator 1
    unsetRegisterBits(CTRL_REG3, _BV(INT1_IG1));
}

void LIS3DE::enableInterrupt2(uint8_t axesEvents, double threshold, uint8_t duration, InterruptMode interruptMode)
{
    // setup the interrupt
    writeRegister(IG2_CFG, interruptMode | (axesEvents & 0b00111111));
    writeRegister(IG2_THS, getScaledValueFromGs(threshold));
    writeRegister(IG2_DURATION, duration);

    // disable latching
    unsetRegisterBits(CTRL_REG5, _BV(LIR_IG2));

    // enable interrupt generator 2 on INT2
    setRegisterBits(CTRL_REG6, _BV(INT2_IG2));
}

void LIS3DE::disableInterrupt2()
{
    // enable interrupt generator 2 on INT2
    unsetRegisterBits(CTRL_REG6, _BV(INT2_IG2));
}

uint8_t LIS3DE::readRegister(uint8_t reg)
{
    Wire.beginTransmission(_address);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    
    Wire.requestFrom(_address, 1);
    
    return Wire.read();
}

void LIS3DE::writeRegister(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission((uint8_t)_address);
    
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    
    Wire.endTransmission();
}
