#include "birdhouse_sdk.h"

Relays::Relays(const byte *portsList, unsigned long long portsListSize) {
    //BaseRelay();
    this->portsList = portsList;
    this->portsListSize = portsListSize;
}

void Relays::InitRelays() {
    for (byte i=0; i<portsListSize; i++) {
        pinMode(i, OUTPUT);
        digitalWrite(i, HIGH); //turn off out
    }
}

void Relays::TurnRelayByOutNum(uint8_t relayNum, bool isOn) {
    if (relayNum < portsListSize) {
        digitalWrite(portsList[relayNum], isOn ? LOW : HIGH);
    }
}

void Relays::TurnAllRelays(bool isOn) {
    for (byte i=0; i<portsListSize; i++) {
        TurnRelayByOutNum(i, isOn);
    }
}