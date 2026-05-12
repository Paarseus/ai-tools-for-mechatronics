# Using AI Tools in Mechatronics Projects

Guest lecture given to a mechatronics class at Cal Poly Pomona. The audience was mostly mechanical engineering students with little prior coding experience, so the talk focuses on workflow and common pitfalls rather than software theory.

## Slides

- [`ai_mechatronics_lecture.pdf`](ai_mechatronics_lecture.pdf) — compiled slides
- [`ai_mechatronics_lecture.tex`](ai_mechatronics_lecture.tex) — Beamer source (metropolis theme)
- [`images/`](images) — figures used in the slides

To rebuild the PDF:

```bash
pdflatex ai_mechatronics_lecture.tex
pdflatex ai_mechatronics_lecture.tex
```

## Live demo materials

- [`demo_prompts.md`](demo_prompts.md) — the prompt scripts used during the live coding portion, plus the pre-lecture hardware checklist
- [`PWMTest.ino`](PWMTest.ino) — known-working reference firmware for the Teensy 4.1, used as a backup in case live coding fails

## Hardware used in the demos

- Teensy 4.1
- DC gear motor with quadrature encoder (5760 counts/rev)
- Dual-PWM H-bridge motor driver (RPWM on pin 3, LPWM on pin 4)
- Encoder channels A and B on pins 2 and 7
- Bench power supply for the motor; logic powered over USB

## Topics covered

1. What agentic CLI tools (Gemini CLI, Claude Code) actually do
2. Installation, including WSL setup for Windows
3. Writing prompts that produce runnable embedded code
4. Common failure modes — wrong pin numbers, fabricated library functions, unit errors — and how to catch them
5. Live coding a PID position controller from an empty folder
6. Two follow-on demos that reuse the same firmware:
   - Webcam hand tracking driving the motor (MediaPipe)
   - Local voice control (OpenAI Whisper)

## Tools and libraries referenced

- [Gemini CLI](https://github.com/google-gemini/gemini-cli)
- [Claude Code](https://claude.com/claude-code)
- [arduino-cli](https://arduino.github.io/arduino-cli/)
- [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html)
- [Encoder library (PaulStoffregen)](https://github.com/PaulStoffregen/Encoder)
- [PID_v1 library (br3ttb)](https://github.com/br3ttb/Arduino-PID-Library)
- [MediaPipe Hands](https://google.github.io/mediapipe/)
- [OpenAI Whisper](https://github.com/openai/whisper)

## Acknowledgments

Thanks to the Autonomous Vehicle Laboratory at Cal Poly Pomona for the hardware and bench time used while preparing the demos.

## Author

Parsa Ghasemi — Computer Engineering, Cal Poly Pomona
GitHub: [@Paarseus](https://github.com/Paarseus)
