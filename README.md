# SysLat_Firmware

Arduino/Custom PCB firmware for SysLat project.
Repo for SysLat Software for Windows can be found here: https://github.com/Skewjo/SysLat_Software

# Build your own

Clone this repo and follow the instructions on https://syslat.com/posts/build_your_own (coming soon)

# Updating SysLat Firmware

//I got through writing these instructions and realized they are completely wrong. You were wanting to write some kind of script using avrdude(?), right? I need to look into that more.
While there are multiple ways to "flash"(am I using this word right? probably not) an Adafruit microcontroller, this set of instructions will require the Arduino IDE.

1. Download the file [syslat_fw_pcb_rev_1.ino.with_bootloader.itsybitsy32u4.hex](https://github.com/Skewjo/SysLat_Firmware/blob/master/syslat_fw_pcb_rev_1/syslat_fw_pcb_rev_1.ino.with_bootloader.itsybitsy32u4.hex)
2. [Download and install the Arduino IDE](https://www.arduino.cc/en/software)
3. Open the Arduino IDE
  * Go to **File -> Preferences -> Additional Boards Manager URLs** and enter the following URL, then press OK.
     * https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
  * From the **Tools** menu:
     * Go to **Board -> Boards Manager** then install **Adafruit AVR Boards**, and close the window.
     * Then in **Board -> Adafruit Boards** choose **AdaFruit ItsyBitsy 32u4 5V 16MHz**.
     * Under the **Port** menu choose the correct COM port that represents your device as shown in [this screenshot](https://imgur.com/a/lwYAbTU).
        * Very common errors occur here, but I can't remember the exact problem so I can't find the documentation for it. If you run into an error at this step please reach out to us at [support@syslat.com](support@syslat.com).
  * Upload the file to the board.
