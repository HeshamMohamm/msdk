## Description
This Example shows how to wake up a device with a wake up timer. Pressing PB0 will enable the wakeup timer and place the device in  a low-power mode. After a defined number of seconds (5s by default) the wakeup timer will wake the device.

On the MAX32655EVKIT:
-    PB0: P0.18/SW3

On the MAX32655FTHR:
-    PB0: P0.2/SW2

## Software

### Project Usage

Universal instructions on building, flashing, and debugging this project can be found in the **[MSDK User Guide](https://analog-devices-msdk.github.io/msdk/USERGUIDE/)**.

### Project-Specific Build Notes

* This project comes pre-configured for the MAX32655EVKIT.  See [Board Support Packages](https://analog-devices-msdk.github.io/msdk/USERGUIDE/#board-support-packages) in the MSDK User Guide for instructions on changing the target board.

## Required Connections
If using the MAX32655EVKIT (EvKit\_V1):
-   Connect a USB cable between the PC and the CN1 (USB/PWR) connector.
-   Connect pins JP4(RX_SEL) and JP5(TX_SEL) to RX0 and TX0  header.
-   Open an terminal application on the PC and connect to the EV kit's console UART at 115200, 8-N-1.

If using the MAX32655FTHR (FTHR\_Apps\_P1):
-   Connect a USB cable between the PC and the J4 (USB/PWR) connector.
-   Open an terminal application on the PC and connect to the board's console UART at 115200, 8-N-1.

## Expected Output

```
/************** Wakeup timer example ********************/
This example is to show how the Wakeup timer is used and configured
Press PB0 to put the chip into sleep and then the wakeup timer will wake up in 5000 Miliseconds
Entering SLEEP mode.
Waking up from SLEEP mode.
```
