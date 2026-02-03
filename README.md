# NightWatch

# 1. About the project

**NightWatch** is a lighting control system using RGB strips that automatically adjusts lighting depending on the time of day. This allows you to smoothly switch between work mode and relaxation mode, helping you prepare for sleep.

The project is written entirely in Bare Metal style, without using ready-made Arduino or HAL libraries. All drivers and controls (I2C, PWM, Interrupts) are implemented through direct control of the microcontroller registers.

# 2. Functionality

### 2.1 Automatic mode switching

- **Day:** Bright purple light for concentration and work mode.
- **Sunset:** Transition to a warm orange spectrum.
- **Night:** Transition to a dim red color to maintain melatonin production and relaxation before bedtime.

### 2.2 I2C Driver

Driver implementation for communication with RTC DS3231.

### 2.3 Hardware PWM

Smooth color control via timers.

### 2.4 Interactive Control

Brightness adjustment via buttons using external interrupts.

# 3. Tech Stack

## 3.1 Hardware

- **MCU:** ATmega328P (Arduino Uno).
- **RTC:** DS3231
- N-Channel MOSFETs **IRLZ44N**
## 3.2 Software

- **Language:** C (AVR-GCC).
- **Concepts:** Direct Register Access, Bitwise Operations, ISR (Interrupt Service Routines), I2C Protocol state machine.

# 4. Scheme

![Scheme](scheme/scheme_v1.png)

### Description

The circuit is based on the **Low-Side Switching** principle using N-Channel MOSFETs

- **Power section:** Three N-channel MOSFETs control the Red, Green, and Blue channels of the LED strip (12V). A common anode (+12V Common Anode) circuit is used.
  
- **Gate Resistors (R4, R5, R6 - 100Ω):** Protect the microcontroller pins from current surges.

- **Pull-down Resistors (R1, R2, R3 — 1kΩ):** Pull the gates to ground.

- **Time module:** DS3231 is connected via the I2C bus (`SDA`, `SCL`) and has its own pull-up resistors.

##### Brightness control:

Two push buttons connected to external interrupt pins are used to adjust the brightness:

- **BTN_DOWN:** Pin `PD2` (INT0) - closed to GND.
- **BTN_UP:** Pin `PD3` (INT1) - shorted to GND.
  
  _Internal pull-up resistors are enabled in the code._

# 5. Future Improvements

- [ ] Non-blocking Architecture: Replace _delay_ms in ISR with timer-based debouncing.

- [ ] EEPROM Storage: Save brightness level so it persists after power loss.

- [ ] UART CLI: Add a command-line interface to set time without recompiling code.

# 6. Gallery

![Change](gallery/led.gif)