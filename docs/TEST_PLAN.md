# Bench test checklist

Use this checklist to document actual observations. All results below are **not recorded** until tested on the hardware; expected behaviour is derived from the committed code.

Record the date, Git commit, camera/receiver board models, ESP32 core version, IDE or PlatformIO configuration, camera sensor, PSRAM settings, receiver MAC, power supply, LED wiring and test lighting.

## Existing sketches

| Test | Procedure | Expected behaviour | Recorded result |
| --- | --- | --- | --- |
| Receiver startup | Reset receiver | Three active-high indicator LEDs on; STA MAC printed | Not recorded |
| Synthetic left | Run sender test | Zone 1 received; left off, other LEDs on | Not recorded |
| Synthetic centre | Continue sender cycle | Zone 2 received; centre off, other LEDs on | Not recorded |
| Synthetic right | Continue sender cycle | Zone 3 received; right off, other LEDs on | Not recorded |
| Synthetic no target | Continue sender cycle | No-target message received; all LEDs on | Not recorded |
| Camera left/centre/right | Run camera sketch and move a red marker | Serial zone follows the centroid's image third | Not recorded |
| Camera no target | Remove marker | No-target serial output when red-pixel count is at most 100 | Not recorded |
| Boundary behaviour | Move marker slowly across image thirds | Observe switching and record any flicker; no hysteresis is implemented | Not recorded |
| Multiple red objects | Place red objects in different thirds | Record combined-centroid behaviour | Not recorded |
| Lighting variation | Repeat at several documented light levels | Record classification failures and any threshold changes | Not recorded |
| Communication loss | Stop sender during a detected-zone state | Receiver retains the last state; no timeout is implemented | Not recorded |

## After integration

These tests require a combined camera/wireless sketch that is not currently committed.

- Move the marker through all three zones and confirm the corresponding LED turns off.
- Remove the marker and confirm all LEDs return on.
- Reset each board independently and record recovery behaviour.
- Implement and verify the selected communication-loss timeout and fallback state.
- After adding input validation, verify truncated packets and invalid zone values are rejected.

## Evidence to capture

Record a short continuous video showing the marker and all three LEDs. Include left, centre, right and no-target conditions. Keep original serial logs with the commit and board settings.

For timing measurements, define the start/end events and instrument used. A programmed delay is not a measured end-to-end latency. For classification checks, record the number of trials, correct results and lighting conditions before reporting a percentage.

## Results template

| Date / commit | Test | Conditions / repetitions | Observation | Pass / fail / limitation | Evidence |
| --- | --- | --- | --- | --- | --- |
| — | — | — | — | — | — |
