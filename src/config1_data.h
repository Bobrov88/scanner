#pragma once

#include <boost/json.cpp>
#include <string>
#include "enums/conf-1.enums.h"

class config1_data
{
    workMode workMode_;
    std::string trigCmd_;
    outMode outMode_;
    fillLed fillLed_;
    locateLed locateLed_;
    buzzerType buzzerType_;
    unsigned short buzzerOnTime_;
    activeBuzzerWorkLevel activeBuzzerWorkLevel_;
    buzzerFreq buzzerFreq_;
    bool devStatBuzzer_;
    bool decodeSucessBuzzer_;
    bool setCodeBuzzer_;
    bool continuousSwitch_;
    bool decodeLed_;
    bool ledWorkLevel_;
    bool ledOnTime_;
    bool mute_;
    bool cmdTrigAck_;
    bool trigSignal_;
    bool setCodeEnable_;
    bool printSetCode_;
    unsigned int steadyPictureTime_;
    unsigned int readInterval_;
    unsigned int longestReadTime_;
    ctrlCharMode ctrlCharMode_;
    bool disableChinese_;
    unsigned int hidPolling_;
    unsigned int keyboardKeyDelay_;
    bool keyboardLead_;
    bool keypadCalcSingal_;
    bool keypadNumLock_;
    capsLockMode capsLockMode_;
    bool codeTypeAutoCheckUtf8_;
    std::string codePage_;
    std::string keyboardLayout_;
    dataOutType dataOutType_;
    bool virtualKeyboard_;
    bool UART_TTLOutKbdProtocol_;
    bool sensitivity1_;
    bool sensitivity2_;
    unsigned int sameReadDelay_;
    bool sameReadDelayState_;
    unsigned int inductionReadDelay_;
    placeMode placeMode_;
    bool motor_;
    bool reinforceRead_;
    bool reflectProcess_;
    bool keyContinuous_;
    uartParity uartParity_;
    unsigned int uartBaud_;
    unsigned short uartDataBits_;
    unsigned short uartStopBits_;
};