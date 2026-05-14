# Multi-Player LED Matrix Roulette

An embedded systems project implementing a digital roulette game using an Arduino microcontroller. The system integrates a multiplexed LED perimeter display, a 4x4 matrix keypad with custom shift-key logic, and a synchronous I2C LCD interface.

---

## System Architecture

The application is built on a state-machine architecture that manages player registration, selection validation, and a simulated physics engine for the "spin" phase.

## Hardware Configuration

| Component | Connection/Pin | Details |
| --- | --- | --- |
| **Microcontroller** | Arduino Uno/Nano | Base controller |
| **LCD Display** | I2C (SDA/SCL) | 16x2 Character Display (Address: 0x27) |
| **Keypad** | Rows: 11, 12, 13, A0 | 4x4 Matrix Configuration |
|  | Cols: A1, A2, A3, 1 |  |
| **LED Matrix** | Rows: 2, 3, 4, 5 | Multiplexed Output |
|  | Cols: 6, 7, 8, 9 |  |
| **Audio** | Pin 10 | PWM-driven Buzzer |

---

## LED Perimeter Mapping

The software maps 12 LEDs into a clockwise perimeter layout. The `spinFrame()` function translates logical positions (0-11) into physical matrix coordinates:

```text
[12] [01] [02] [03]
[11] [  ] [  ] [04]
[10] [  ] [  ] [05]
[09] [08] [07] [06]

```

---

## Technical Implementation Details

### Multiplexing Logic

The LEDs are driven through a custom scanning routine. To minimize pin usage, the LEDs are wired in a 4x4 grid. The software handles row-sinking and column-sourcing to illuminate individual LEDs without ghosting.

### Input Handling & Shift Logic

To support 12 distinct tile selections using a standard numeric keypad, the system implements a "Shift" state:

* **Direct Entry:** Keys 1-9 register the corresponding tile.
* **Shift Entry:** Pressing '0' toggles a `shiftActive` boolean. Subsequent presses of 0, 1, or 2 result in selections 10, 11, or 12.
* **Navigation:** The 'N' key cycles the `currentPlayer` index, while 'S' triggers the game execution logic.

### Spin Simulation Engine

The spin phase utilizes a pseudo-random step generator. It implements a linear deceleration algorithm:

* **Initial Phase:** High-speed rotation with a 10ms delay between steps.
* **Decay Phase:** In the final 15 steps, the delay increases incrementally (+30ms per step), simulating physical friction and momentum loss.
* **Result Calculation:** The final position is determined via `totalSteps % 13` to resolve the 12-point perimeter.

---

## Software Prerequisites

* **Wire.h:** I2C communication protocol.
* **LiquidCrystal_I2C.h:** LCD management.
* **Keypad.h:** Matrix scanning and debounce management.

---

## Setup Instructions

1. **Clone the repository** and open the `.ino` file in the Arduino IDE.
2. **Install required libraries** via the Library Manager.
3. **Wire the LED matrix and keypad** according to the pin definitions provided in the code.
4. **Upload the sketch** to the microcontroller.
