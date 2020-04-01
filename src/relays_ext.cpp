#include "birdhouse_sdk.h"

RelaysExtended::RelaysExtended(const byte *addressesList, unsigned long long addressesListSize, byte outputsCount) {
    //BaseRelay();
    this->addressesList = addressesList;
    this->addressesListSize = addressesListSize;
    if (this->addressesListSize > MAX_BLOCKS) {
        this->addressesListSize = MAX_BLOCKS;
    }
    this->outputsCount = outputsCount;
}

void RelaysExtended::InitRelays() {
    for (byte i=0; i<addressesListSize; i++) {
        blocks[i].begin(addressesList[i]);
        for (byte r=0; r<RELAYS_IN_BLOCK; r++) {
            if ((i*RELAYS_IN_BLOCK+r) > outputsCount) {
                return; //cant'do anything after this cycles by this line!!!
            }

            blocks[i].pinMode(r, OUTPUT);
            blocks[i].digitalWrite(r, HIGH); //turn off out
        }
    }
}

void RelaysExtended::TurnRelayByOutNum(uint8_t relayNum, bool isOn) {
    if (relayNum >= outputsCount) {
        return;
    }
    byte block = relayNum / RELAYS_IN_BLOCK;
    if (block >= addressesListSize) block = addressesListSize;
    byte relayInBlock = relayNum % RELAYS_IN_BLOCK;

    blocks[block].digitalWrite(relayInBlock, isOn ? LOW : HIGH);
}

void RelaysExtended::TurnAllRelays(bool isOn) {
    for (byte i=0; i<outputsCount; i++) {
        TurnRelayByOutNum(i, isOn);
    }
}