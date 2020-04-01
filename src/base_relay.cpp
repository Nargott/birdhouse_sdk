#include "birdhouse_sdk.h"

BaseRelay::BaseRelay() {
    //do nothing now
}

int BaseRelay::findFirstDisabledSerialBlink() {
    for (byte i=0; i<MAX_SRL_BLINKS; i++) {
        if (srlBlinks[i].ID < 0) {
            return i;
        }
    }

    return -1;
}

int BaseRelay::StartSerialBlinking(byte relayFromNum, byte relayToNum, uint64_t interval, BlinkModes mode) {
    int idx = findFirstDisabledSerialBlink();
    if (idx < 0) {
        return -1;
    }

    srlBlinks[idx].ID = idx;
    srlBlinks[idx].isEnabled = true;
    srlBlinks[idx].relayFromNum = relayFromNum;
    srlBlinks[idx].relayToNum = relayToNum;
    srlBlinks[idx].interval = interval;
    srlBlinks[idx].mode = mode;

    srlBlinks[idx].lastTime = 0;
    srlBlinks[idx].direction = D_START_TO_END;

    return idx;
}

void BaseRelay::TurnSerialBlinking(int serialBlinkID, bool isOn) {
    if ((serialBlinkID < 0) || (srlBlinks[serialBlinkID].ID < 0))  {
        return;
    }

    srlBlinks[serialBlinkID].isEnabled = isOn;

    if (!srlBlinks[serialBlinkID].isEnabled) { //turn off
        for (byte i=srlBlinks[serialBlinkID].relayFromNum; i<=srlBlinks[serialBlinkID].relayToNum; i++) {
            TurnRelayByOutNum(i, false);
            srlBlinks[serialBlinkID].direction = D_START_TO_END;
            srlBlinks[serialBlinkID].lastTime = 0;
            srlBlinks[serialBlinkID].lastRelayNum = 0;
        }
    }
}

void BaseRelay::RemoveSerialBlinking(int serialBlinkID) {
    if ((serialBlinkID < 0) || (srlBlinks[serialBlinkID].ID < 0))  {
        return;
    }

    TurnSerialBlinking(serialBlinkID, false);
    srlBlinks[serialBlinkID].ID = 0;
}

int BaseRelay::findFirstDisabledRandomBlink() {
    for (byte i=0; i<MAX_RND_BLINKS; i++) {
        if (rndBlinks[i].ID < 0) {
            return i;
        }
    }

    return -1;
}

void BaseRelay::SetRandomBlinkingDisableChance(int randomBlinkID, byte chance) {
    if (chance > 0) {
        rndBlinks[randomBlinkID].disableChance = chance;
    }
}

int BaseRelay::StartRandomBlinking(byte relayFromNum, byte relayToNum, uint64_t minInterval, uint64_t maxInterval, byte maxLampsOn) {
    int idx = findFirstDisabledRandomBlink();
    if (idx < 0) {
        return -1;
    }

    rndBlinks[idx].ID = idx;
    rndBlinks[idx].isEnabled = true;
    rndBlinks[idx].relayFromNum = relayFromNum;
    rndBlinks[idx].relayToNum = relayToNum;
    rndBlinks[idx].minInterval = minInterval;
    rndBlinks[idx].maxInterval = maxInterval;
    rndBlinks[idx].maxOn = maxLampsOn;
    rndBlinks[idx].disableChance = DEFAULT_RND_DISABLE_CHANCE;
    if (rndBlinks[idx].maxOn > (rndBlinks[idx].relayToNum-rndBlinks[idx].relayFromNum)) rndBlinks[idx].maxOn = rndBlinks[idx].relayToNum-rndBlinks[idx].relayFromNum;

    rndBlinks[idx].lastTime = 0;
    rndBlinks[idx].currentInterval = random(rndBlinks[idx].minInterval, rndBlinks[idx].maxInterval);

    for (byte i=rndBlinks[idx].relayFromNum; i<=rndBlinks[idx].relayToNum; i++) {
        rndBlinks[idx].enabledRelay[i] = false;
    }

    return idx;
}

void BaseRelay::TurnRandomBlinking(int randomBlinkID, bool isOn) {
    if ((randomBlinkID < 0) || (rndBlinks[randomBlinkID].ID < 0))  {
        return;
    }

    rndBlinks[randomBlinkID].isEnabled = isOn;

    if (!rndBlinks[randomBlinkID].isEnabled) { //turn off
        for (byte i=rndBlinks[randomBlinkID].relayFromNum; i<=rndBlinks[randomBlinkID].relayToNum; i++) {
            TurnRelayByOutNum(i, false);
            rndBlinks[randomBlinkID].curentEnabledRelaysCount = 0;
            rndBlinks[randomBlinkID].enabledRelay[i] = false;
        }
    }
}

void BaseRelay::RemoveRandomBlinking(int randomBlinkID) {
    if ((randomBlinkID < 0) || (rndBlinks[randomBlinkID].ID < 0))  {
        return;
    }

    TurnRandomBlinking(randomBlinkID, false);
    rndBlinks[randomBlinkID].ID = -1;
}

void BaseRelay::processSerialBlinking() {
    for (byte idx=0; idx<MAX_SRL_BLINKS; idx++) {
        if (srlBlinks[idx].ID < 0) {
            continue;
        }

        if ((srlBlinks[idx].isEnabled) && ((currentTime - srlBlinks[idx].lastTime) > srlBlinks[idx].interval)) {
            switch (srlBlinks[idx].mode) {
                case BM_TO_END_TO_START: {
                    if (srlBlinks[idx].lastRelayNum == 0) {
                        srlBlinks[idx].lastRelayNum = srlBlinks[idx].relayFromNum;
                    }

                    if (srlBlinks[idx].direction == D_START_TO_END) {
                        TurnRelayByOutNum(srlBlinks[idx].lastRelayNum, true);
                    }

                    if (srlBlinks[idx].direction == D_END_TO_START) {
                        TurnRelayByOutNum(srlBlinks[idx].lastRelayNum, false);
                    }

                    if (srlBlinks[idx].direction == D_START_TO_END) {
                        if (srlBlinks[idx].lastRelayNum < srlBlinks[idx].relayToNum) {
                            srlBlinks[idx].lastRelayNum++;
                        } else {
                            srlBlinks[idx].direction = D_END_TO_START;
                        }
                    }
                    if (srlBlinks[idx].direction == D_END_TO_START) {
                        if (srlBlinks[idx].lastRelayNum > srlBlinks[idx].relayFromNum) {
                            srlBlinks[idx].lastRelayNum--;
                        } else {
                            srlBlinks[idx].direction = D_START_TO_END;
                        }
                    }
                } break;
                case BM_TO_END: {
                    if (srlBlinks[idx].lastRelayNum == 0) {
                        srlBlinks[idx].lastRelayNum = srlBlinks[idx].relayFromNum;
                    }

                    TurnRelayByOutNum(srlBlinks[idx].lastRelayNum, true);

                    if (srlBlinks[idx].lastRelayNum < srlBlinks[idx].relayToNum) {
                        srlBlinks[idx].lastRelayNum++;
                    } else {
                        //turn off
                        for (byte i=srlBlinks[idx].relayFromNum; i<=srlBlinks[idx].relayToNum; i++) {
                            TurnRelayByOutNum(i, false);
                        }
                        srlBlinks[idx].lastRelayNum = srlBlinks[idx].relayFromNum;
                    }
                } break;
            }

            srlBlinks[idx].lastTime = currentTime;
        }
    }
}

void BaseRelay::processRandomBlinking() {
    for (byte idx=0; idx<MAX_RND_BLINKS; idx++) {
        if (rndBlinks[idx].ID < 0) {
            continue;
        }

        if ((rndBlinks[idx].isEnabled) && ((currentTime - rndBlinks[idx].lastTime) > rndBlinks[idx].currentInterval)) {
            byte rnd;
            byte cycles = 0;
            while(cycles <= (rndBlinks[idx].relayToNum-rndBlinks[idx].relayFromNum)) {
                rnd = random(rndBlinks[idx].relayFromNum, rndBlinks[idx].relayToNum+1);
                if (rndBlinks[idx].enabledRelay[rnd]) {
                    cycles++;
                    continue;
                } //try to enable only disabled relays
                
                TurnRelayByOutNum(rnd, true);
                rndBlinks[idx].curentEnabledRelaysCount++;
                rndBlinks[idx].enabledRelay[rnd] = true;
                break;
            }

            //check for carry-over
            bool isNeedDisableSomething = false;
            if (rndBlinks[idx].curentEnabledRelaysCount > rndBlinks[idx].maxOn) {
                isNeedDisableSomething = true;
            }

            //disable some of enabled relays
            //TODO: process disableCount
            //byte disableCount = random(1, rndBlinks[idx].curentEnabledRelaysCount);
            byte isDisable;
            for (byte d = 0; d<rndBlinks[idx].curentEnabledRelaysCount; d++) { //cycle to go thought random disabling relays
                isDisable = random(0, rndBlinks[idx].disableChance);
                if (isNeedDisableSomething) {
                    isDisable = 0;
                    isNeedDisableSomething = false;
                }
                if (isDisable != 0) continue; //nothing to do
                
                //disable some random one
                cycles = 0;
                while((rndBlinks[idx].curentEnabledRelaysCount > 1) && (cycles < 100)) {
                    byte rd = random(rndBlinks[idx].relayFromNum, rndBlinks[idx].relayToNum+1);
                    if ((!rndBlinks[idx].enabledRelay[rd]) || (rd == rnd)) { //ignore enabled and do not disturb currently enabled
                        continue;
                        cycles++;
                    }

                    TurnRelayByOutNum(rd, false);
                    rndBlinks[idx].enabledRelay[rd] = false;
                    if (rndBlinks[idx].curentEnabledRelaysCount > 0) rndBlinks[idx].curentEnabledRelaysCount--;
                    break;
                }
            }

            rndBlinks[idx].currentInterval = random(rndBlinks[idx].minInterval, rndBlinks[idx].maxInterval);
            rndBlinks[idx].lastTime = currentTime;
        }
    }
}

void BaseRelay::ProcessRelays() {
    currentTime = millis();

    processSerialBlinking();
    processRandomBlinking();
}

