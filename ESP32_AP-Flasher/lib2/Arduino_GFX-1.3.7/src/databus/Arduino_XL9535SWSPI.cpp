#include "Arduino_XL9535SWSPI.h"

Arduino_XL9535SWSPI::Arduino_XL9535SWSPI(int8_t sda, int8_t scl, int8_t pwd, int8_t cs, int8_t sck, int8_t mosi, TwoWire *wire)
    : _sda(sda), _scl(scl), _pwd(pwd), _cs(cs), _sck(sck), _mosi(mosi), _wire(wire)
{
}

bool Arduino_XL9535SWSPI::begin(int32_t speed, int8_t dataMode)
{
    UNUSED(speed);
    UNUSED(dataMode);

    _address = XL9535_IIC_ADDRESS;
    _wire->beginTransmission(_address);
    if (!_wire->endTransmission())
    {
        Serial.println("Found xl9535");
        is_found = true;

        if (_pwd != GFX_NOT_DEFINED)
        {
            this->pinMode(_pwd, OUTPUT);
        }
        this->pinMode(_cs, OUTPUT);
        this->pinMode(_sck, OUTPUT);
        this->pinMode(_mosi, OUTPUT);

        if (_pwd != GFX_NOT_DEFINED)
        {
            this->digitalWrite(_pwd, 1);
        }
        this->digitalWrite(_cs, 1);
        this->digitalWrite(_sck, 1);
        this->digitalWrite(_mosi, 1);

        // while(1)
        // {
        //     this->digitalWrite(4, 0);
        //     delay(1000);
        //     this->digitalWrite(4, 1);
        //     delay(1000);
        // }

        // this->digitalWrite(_cs, 0);
        // this->digitalWrite(5, 1);
        // delay(100);
        // this->digitalWrite(5, 0);
        // delay(800);
        // this->digitalWrite(5, 1);
        // delay(800);
        // this->digitalWrite(_cs, 1);
    }
    else
    {
        Serial.println("xl9535 not found");
        is_found = false;
    }

    return true;
}

void Arduino_XL9535SWSPI::beginWrite()
{
    this->digitalWrite(_cs, 0);
}

void Arduino_XL9535SWSPI::endWrite()
{
    this->digitalWrite(_cs, 1);
}

void Arduino_XL9535SWSPI::writeCommand(uint8_t c)
{
    // D/C bit, command
    this->digitalWrite(_mosi, 0);
    this->digitalWrite(_sck, 0);
    this->digitalWrite(_sck, 1);

    uint8_t bit = 0x80;
    while (bit)
    {
        if (c & bit)
        {
            this->digitalWrite(_mosi, 1);
        }
        else
        {
            this->digitalWrite(_mosi, 0);
        }
        this->digitalWrite(_sck, 0);
        bit >>= 1;
        this->digitalWrite(_sck, 1);
    }
}

void Arduino_XL9535SWSPI::writeCommand16(uint16_t)
{
}

void Arduino_XL9535SWSPI::write(uint8_t d)
{
    // D/C bit, data
    this->digitalWrite(_mosi, 1);
    this->digitalWrite(_sck, 0);
    this->digitalWrite(_sck, 1);

    uint8_t bit = 0x80;
    while (bit)
    {
        if (d & bit)
        {
            this->digitalWrite(_mosi, 1);
        }
        else
        {
            this->digitalWrite(_mosi, 0);
        }
        this->digitalWrite(_sck, 0);
        bit >>= 1;
        this->digitalWrite(_sck, 1);
    }
}

void Arduino_XL9535SWSPI::write16(uint16_t)
{
    // not implemented
}

void Arduino_XL9535SWSPI::writeRepeat(uint16_t p, uint32_t len)
{
    // not implemented
}

void Arduino_XL9535SWSPI::writePixels(uint16_t *data, uint32_t len)
{
    // not implemented
}

#if !defined(LITTLE_FOOT_PRINT)
void Arduino_XL9535SWSPI::writeBytes(uint8_t *data, uint32_t len)
{
    // not implemented
}
#endif // !defined(LITTLE_FOOT_PRINT)

void Arduino_XL9535SWSPI::writeRegister(uint8_t reg, uint8_t *data, size_t len)
{
    _wire->beginTransmission(_address);
    _wire->write(reg);
    for (size_t i = 0; i < len; i++)
    {
        _wire->write(data[i]);
    }
    _wire->endTransmission();
}

uint8_t Arduino_XL9535SWSPI::readRegister(uint8_t reg, uint8_t *data, size_t len)
{
    _wire->beginTransmission(_address);
    _wire->write(reg);
    _wire->endTransmission();
    _wire->requestFrom(_address, len);
    size_t index = 0;
    while (index < len)
        data[index++] = _wire->read();
    return 0;
}

void Arduino_XL9535SWSPI::pinMode(uint8_t pin, uint8_t mode)
{
    if (is_found)
    {
        uint8_t port = 0;
        if (pin > 7)
        {
            this->readRegister(XL9535_CONFIG_PORT_1_REG, &port, 1);
            if (mode == OUTPUT)
            {
                port = port & (~(1 << (pin - 10)));
            }
            else
            {
                port = port | (1 << (pin - 10));
            }
            this->writeRegister(XL9535_CONFIG_PORT_1_REG, &port, 1);
        }
        else
        {
            this->readRegister(XL9535_CONFIG_PORT_0_REG, &port, 1);
            if (mode == OUTPUT)
            {
                port = port & (~(1 << pin));
            }
            else
            {
                port = port | (1 << pin);
            }
            this->writeRegister(XL9535_CONFIG_PORT_0_REG, &port, 1);
        }
    }
    else
    {
        Serial.println("xl9535 not found");
    }
}
void Arduino_XL9535SWSPI::pinMode8(uint8_t port, uint8_t pin, uint8_t mode)
{
    if (is_found)
    {
        uint8_t _pin = (mode != OUTPUT) ? pin : ~pin;
        if (port)
        {
            this->writeRegister(XL9535_CONFIG_PORT_1_REG, &_pin, 1);
        }
        else
        {
            this->writeRegister(XL9535_CONFIG_PORT_0_REG, &_pin, 1);
        }
    }
    else
    {
        Serial.println("xl9535 not found");
    }
}

void Arduino_XL9535SWSPI::digitalWrite(uint8_t pin, uint8_t val)
{
    if (is_found)
    {
        uint8_t port = 0;
        uint8_t reg_data = 0;
        if (pin > 7)
        {
            this->readRegister(XL9535_OUTPUT_PORT_1_REG, &reg_data, 1);
            reg_data = reg_data & (~(1 << (pin - 10)));
            port = reg_data | val << (pin - 10);
            this->writeRegister(XL9535_OUTPUT_PORT_1_REG, &port, 1);
        }
        else
        {
            this->readRegister(XL9535_OUTPUT_PORT_0_REG, &reg_data, 1);
            reg_data = reg_data & (~(1 << pin));
            port = reg_data | val << pin;
            this->writeRegister(XL9535_OUTPUT_PORT_0_REG, &port, 1);
        }
    }
    else
    {
        Serial.println("xl9535 not found");
    }
}

int Arduino_XL9535SWSPI::digitalRead(uint8_t pin)
{
    if (is_found)
    {
        int state = 0;
        uint8_t port = 0;
        if (pin > 7)
        {
            this->readRegister(XL9535_INPUT_PORT_1_REG, &port, 1);
            state = port >> (pin - 10) & 0x01 ? 1 : 0;
        }
        else
        {
            this->readRegister(XL9535_INPUT_PORT_0_REG, &port, 1);
            state = port >> pin & 0x01 ? 1 : 0;
        }
        return state;
    }
    else
    {
        Serial.println("xl9535 not found");
    }
    return 0;
}
