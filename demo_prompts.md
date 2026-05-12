# Demo Prompt Scripts

All prompts are for **Gemini CLI** (or Claude Code) running in a terminal. Assumes `arduino-cli` is installed and Teensy core is set up.

---

## Pre-lecture checklist

- [ ] `gemini` works from terminal
- [ ] `arduino-cli board list` shows the Teensy
- [ ] Motor wired: RPWM on pin 3, LPWM on pin 4, encoder on pins 2 and 7
- [ ] Bench PSU on, laptop on battery (avoids ground loops)
- [ ] Webcam working (for Demo 1)
- [ ] Headset/mic tested (for Demo 2)
- [ ] Backup video of each demo on the desktop
- [ ] This file open in a second window you can glance at

---

## Warm-up (2 min, optional)

Small "it works" moment before the real coding starts.

### Prompt W1
> I have a Teensy 4.1 on `/dev/ttyACM0`. Write a minimal Arduino sketch that blinks the built-in LED at 2 Hz, save it as `blink/blink.ino`, then compile with arduino-cli for fqbn `teensy:avr:teensy41` and upload it.

*(On Windows native: use `COM3` or whichever port Device Manager shows.)*

### What to say
"Notice I'm not typing any code. I'm describing what I want. The CLI wrote the file, compiled it, and flashed it. That's the workflow we'll use for everything today."

---

## Main live coding: Motor firmware from scratch (15 min)

**Goal:** build a full position-controlled motor with PID, live on stage, starting from an empty folder. This is the core of the lecture.

### Wiring (matches `PWMTest.ino` in this folder)
- Motor driver: **RPWM on pin 3, LPWM on pin 4** (dual-PWM H-bridge style)
- Encoder: **channel A on pin 2, channel B on pin 7**, both with `INPUT_PULLUP`
- Gearbox motor with **5760 counts per revolution**
- Motor power from bench PSU, logic from USB
- PWM at 20 kHz (quiet, no audible whine)

### Backup plan
If live coding collapses, `PWMTest.ino` in this folder is the known-working reference. Flash it directly:
```
arduino-cli compile --fqbn teensy:avr:teensy41 PWMTest.ino
arduino-cli upload -p /dev/ttyACM0 --fqbn teensy:avr:teensy41 PWMTest.ino
```

### Prompt style: loose first, specific if it goes wrong

Each step has **a loose prompt** (try this first — let the AI make reasonable choices) and a **detailed fallback** (use this if the loose version produces wrong code).

The loose version makes a better demo: the audience sees the AI reasoning, not you dictating. Only fall back when you actually need to.

---

### Step 1 — Basic motor spin

#### Prompt M1 — loose
> I have a Teensy 4.1 with a dual-PWM H-bridge driver. RPWM is pin 3, LPWM is pin 4. Write a short Arduino sketch that cycles the motor: forward, stop, reverse, on a few-second loop. Use a quiet PWM frequency so the motor doesn't whine. Save it as `motor/motor.ino`, compile with arduino-cli for `teensy:avr:teensy41`, and upload to `/dev/ttyACM0`.

#### Fallback M1 — detailed
> Use 20 kHz PWM via `analogWriteFrequency` on both pins. For forward, set LPWM to ~128 and RPWM to 0. For backward, swap them. Hold each state for 2 seconds, stop between for 1 second.

**Verify:** motor spins forward, stops, reverses, repeats. No whine means the frequency is right.

---

### Step 2 — Read the encoder

#### Prompt M2 — loose
> Now add encoder reading. I have a quadrature encoder on pins 2 and 7. It's on a gear motor with 5760 counts per revolution. Print the current angle in degrees so I can see it change when I turn the shaft.

#### Fallback M2 — detailed
> Use the Arduino `Encoder` library. Set both encoder pins to `INPUT_PULLUP`. Print the angle at 10 Hz over Serial while the motor keeps cycling.

**Verify:** with the motor off, turn the shaft by hand. The angle should count up and down in `arduino-cli monitor -p /dev/ttyACM0`.

---

### Step 3 — Accept setpoints over serial

#### Prompt M3 — loose
> Change the sketch: stop the forward/backward cycling, and instead listen on serial for a target angle from my computer. Store it as the setpoint. Keep printing the current angle and the setpoint so I can see what's happening.

#### Fallback M3 — detailed
> Expect lines of the form `setpoint: <number>\n` where `<number>` is degrees. Store it in a `double`. No control loop yet — just echo setpoint and current angle at 10 Hz.

**Verify:** type `setpoint: 90` into the serial monitor. The sketch should echo it back.

---

### Step 4 — Add the PID controller

#### Prompt M4 — loose
> Add a position PID controller. The input is the current encoder angle, the target is the setpoint from serial, and the output should drive the H-bridge. Start with conservative gains and briefly explain your choice. Route positive output to one motor pin and negative to the other.

#### Fallback M4 — detailed
> Use the `PID_v1` library. Sample time 20 ms. Output clamped to [-255, 255]. Try Kp = 7.0, Ki = 2.0, Kd = 1.75. On positive output: `analogWrite(LPWM, output)` and `analogWrite(RPWM, 0)`. On negative: swap.

**Verify:** `setpoint: 90` → shaft rotates to 90° and holds. `setpoint: 0` → it returns.

---

### Step 5 — Tune (only if needed)

#### Prompt M5
> The motor is [overshooting / buzzing / sluggish]. Suggest better gains for a small gear motor with roughly 50 ms settling time. Update the sketch.

---

### Step 6 — The disturbance moment

Send `setpoint: 90`. Let it settle. Then try to push the shaft by hand. The PID pushes back.

### What to say
"I never wrote a line of PID code. I told the CLI what I had and what I wanted. It picked the library, the sample rate, and the gains. I read each line as it appeared, caught one thing it got wrong about the encoder, and fixed it with a follow-up prompt."

---

### Common failure: runaway

If the motor accelerates away from the setpoint instead of settling, the encoder is reading the opposite sign from what the controller expects.

#### Recovery prompt
> The motor runs away when I send a setpoint. Probably the encoder direction is flipped relative to the motor. Invert the sign of the encoder reading and reflash.

**Keep one hand on the bench PSU kill switch while testing PID for the first time.**

---

## Demo 1: Vision → motor (10 min)

**Goal:** laptop webcam sees your hand, motor follows hand position.

### Before the demo
- Install once: `pip install mediapipe opencv-python pyserial`
- Have the Teensy already flashed with a basic PID position controller that reads setpoints over serial (pre-built, not live). Keep the `.ino` ready as a reference file.

### Prompt 1 — build the vision script

> Write a Python script called `hand_to_motor.py` that uses MediaPipe Hands to track a single hand from the default webcam. Print the y-coordinate of the wrist landmark at 30 Hz, normalized to a range of 0 to 1000 where the top of the frame is 0 and the bottom is 1000.

### Prompt 2 — add serial

> Now modify the script to also send that value over USB serial to `/dev/ttyACM0` at 115200 baud, as a plain-text integer followed by a newline. Keep the print for debugging.

### Prompt 3 — run it

> Run the script.

### Prompt 4 — if motor doesn't move
Paste the Teensy's serial output or the Python error, then:

> Here's what I see when I run it: [paste]. Tell me what's wrong and fix it.

### Prompt 5 — polish (if time)

> Add a visualization: show the webcam feed with a dot on the tracked wrist and the numeric setpoint overlaid.

### What to say
"MediaPipe is a neural network Google trained on millions of hand images. I did not train anything. I just asked the CLI to glue it to my motor."

---

## Demo 2: Voice → motor (10 min)

**Goal:** say a command, motor reacts.

### Before the demo
- Install once: `pip install openai-whisper sounddevice numpy pyserial`
- Whisper will download its model on first run (~140 MB for `base`). Do this BEFORE the lecture.
- Test your microphone works with `python -c "import sounddevice; print(sounddevice.query_devices())"`.

### Prompt 1 — build the listener

> Write a Python script `voice_motor.py` that records 3 seconds of audio from the default microphone when I press Enter, then uses Whisper's `base` model to transcribe it locally, and prints the text.

### Prompt 2 — parse commands

> Extend the script to parse the transcript for motor commands. If it contains "forward" spin the motor at +50% PWM. If it contains "backward" spin at -50%. If it contains "stop" set to 0. If it contains "fast" use 90% of whichever direction. Send the signed integer PWM to `/dev/ttyACM0` at 115200 baud.

### Prompt 3 — run it

> Run the script.

### Prompt 4 — if Whisper is too slow
Whisper `base` is ~1 sec per 3 sec of audio. If it feels sluggish:

> Switch it to the `tiny` model for faster transcription. Accuracy drop is fine.

### Prompt 5 — audience interaction

Ask a volunteer to come up. Tell them the keywords: **forward, backward, stop, fast**.

### What to say
"Speech recognition was an unsolved problem in AI for forty years. It's now a `pip install` away, running entirely on my laptop, no internet."

---

## Universal fallback prompts

### When something is broken and you don't know why

> Here is the code: [paste]. Here is the error or output I'm seeing: [paste]. What's wrong?

### When the code doesn't compile

> This didn't compile. Here's the exact error message: [paste]. Fix it.

### When the motor behaves wrong

> The motor is [overshooting / jittering / not moving / spinning wrong direction]. Here's the controller code: [paste]. What should I change?

### When you need to kill the demo gracefully

> Show me the keyboard shortcut to kill this Python script safely and stop the motor.

---

## Prompts that WON'T work well (avoid on stage)

- "Build me a voice-controlled robot." *(too vague, will ramble)*
- "Make it better." *(no direction)*
- Any prompt longer than 3 sentences that you haven't tested beforehand.

---

## Rehearsal note

Run every one of these prompts **at least twice** the day before the lecture. Not because the CLI is unreliable, but because **you** need to know what the output looks like so you can talk through it without reading it for the first time in front of people.
