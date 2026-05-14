# Multi-Player LED Matrix Roulette

An System Integration and Architecture 2 project implementing a digital roulette game using an Arduino microcontroller. This project demonstrates direct GPIO manipulation to drive a peripheral matrix without external drivers, alongside a multi-player state-machine and custom audio feedback.

---

## System Architecture

The application is built on a state-machine architecture that manages player registration, selection validation, and a simulated physics engine for the "spin" phase.

## Hardware Configuration

### Bill of Materials

* **Microcontroller:** Arduino Uno R3
* **Display:** 16x2 LCD with I2C Interface Driver (Address: `0x27`)
* **LED Interface:** 8x8 LED Matrix (Driven directly via GPIO)
* **Input:** 4x4 Matrix Keypad
* **Audio:** Passive Buzzer (PWM-controlled)
* **Resistors:** 4x 220Ω (Required for Matrix Rows current limiting)
* **Interconnects:** Male-to-Male, Female-to-Male, and Female-to-Female jumper wires

### Pin Mapping

| Component | Arduino Pins | Details |
| --- | --- | --- |
| **LED Matrix Rows** | D2, D3, D4, D5 | Direct Multiplexed Output (Requires 220Ω resistors) |
| **LED Matrix Cols** | D6, D7, D8, D9 | Column Sourcing |
| **LCD (I2C)** | SDA (A4), SCL (A5) | Uses synchronous I2C protocol |
| **Passive Buzzer** | D10 | PWM-driven for custom frequencies |
| **Keypad (4x4)** | D11, D12, D13, A0, A1, A2, A3 | Matrix Scanning (Note: Pin 1 used for TX Serial) |

---

## Hardware Orientation & Pin Identification

Because 8x8 LED matrices (like the **1088BS**) often lack standardized pin labels, use the following orientation to ensure the code's mapping logic works correctly:

* **Orientation**: Position the matrix so the side with the serial number (e.g., **1088BS**) is on the **left**.
* **Sides**: The pins on the side with the serial number are the **Left Pins**; the opposite side are the **Right Pins**.
* **Pin Numbering**: On both sides, Pin 1 starts at the **bottom**. The pins count upward to Pin 8 at the **top**.

### Matrix-to-Arduino Wiring Reference

| Matrix Pin (Visual Identification) | Arduino Pin | Function |
| --- | --- | --- |
| **Left Pin 4** | **D2** | Row 1 (Resistor Required) |
| **Right Pin 7** | **D3** | Row 2 (Resistor Required) |
| **Left Pin 6** | **D4** | Row 3 (Resistor Required) |
| **Right Pin 6** | **D5** | Row 4 (Resistor Required) |
| **Left Pin 7** | **D6** | Column 1 |
| **Left Pin 1** | **D7** | Column 2 |
| **Right Pin 5** | **D8** | Column 3 |
| **Left Pin 8** | **D9** | Column 4 |

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
3. **Wire the components** according to the Hardware Configuration and Hardware Orientation tables.
* **CRITICAL:** Place a 220Ω resistor between each Row pin (D2-D5) and the Matrix pins to prevent overcurrent.


4. **Upload the sketch** to the Arduino Uno R3.

## License

This project is open-source and available under the **MIT License**.
