#include "birdhouse_sdk.h"

Buttons::Buttons(const byte *portsList, unsigned long long portsListSize) {
    this->portsList = portsList;
    this->portsListSize = portsListSize;
    if (this->portsListSize > MAX_BUTTONS) this->portsListSize = MAX_BUTTONS;

    buttons = new Bounce[this->portsListSize];
}

void Buttons::InitButtons() {
    for (byte i=0; i<portsListSize; i++) {
        buttons[i].attach(portsList[i], INPUT);
        buttons[i].interval(DEBOUNCE_TIME);
    }
}

void Buttons::ProcessButtons() {
    for (byte i=0; i<portsListSize; i++) {
        buttons[i].update();
    }
}

bool Buttons::ReadButtonState(byte btnNum) {
    if (btnNum >= portsListSize) btnNum = portsListSize-1; //set to the last one
    return buttons[btnNum].read();
}

bool Buttons::IsButtonChange(byte btnNum, ButtonChanges ch) {
    switch (ch) {
    case BM_ROSE:
        return buttons[btnNum].rose();
        break;
    case BM_FELL:
        return buttons[btnNum].fell();
    break;
    default:
        return false;
        break;
    }
}

void Buttons::DoOnButton(byte btnNum, Buttons::ButtonChanges ch, OnChange_t cb) {
    if(!cb) return;

    switch (ch) {
    case BM_ROSE:
        if (buttons[btnNum].rose()) cb();
        break;
    case BM_FELL:
        if (buttons[btnNum].fell()) cb();
    break;
    default:
    break;
    }
}