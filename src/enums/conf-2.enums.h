#pragma once

enum suffixType
{
    CR = 1,
    CRLF,
    TAB
};

enum dataCutSetOut
{
    all = 1,
    firstNByts,
    tailNByts,
    MiddleNBytes
};

enum AIMPrefixType
{
    codeID_aim_custom = 1,
    aim_codeID_custom,
    aim_custom_codeID,
    codeID_custom_aim,
    custom_codeID_aim,
    custom_aim_codeID
};

