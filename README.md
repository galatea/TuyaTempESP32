Dear Readers,

In the following description, I would like to show you how to cost-effectively access the data from your Tuya temperature and humidity sensors at home—without the need for a Gateway or other magic boxes.

Here is the temperature sensor: <br>
<img width="270" height="273" alt="image" src="https://github.com/user-attachments/assets/37452922-8a3f-4863-b9ea-9b5a01f98708" />

The first step is to flash the firmware on the sensors with a PVVX (https://pvvx.github.io/TH05F/) bootloader and then switch them to Application mode.

1. To flash them, you first need to disassemble the thermometers:  
   ![image](https://github.com/user-attachments/assets/5bfeff59-edfa-43fa-bc22-a741ddc47a37)

2. You need to get a UART to USB converter.

3. Connect the solder pads to the pins using wires and solder, according to the table below:

   | Adapter | Device |
   |---------|--------|
   | GND     | -Vbat  |
   | +3.3V   | +Vbat  |
   | TX      | RX1    |
   | RX      | TX1    |
   | RTS     | RESET  |

4. Download this Python file:  
   [rdwr_phy62x2.py](https://github.com/MakersFunDuck/humidity-temperature-sensor-TH05F/blob/main/rdwr_phy62x2.py)  
   and run the correct command:  
   ```bash
   python3 rdwr_phy62x2.py -p COM11 -e -r wh BOOT_XXX_vXX.hex
   ```

5. Reassemble the sensor and try to connect to it via Bluetooth using the following website:  
   [https://pvvx.github.io/THB2/web/PHY62x2BTHome.html](https://pvvx.github.io/THB2/web/PHY62x2BTHome.html)

6. Connect - > OTA - > Change to XXX_XXX_XX Application Firmwave.

7. Upload the code to the ESP32

8. Check the Console:
   <br><img width="674" height="474" alt="image" src="https://github.com/user-attachments/assets/cb86c8c9-a036-4090-99b6-f569729f93a4" />

