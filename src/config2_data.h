#pragma once

#include <boost/json.cpp>
#include <string>
#include "enums/conf-2.enums.h"

class config2_data
{
    bool endCharEnable_;
    bool suffixEnable_;
    bool codeIDEnable_;
    bool prefixEnable_;
    bool RFEnable_;
    suffixType suffixType_;
    bool GSExchange_;
    bool urlCodeReadEnable_;
    bool code128Lead_;
    bool QRLead_;
    unsigned int GSExchangeChar_;
    bool TTL_VCOMOutProtocol_;
    std::string prefix_;
    std::string suffix_;
    std::string RFInfo_;
    std::string codeID_;
    dataCutSetOut dataCutSetOut_;
    unsigned int dataCutBeforeLen_;
    unsigned int dataCutAfterLen_;
    bool AIMPrefixEnable_;
    AIMPrefixType AIMPrefixType_;
    unsigned int GMCodeID_;
    int productType_;
    std::string softVersion_;
    std::string date_;
    bool lrcOutEnable_;
    bool strExchangeEnable_;
    std::string strExchangeBefore_;
    std::string strExchangeAfter_;
    std::string deviceID;
    std::string MID;
    std::string FID;
};