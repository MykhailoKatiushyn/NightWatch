# NightWatch

# 1. About the project

**NightWatch** is a standalone lighting control system using RGB strips that automatically adjusts lighting depending on the time of day. This allows you to smoothly switch between work mode and relaxation mode, helping you prepare for sleep.

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

Brightness adjustment via buttons using external interrupts with debounce protection.

# 3. Tech Stack

## 3.1 Hardware

- **MCU:** ATmega328P (Arduino Uno).
- **RTC:** DS3231
- N-Channel MOSFETs **IRLZ44N**
## 3.2 Software

- **Language:** C (AVR-GCC).
- **Concepts:** Direct Register Access, Bitwise Operations, ISR (Interrupt Service Routines), I2C Protocol state machine.

# 4. Scheme

etc
