#ifndef LIS3DE_H_
#define LIS3DE_H_

#include "Arduino.h"
#include <Wire.h>

#define LIS3DE_ADDRESS 0b0101000

#define _BV(bit) (1 << (bit))

class LIS3DE
{
    public:
    
        enum Register {
            STATUS_REG_AUX = 0x07,
            OUT_ADC1_L = 0x08,
            OUT_ADC1_H = 0x09,
            OUT_ADC2_L = 0x0A,
            OUT_ADC2_H = 0x0B,
            OUT_ADC3_L = 0x0C,
            OUT_ADC3_H = 0x0D,
            INT_COUNTER_REG = 0x0E,
            WHO_AM_I = 0x0F,
            TEMP_CFG_REG = 0x1F,
            CTRL_REG1 = 0x20,
            CTRL_REG2 = 0x21,
            CTRL_REG3 = 0x22,
            CTRL_REG4 = 0x23,
            CTRL_REG5 = 0x24,
            CTRL_REG6 = 0x25,
            REFERENCE = 0x26,
            STATUS_REG2 = 0x27,
            OUT_X = 0x29,
            OUT_Y = 0x2B,
            OUT_Z = 0x2D,
            FIFO_CTRL_REG = 0x2E,
            FIFO_SRC_REG = 0x2F,
            IG1_CFG = 0x30,
            IG1_SOURCE = 0x31,
            IG1_THS = 0x32,
            IG1_DURATION = 0x33,
            IG2_CFG = 0x34,
            IG2_SOURCE = 0x35,
            IG2_THS = 0x36,
            IG2_DURATION = 0x37,
            CLICK_CFG = 0x38,
            CLICK_SRC = 0x39,
            CLICK_THS = 0x3A,
            TIME_LIMIT = 0x3B,
            TIME_LATENCY = 0x3C,
            TIME_WINDOW = 0x3D,
            ACT_THS = 0x3E,
            ACT_DUR = 0x3F
        };
        
        enum Bits {
            // CTRL_REG1
            ODR0 = 4,
            LPen = 3,
            Zen = 2,
            Yen = 1,
            Xen = 0,

            // CTRL_REG3
            INT1_CLICK = 7,
            INT1_IG1 = 6,
            INT1_IG2 = 5,
            INT1_DRDY1 = 4,
            INT1_DRDY2 = 3,
            INT1_WTM = 2,
            INT1_OVERRUN = 1,

            // CTRL_REG4
            BDU = 7,
            FS0 = 4,
            ST0 = 1,
            SIM = 0,

            // CTRL_REG5
            BOOT = 7,
            FIFO_EN = 6,
            LIR_IG1 = 3,
            D4D_IG1 = 2,
            LIR_IG2 = 1,
            D4D_IG2 = 0,

            // CTRL_REG6
            INT2_CLICK = 7,
            INT2_IG1 = 6,
            INT2_IG2 = 5,
            INT2_BOOT = 4,
            INT2_ACT = 3,
            H_LACTIVE = 1,

            // TEMP_CFG_REG
            ADC_PD = 7,
            TEMP_EN = 6,
        };

        enum ODR {
            PowerDown = 0b000,
            NormalLowPower1Hz = 0b0001,
            NormalLowPower10Hz = 0b0010,
            NormalLowPower25Hz = 0b0011,
            NormalLowPower50Hz = 0b0100,
            NormalLowPower100Hz = 0b0101,
            NormalLowPower200Hz = 0b0110,
            NormalLowPower400Hz = 0b0111,
            LowPower1k6Hz = 0b1000,
            Normal1k344LowPower5k376Hz = 0b1001
        };
        
        enum Axes {
            NoAxis = 0,
            X = 0b001,
            Y = 0b010,
            Z = 0b100,
            XY = X | Y,
            XZ = X | Z,
            YZ = Y | Z,
            XYZ = X | Y | Z
        };
        
        enum Scale {
            Scale2g = 0,
            Scale4g = 0b01,
            Scale8g = 0b10,
            Scale16g = 0b11
        };

        enum AxesEvents {
            ZHigh = _BV(5),
            ZLow = _BV(4),
            YHigh = _BV(3),
            YLow = _BV(2),
            XHigh = _BV(1),
            XLow = _BV(0)
        };

        enum InterruptMode {
            OrCombination = 0b00000000,
            MovementRecognition = 0b01000000,
            AndCombination = 0b10000000,
            PositionRecognition = 0b11000000
        };

        LIS3DE(TwoWire& wire = Wire, uint8_t address = LIS3DE_ADDRESS);
        int8_t getTemperatureDelta();
        void enable(bool isLowPowerEnabled = false, ODR odr = NormalLowPower25Hz, Axes axes = XYZ, Scale scale = Scale2g, bool isTemperatureOn = true);
        void disable();
        void reboot();

        void enableInterrupt1(uint8_t axesEvents, double threshold, uint8_t duration, InterruptMode interruptMode = MovementRecognition);
        void disableInterrupt1();
        void enableInterrupt2(uint8_t axesEvents, double threshold, uint8_t duration, InterruptMode interruptMode = MovementRecognition);
        void disableInterrupt2();
        
        double getX() { return getGsFromScaledValue(readRegister(LIS3DE::OUT_X)); };
        double getY() { return getGsFromScaledValue(readRegister(LIS3DE::OUT_Y)); };
        double getZ() { return getGsFromScaledValue(readRegister(LIS3DE::OUT_Z)); };

        uint8_t readRegister(uint8_t reg);
        void writeRegister(uint8_t reg, uint8_t value);
        void setRegisterBits(Register reg, uint8_t byteValue);
        void unsetRegisterBits(Register reg, uint8_t byteValue);
protected:
        TwoWire& _wire;
        uint8_t _address;
        Scale _scale;

        void setScale(Scale scale);

        double getGsFromScaledValue(int8_t value);
        int8_t getScaledValueFromGs(double gValue);
        int8_t getScaleMax(Scale scale);
};

#endif
