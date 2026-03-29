# Pulse Looper Card

A simple trigger looper for the Workshop Computer module. It records the **timing between incoming pulses** while the Record input is held high, then continuously replays that timing as a loop when recording stops. Playback timing can be stretched or compressed with the Speed knob, output pulse length is adjustable, and each event can be randomly skipped using the Probability control. A Reset input restarts the loop from the beginning.

CAVEAT: Almost entirely vibe-coded, builds but doesn't seem to record anything. Very much a work in progress.

---

## Controls & I/O

* **IN 0 (Pulse In):** trigger/gate input to record

* **IN 1 (Reset):** restarts loop playback

* **IN 2 (Record):** HIGH = record, LOW = play

* **OUT 0 (Pulse Out):** looped trigger output

* **Switch:** Start recording
 

* **Big knob (Speed):** scales playback timing (slower ↔ faster)

* **X knob (Width):** sets output pulse length

* **Y knob (Probability):** chance each trigger will fire

---

## Usage

* Hold **Record** high and send pulses into **IN 0**
* Release Record to begin looping
* Adjust knobs to shape playback
* Send a pulse to **Reset** to resync the loop

---

## Notes

* Stores up to ~4000 timing events
* Very fast pulses may be ignored
* Output is a digital trigger/gate signal

---:::
