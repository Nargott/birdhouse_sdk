#include "birdhouse_sdk.h"

AnalogIndicators::AnalogIndicators(byte *portsList, unsigned long long portsListSize) {
    this->portsList = portsList;
    this->portsListSize = portsListSize;
    if (this->portsListSize > MAX_ANALOG_INDICATORS) this->portsListSize = MAX_ANALOG_INDICATORS;
    //TODO: check if each port can be used as PWM?
}

void AnalogIndicators::InitAnalogIndicators() {
    for (byte i=0; i<portsListSize; i++) {
        pinMode(portsList[i], OUTPUT);
    }
}

void AnalogIndicators::SetIndicatorValueByNum(byte indNum, byte value) {
    if (indNum >= portsListSize) indNum = portsListSize-1; //use last one

    analogWrite(portsList[indNum], (int)value);
}

int AnalogIndicators::findFirstDisabledRandomBlink() {
    for (byte i=0; i<MAX_RND_BLINKS; i++) {
        if (rndBlinks[i].ID == 0) {
            return i;
        }
    }

    return -1;
}

int AnalogIndicators::StartRandomBlinking(byte indFromNum, byte indToNum, uint64_t minInterval, uint64_t maxInterval, byte minValue, byte maxValue, byte maxIndsOn) {
    int idx = findFirstDisabledRandomBlink();
    if (idx < 0) {
        return -1;
    }

    rndBlinks[idx].ID = idx;
    rndBlinks[idx].isEnabled = true;
    rndBlinks[idx].indFromNum = indFromNum;
    rndBlinks[idx].indToNum = indToNum;
    rndBlinks[idx].minInterval = minInterval;
    rndBlinks[idx].maxInterval = maxInterval;
    rndBlinks[idx].minValue = minValue;
    rndBlinks[idx].maxValue = maxValue;
    rndBlinks[idx].maxIndsOn = maxIndsOn;
    if (rndBlinks[idx].maxIndsOn > (rndBlinks[idx].indToNum-rndBlinks[idx].indFromNum)) rndBlinks[idx].maxIndsOn = rndBlinks[idx].indToNum-rndBlinks[idx].indFromNum;

    rndBlinks[idx].lastTime = 0;
    rndBlinks[idx].currentInterval = random(rndBlinks[idx].minInterval, rndBlinks[idx].maxInterval);

    for (byte i=rndBlinks[idx].indFromNum; i<=rndBlinks[idx].indToNum; i++) {
        rndBlinks[idx].enabledInd[i] = 0;
    }

    return idx;
}

void AnalogIndicators::TurnRandomBlinking(int randomBlinkID, bool isOn) {
    if ((randomBlinkID < 0) || (rndBlinks[randomBlinkID].ID < 0))  {
        return;
    }

    rndBlinks[randomBlinkID].isEnabled = isOn;

    if (!rndBlinks[randomBlinkID].isEnabled) { //turn off
        for (byte i=rndBlinks[randomBlinkID].indToNum; i<=rndBlinks[randomBlinkID].indFromNum; i++) {
            SetIndicatorValueByNum(i, 0);
            rndBlinks[randomBlinkID].curentEnabledIndsCount = 0;
            rndBlinks[randomBlinkID].enabledInd[i] = 0;
        }
    }
}

void AnalogIndicators::RemoveRandomBlinking(int randomBlinkID) {
    if ((randomBlinkID < 0) || (rndBlinks[randomBlinkID].ID < 0))  {
        return;
    }

    TurnRandomBlinking(randomBlinkID, false);
    rndBlinks[randomBlinkID].ID = 0;
}

void AnalogIndicators::processRandomBlinking() {
    for (byte idx=0; idx<MAX_IND_RND_BLINKS; idx++) {
        if (rndBlinks[idx].ID < 0) {
            continue;
        }

        if ((rndBlinks[idx].isEnabled) && ((currentTime - rndBlinks[idx].lastTime) > rndBlinks[idx].currentInterval)) {
            byte rnd = random(rndBlinks[idx].indFromNum, rndBlinks[idx].indToNum+1);
            byte rndVal = random(rndBlinks[idx].minValue, rndBlinks[idx].maxValue+1);
            
            byte cycles = 0;
            while(cycles <= (rndBlinks[idx].indToNum-rndBlinks[idx].indFromNum)) {
                rnd = random(rndBlinks[idx].indFromNum, rndBlinks[idx].indToNum+1);
                if (rndBlinks[idx].enabledInd[rnd] > 0) {
                    cycles++;
                    continue;
                } //try to enable only disabled relays
                
                SetIndicatorValueByNum(rnd, rndVal);
                rndBlinks[idx].curentEnabledIndsCount++;
                rndBlinks[idx].enabledInd[rnd] = rndVal;
                break;
            }
            

            //check for carry-over
            bool isNeedDisableSomething = false;
            if (rndBlinks[idx].curentEnabledIndsCount > rndBlinks[idx].maxIndsOn) {
                isNeedDisableSomething = true;
            }

            //disable some of enabled relays
            //TODO: process disablecount
            //byte disableCount = random(1, rndBlinks[idx].curentEnabledIndsCount);
            byte isDisable;
            for (byte d = 0; d<rndBlinks[idx].curentEnabledIndsCount; d++) { //cycle to go thought random disabling relays
                isDisable = random(0, rndBlinks[idx].disableChance);
                if (isNeedDisableSomething) {
                    isDisable = 0;
                    isNeedDisableSomething = false;
                }
                if (isDisable != 0) continue; //nothing to do
                
                //disable some random one
                cycles = 0;
                while((rndBlinks[idx].curentEnabledIndsCount > 1) && (cycles < 100)) {
                    byte rd = random(rndBlinks[idx].indFromNum, rndBlinks[idx].indToNum+1);
                    if ((!rndBlinks[idx].enabledInd[rd]) || (rd == rnd)) { //ignore enabled and do not disturb currently enabled
                        continue;
                        cycles++;
                    }

                    SetIndicatorValueByNum(rd, false);
                    rndBlinks[idx].enabledInd[rd] = false;
                    if (rndBlinks[idx].curentEnabledIndsCount > 0) rndBlinks[idx].curentEnabledIndsCount--;
                    break;
                }
            }

            rndBlinks[idx].currentInterval = random(rndBlinks[idx].minInterval, rndBlinks[idx].maxInterval);
            rndBlinks[idx].lastTime = currentTime;
        }
    }
}

void AnalogIndicators::ProcessIndicators() {
    currentTime = millis();

    processRandomBlinking();
}