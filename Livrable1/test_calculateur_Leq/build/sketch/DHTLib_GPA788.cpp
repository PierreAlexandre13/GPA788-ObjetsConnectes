#line 1 "/home/pa/Documents/ETS/objetsConnectes/labo4/test_calculateur_Leq/DHTLib_GPA788.cpp"
#include "DHTLib_GPA788.h"


// return values:
// DHTLIB_OK
// DHTLIB_ERROR_CHECKSUM
// DHTLIB_ERROR_TIMEOUT
bool dhtlib_gpa788::isConnected(){
    // Retourne true si le capteur DHT11 fonctionne correctement
    if(humidity == static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE) && 
    temperature == static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE)){
        return false;
    }
    return true;
}

void dhtlib_gpa788::reset(){
    // Assigner DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE dans les variables humidity et temperature.
    humidity = static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE);
    temperature = static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE);
}

void dhtlib_gpa788::disConnect(){
    // DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE dans les variables humidity ettemperature.
    humidity = static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE);
    temperature = static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE);
}

void dhtlib_gpa788::Connect(){
    // Permettre la lecture du capteur DHT11
    

}


DHTLIB_ErrorCode dhtlib_gpa788::read11(uint8_t pin)
{
    // READ VALUES
    DHTLIB_ErrorCode rv = _readSensor(pin, DHTLIB_DHT11_WAKEUP);
    if (rv != DHTLIB_ErrorCode::DHTLIB_OK)
    {
        humidity    = static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE); // invalid value, or is NaN prefered?
        temperature = static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE); // invalid value
        return rv;
    }

    // CONVERT AND STORE
    humidity    = bits[0];  // bits[1] == 0;
    temperature = bits[2];  // bits[3] == 0;

    // TEST CHECKSUM
    // bits[1] && bits[3] both 0
    // Certains capteurs DHT11 transmettent le checksum dont la valeur inclut
    // la partie fractionnaire de la température et l'humidité relative
    // d'où la modification apportée ici. (TW, 2019)
    uint8_t sum = bits[0] + bits[2] + bits[1] + bits[3];
    if (bits[4] != sum) return DHTLIB_ErrorCode::DHTLIB_ERROR_CHECKSUM;

    return DHTLIB_ErrorCode::DHTLIB_OK;
}


// return values:
// DHTLIB_OK
// DHTLIB_ERROR_CHECKSUM
// DHTLIB_ERROR_TIMEOUT
DHTLIB_ErrorCode dhtlib_gpa788::read(uint8_t pin)
{
    // READ VALUES
    DHTLIB_ErrorCode rv = _readSensor(pin, DHTLIB_DHT_WAKEUP);
    if (rv != DHTLIB_ErrorCode::DHTLIB_OK)
    {
        humidity    = static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE);  // invalid value, or is NaN prefered?
        temperature = static_cast<double>(DHTLIB_ErrorCode::DHTLIB_INVALID_VALUE);  // invalid value
        return rv; // propagate error value
    }

    // CONVERT AND STORE
    humidity = word(bits[0], bits[1]) * 0.1;
    temperature = word(bits[2] & 0x7F, bits[3]) * 0.1;
    if (bits[2] & 0x80)  // negative temperature
    {
        temperature = -temperature;
    }

    // TEST CHECKSUM
    uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
    if (bits[4] != sum)
    {
        return DHTLIB_ErrorCode::DHTLIB_ERROR_CHECKSUM;
    }
    return DHTLIB_ErrorCode::DHTLIB_OK;
}

/////////////////////////////////////////////////////
//
// PRIVATE
//

// return values:
// DHTLIB_OK
// DHTLIB_ERROR_TIMEOUT
DHTLIB_ErrorCode dhtlib_gpa788::_readSensor(uint8_t pin, uint8_t wakeupDelay)
{
    // INIT BUFFERVAR TO RECEIVE DATA
    uint8_t mask = 128;
    uint8_t idx = 0;

    // replace digitalRead() with Direct Port Reads.
    // reduces footprint ~100 bytes => portability issue?
    // direct port read is about 3x faster
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *PIR = portInputRegister(port);

    // EMPTY BUFFER
    for (uint8_t i = 0; i < 5; i++) bits[i] = 0;

    // REQUEST SAMPLE
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // T-be 
    delay(wakeupDelay);
    digitalWrite(pin, HIGH);   // T-go
    delayMicroseconds(40);
    pinMode(pin, INPUT);

    // GET ACKNOWLEDGE or TIMEOUT
    uint16_t loopCntLOW = DHTLIB_TIMEOUT;
    while ((*PIR & bit) == LOW )  // T-rel
    {
        if (--loopCntLOW == 0) return DHTLIB_ErrorCode::DHTLIB_ERROR_TIMEOUT;
    }

    uint16_t loopCntHIGH = DHTLIB_TIMEOUT;
    while ((*PIR & bit) != LOW )  // T-reh
    {
        if (--loopCntHIGH == 0) return DHTLIB_ErrorCode::DHTLIB_ERROR_TIMEOUT;
    }

    // READ THE OUTPUT - 40 BITS => 5 BYTES
    for (uint8_t i = 40; i != 0; i--)
    {
        loopCntLOW = DHTLIB_TIMEOUT;
        while ((*PIR & bit) == LOW )
        {
            if (--loopCntLOW == 0) return DHTLIB_ErrorCode::DHTLIB_ERROR_TIMEOUT;
        }

        uint32_t t = micros();

        loopCntHIGH = DHTLIB_TIMEOUT;
        while ((*PIR & bit) != LOW )
        {
            if (--loopCntHIGH == 0) return DHTLIB_ErrorCode::DHTLIB_ERROR_TIMEOUT;
        }

        if ((micros() - t) > 40)
        { 
            bits[idx] |= mask;
        }
        mask >>= 1;
        if (mask == 0)   // next byte?
        {
            mask = 128;
            idx++;
        }
    }
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);

    return DHTLIB_ErrorCode::DHTLIB_OK;
}
//
// END OF FILE
//
