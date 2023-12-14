#pragma once

enum workMode
{
    keyTrig = 1,
    edgeTrig,
    cmdTrig,
    incuction,
    noMoveNoRead,
    continuous
};

enum outMode
{
    UART_TTL = 1,
    keyboard,
    disk,
    virtualCom,
    pos,
    composite,
    UART_TTL_keyboard
};

enum fillLed
{
    allwaysOff = 1,
    getPictureOn,
    allwaysON
};

enum locateLed
{
    allwaysOff = 1,
    getPictureOn,
    allwaysON
};

enum buzzerType
{
    passive = 1,
    active,
    mimicry
};

enum activeBuzzerWorkLevel
{
    high = 1,
    low
};

enum buzzerFreq
{
    kHz1_7 = 1,
    kHz2_0,
    kHz2_7,
    kHz3_7
};

enum ctrlCharMode {
    off = 1,
    ctrlMode,
    altMode,
    notOut
};

enum capsLockMode {
    normal = 1,
    turn,
    allwaysOn,
    allwaysOff
};

enum dataOutType {
    GBK = 1,
    Unicode,
    Raw,
    UTF8,
    BIG5
};

enum placeMode {
    bracket = 1,
    handle
};

enum uartParity {
    none = 1,
    odd,
    even
};