#ifndef birdhouse_sdk_h
#define birdhouse_sdk_h

#include <Arduino.h>

#include <PCF857X.h>
#include <Bounce2.h>

enum BlinkModes {
    BM_TO_END_TO_START = 0,
    BM_TO_END = 1,
};

enum Directions {
    D_START_TO_END = 0,
    D_END_TO_START = 1,
};

#define MAX_SRL_BLINKS 4
#define MAX_RND_BLINKS 4
#define MAX_RELAYS 32
#define DEFAULT_RND_DISABLE_CHANCE 10
class BaseRelay {
public:
    virtual void InitRelays() = 0;
    virtual void TurnRelayByOutNum(uint8_t relayNum, bool isOn) = 0;
    virtual void TurnAllRelays(bool isOn) = 0;
    BaseRelay();
    int StartSerialBlinking(byte relayFromNum, byte relayToNum, uint64_t interval, BlinkModes mode);
    void TurnSerialBlinking(int serialBlinkID, bool isOn);
    void RemoveSerialBlinking(int serialBlinkID);
    int StartRandomBlinking(byte relayFromNum, byte relayToNum, uint64_t minInterval, uint64_t maxInterval, byte maxOn);
    void TurnRandomBlinking(int randomBlinkID, bool isOn);
    void RemoveRandomBlinking(int randomBlinkID);
    void SetRandomBlinkingDisableChance(int randomBlinkID, byte chance);
    void ProcessRelays();
private:
    struct serialBlinkConfig {
        int ID = -1;
        bool isEnabled = false;
        byte relayFromNum = 0;
        byte relayToNum = 0;
        uint64_t interval = 0;
        BlinkModes mode;

        uint64_t lastTime = 0;
        byte lastRelayNum = 0;
        Directions direction = D_START_TO_END;
    };
    struct randomBlinkConfig {
        int ID = -1;
        bool isEnabled = false;
        byte relayFromNum = 0;
        byte relayToNum = 0;
        uint64_t minInterval = 0;
        uint64_t maxInterval = 0;
        byte maxOn = 0;
        byte disableChance = DEFAULT_RND_DISABLE_CHANCE;

        uint64_t lastTime = 0;
        uint64_t currentInterval = 0;
        bool enabledRelay[MAX_RELAYS];
        byte curentEnabledRelaysCount = 0;
    };

    unsigned long currentTime;
    serialBlinkConfig srlBlinks[MAX_SRL_BLINKS];
    int findFirstDisabledSerialBlink();
    randomBlinkConfig rndBlinks[MAX_RND_BLINKS];
    int findFirstDisabledRandomBlink();
    void processSerialBlinking();
    void processRandomBlinking();
};

class Relays: public BaseRelay {
public:
    Relays(const byte *portsList, unsigned long long portsListSize);
    void InitRelays() override;
    void TurnRelayByOutNum(uint8_t relayNum, bool isOn) override;
    void TurnAllRelays(bool isOn) override;
private:
    const byte *portsList;
    unsigned long long portsListSize;
};

#define RELAYS_IN_BLOCK 16
#define MAX_BLOCKS 8
static const byte RelaysExtendedAdresses[] = {0x20, 0x21};
class RelaysExtended: public BaseRelay {
public:
	RelaysExtended(const byte *addressesList, unsigned long long addressesListSize, byte outputsCount);
    void InitRelays() override;
	void TurnRelayByOutNum(uint8_t relayNum, bool isOn) override;
    void TurnAllRelays(bool isOn) override;
private:
	const byte *addressesList;
    unsigned long long addressesListSize;
    PCF857X blocks[MAX_BLOCKS];
    byte outputsCount;
};

#define MAX_BUTTONS 32
#define DEBOUNCE_TIME 5
class Buttons {
public:
    Buttons(const byte *portsList, unsigned long long portsListSize);
    void InitButtons();
    void ProcessButtons();
    bool ReadButtonState(byte btnNum);

    enum ButtonChanges {
        BM_ROSE, //from LOW to HIGH
        BM_FELL //from HIGH to LOW
    };
    typedef void(*OnChange_t)();

    bool IsButtonChange(byte btnNum, ButtonChanges ch); 
    void DoOnButton(byte btnNum, ButtonChanges ch, OnChange_t cb);
private:
    const byte *portsList;
    unsigned long long portsListSize;
    Bounce *buttons;
};

#define MAX_ANALOG_INDICATORS 32
#define MAX_IND_RND_BLINKS 4
class AnalogIndicators {
public:
    AnalogIndicators(byte *portsList, unsigned long long portsListSize);
    void InitAnalogIndicators();
    void SetIndicatorValueByNum(byte indNum, byte value);
    //TODO: values mapping to a real values on indicator
    int StartRandomBlinking(byte indFromNum, byte indToNum, uint64_t minInterval, uint64_t maxInterval, byte minValue, byte maxValue, byte maxIndsOn);
    void TurnRandomBlinking(int randomBlinkID, bool isOn);
    void RemoveRandomBlinking(int randomBlinkID);
    void ProcessIndicators();

    struct randomBlinkConfig {
        int ID = -1;
        bool isEnabled = false;
        byte indFromNum = 0;
        byte indToNum = 0;
        uint64_t minInterval = 0;
        uint64_t maxInterval = 0;
        byte minValue = 0;
        byte maxValue = 0;
        byte maxIndsOn = 0;
        byte maxOn = 0;
        byte disableChance = DEFAULT_RND_DISABLE_CHANCE;

        uint64_t lastTime = 0;
        uint64_t currentInterval = 0;
        byte enabledInd[MAX_ANALOG_INDICATORS];
        byte curentEnabledIndsCount = 0;
    };
private:
    byte *portsList;
    unsigned long long portsListSize;

    unsigned long currentTime;
    randomBlinkConfig rndBlinks[MAX_IND_RND_BLINKS];
    int findFirstDisabledRandomBlink();
    void processRandomBlinking();
};

class Eeprom_at24c256 {
public:
	explicit Eeprom_at24c256(int address);
    bool IsConnected();
	void Write(unsigned  int writeAddress, byte* data, int len);
	void Read(unsigned  int readAdress, byte *buffer, int len);
private:
	int _address;
};

#endif