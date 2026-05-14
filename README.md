# Multi-Player LED Matrix Roulette

An embedded systems project implementing a digital roulette game using an Arduino microcontroller. This project demonstrates direct GPIO manipulation to drive a peripheral matrix without external drivers, alongside a multi-player state-machine and custom audio feedback.

---

## System Architecture

The application is built on a state-machine architecture that manages player registration, selection validation, and a simulated physics engine for the "spin" phase.

## Hardware Configuration

### Bill of Materials

* **Microcontroller:** Arduino Uno R3
* **Display:** 16x2 LCD with I2C Interface Driver (Address: `0x27`)
* **LED Interface:** 8x8 LED Matrix (Driven directly via GPIO, no external driver)
* **Input:** 4x4 Matrix Keypad
* **Audio:** Passive Buzzer (PWM-controlled for custom frequency melodies)
* **Interconnects:** Male-to-Male, Female-to-Male, and Female-to-Female jumper wires

### Pin Mapping

| Component | Connection/Pin | Details |
| --- | --- | --- |
| **Microcontroller** | Arduino Uno R3 | Core Processor |
| **LCD Display** | I2C (SDA/SCL) | Uses A4/A5 on Uno R3 |
| **Keypad** | Rows: 11, 12, 13, A0 | 4x4 Matrix Scanning |
|  | Cols: A1, A2, A3, 1 |  |
| **LED Matrix** | Rows: 2, 3, 4, 5 | Direct Multiplexed Output |
|  | Cols: 6, 7, 8, 9 |  |
| **Audio** | Pin 10 | Passive Buzzer |

---

## LED Perimeter Mapping

The software maps 12 specific LEDs from the matrix into a clockwise perimeter layout. The `spinFrame()` function translates logical game positions into physical matrix coordinates:

```text
[12] [01] [02] [03]
[11] [  ] [  ] [04]
[10] [  ] [  ] [05]
[09] [08] [07] [06]

```

---

## Technical Implementation Details

### Driverless Multiplexing

Unlike standard implementations using a MAX7219 driver, this project drives the LED matrix directly from the Arduino GPIO pins. The software handles row-sinking and column-sourcing manually, using a high-frequency scanning routine to maintain the illusion of a steady display (Persistence of Vision).

### Input Handling & Shift Logic

To support 12 distinct tile selections using a standard 4x4 keypad, the system implements a "Shift" state:

* **Direct Entry:** Keys 1-9 register the corresponding tile.
* **Shift Entry:** Pressing '0' toggles a `shiftActive` boolean. Subsequent presses of 0, 1, or 2 result in selections 10, 11, or 12.
* **Navigation:** The 'N' key cycles the `currentPlayer` index, while 'S' triggers the game execution logic.

### Spin Simulation Engine

The spin phase utilizes a pseudo-random step generator seeded by user input timing. It implements a linear deceleration algorithm:

* **Initial Phase:** High-speed rotation with a 10ms delay between steps.
* **Decay Phase:** In the final 15 steps, the delay increases incrementally (+30ms per step) to simulate physical momentum loss.
* **Result Calculation:** The final position is resolved via `totalSteps % 13`.

---

## Software Prerequisites

* **Wire.h:** I2C communication protocol.
* **LiquidCrystal_I2C.h:** LCD management.
* **Keypad.h:** Matrix scanning and debounce management.

---

## Setup Instructions

1. **Clone the repository** and open the `.ino` file in the Arduino IDE.
2. **Install required libraries** via the Library Manager.
3. **Wire the components** according to the Hardware Configuration table using the appropriate jumper wires.
4. **Upload the sketch** to the Arduino Uno R3.

## License

This project is open-source and available under the **MIT License**.
