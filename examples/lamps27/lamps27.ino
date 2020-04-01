#include <birdhouse_sdk.h>

RelaysExtended relays = RelaysExtended(RelaysExtendedAdresses, sizeof(RelaysExtendedAdresses), 27);
const byte burronsPorts[3] = {7, 8, 9};
Buttons buttons = Buttons(burronsPorts, sizeof(burronsPorts));

int r1, r2, s1;
void setup() {
  buttons.InitButtons();
  relays.InitRelays();

  r1 = relays.StartRandomBlinking(0, 10, 500, 1000, 2);
  relays.TurnRandomBlinking(r1, false);

  r2 = relays.StartRandomBlinking(11, 19, 1000, 2000, 4);
  relays.TurnRandomBlinking(r2, false);

  s1 = relays.StartSerialBlinking(20, 27, 1000, BM_TO_END);
  relays.TurnSerialBlinking(s1, false);
}

void loop() {
  relays.ProcessRelays();
  buttons.ProcessButtons();
  
  buttons.DoOnButton(0, Buttons::BM_ROSE, []() {
    relays.TurnRandomBlinking(r1, true);
  });
  buttons.DoOnButton(0, Buttons::BM_FELL, []() {
    relays.TurnRandomBlinking(r1, false);
  });

  buttons.DoOnButton(1, Buttons::BM_ROSE, []() {
    relays.TurnRandomBlinking(r2, true);
  });
  buttons.DoOnButton(1, Buttons::BM_FELL, []() {
    relays.TurnRandomBlinking(r2, false);
  });

  buttons.DoOnButton(2, Buttons::BM_ROSE, []() {
    relays.TurnSerialBlinking(s1, true);
  });
  buttons.DoOnButton(2, Buttons::BM_FELL, []() {
    relays.TurnSerialBlinking(s1, false);
  });
}

