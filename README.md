STM32 FreeRTOS Sensor Hub
A multi-tasking sensor monitoring system built on FreeRTOS for the STM32F103C8T6 (Blue Pill). This project integrates an MPU6050 (Accelerometer/Gyro), an RTC (Real-Time Clock), and an I2C LCD, coordinated by a Supervisor task.

🚀 Project Overview
The system utilizes four concurrent tasks to handle asynchronous data collection and display. It is designed to be binary-compatible with real hardware while supporting simulation in QEMU.

System Architecture
MPU Task: Polls IMU data via I2C (Stack: 128 words).

RTC Task: Tracks system time (Stack: 128 words).

LCD Task: Manages the visual interface (Stack: 128 words).

Supervisor Task: Coordinates data flow and handles string formatting (Stack: 256 words).

🛠 Memory Configuration
We have optimized the memory layout to fit within the conservative 8KB RAM profile while remaining expandable to the full 20KB of the Blue Pill.


💻 Simulation vs. Hardware
__Running in QEMU
To test logic, task switching, and memory usage:
1. I have used stm32vldiscovery machine type as qemu doesn't support stm32f103ct8 .(stm32vldiscovery is identical to it but with low ram)
2. Note: I2C hardware polling is bypassed in this mode to prevent simulation hangs.
   
__Deploying to Blue Pill
1.Set configCPU_CLOCK_HZ to 72000000.
2.Flash using an ST-Link V2.

📂 Project Structure
🔧 Current Progress
[x] Initialized FreeRTOS with 6KB Heap.
[x] Implemented Task-to-Task communication via Queues.
[x] Optimized stack sizes to prevent Stack Overflow within 8KB RAM.
[x] Created hardware-bypass macros for QEMU simulation.
[x] Debugged Heap allocation vs. Overhead calculations.
