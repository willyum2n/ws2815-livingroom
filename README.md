# ws2815-livingroom

This project is for lighting a living room with indirect lighting utilizing the 12 volt WS2815 series of LED strips.

The LED strips will hidden in crown molding and aimed at the white cieling. This will blend and scatter all the light off this surface and make for nice even lighting with very little "hot spots".

The crown molding will be afixed to the wall using custom designed 3D printed brackets. These brackets will allow the molding to be fastened to the wall without any nails or screws thru the facia of the board. As such, this will make a perfect face of the molding. The molding will also be reinforced with a wooden rod that will also act as the LED standoff. 3D printed jigs will be made for getting these rods installed/glued/screwed from the backside so nothing penetrates the facia.

A microcontroller will be picked that can be programmed from the Arduino IDE so that there is no material expense/tooling required in getting the microcontroller firmware installed (Like what would be necessary if we went with Microchip). The selection of the MCU is important because some will have 5V logic (Arduino Uno), but most others will have 3.3V logic (adafruit feather, nodemcu, etc). If a MCU with 3.3V logic is picked, then a levelshifter will need to be employed to bitbang out LED data because the WS series expects a 5V signal.

The firmware will support several modes of operation:
 - **Off**: All LEDs will not emit light and the power supplies will be disconnected (disconnecting the PS is necessary becase the WS2815 pulls lots of current even when no LEDs are shining)
 - **Full On**: All LEDs in the entire strip are turned on White
 - **CraftingOnly**: Only a section of the LED strip is turned on (This lights the "Crafting" area of the living room
 - **Party**: This will be some sort of fun animation
 - **Dancing Water**: Special animation for a watery effect on the room

The system will be controlled with a momentary press button and a Rotary Encoder (RE) (Which also contains a momentary switch). The switch on the RE will toggle the mode from Off to FullOn and back again. This acts as an On/Off switch. The knob on the RE will act as a brightness control (For modes that consider brightness). The momentary button will act as the MODE button. This button will cycle thru all modes accept the Off mode (Off will be skipped when the modes wrap around). These two components will be installed into a Single-gang faceplate to make a control panel device for a human. We are going to put 2 such devices in parallel (The switch parts. The RE will each need thier own I/O pins) so that we can have 2 control panels on opposite sides of the room.

In order to power all of the LEDs all the way around the room, (6) 12VDC power supplies will be placed around the room, each supplying power to 1/6 of the chain. These power supplies will have a p-channel MOSFET that switches the power to the LED on/off because the WS2815 pulls a significant enough current when the system is doing nothing. This way, when the system is off, the 5050 chips cannot suck on the supplies. All the p-channel MOSFETs will be controlled by a single n-channel MOSFET which is tied to a GPIO pin on the microcontroller. This way, the MCU can control all the power states of all the power supplies.

## Crown Molding for indirect lighting
All the LED Strip lighting will be mounted behind crown moulding and pointed at the ceiling. This will give a nice indirect light that should be scattered nicely by the white ceiling. The crown moulding will not be installed all the way in the corner where the wall and the ceiling meet, but rather, it will be installed so there is a very large gap. Crown moulding needs both the top and the bottom edge nailed and caulked to be secured in place. Because we are not adhearing the top edge, we will need to build a custom bracket that will afix the moulding to the wall securely.

We will also need something for the LED's to be glued to that point them toward the ceiling. In order to get the LEDs in the correct orientation, a long stick of wood will be glued/screwed to the back of the moulding. Then the LED's will be glued to this as well. This stick will also act to make the moulding more rigid and less droopy. Jigs will be 3D printed to position the wooden stick in the correct place while the glue dries.

The bracket that was designed to hold the moulding to the wall has a bit of complication to it. The bracket was designed to screw into the moulding from the back side so that there are no imperfections on the front of the moulding. There will be new screw holds or nails holes to fill on the front side of the moulding (NICE!). The bracket comes in two parts. One side is afixed to the moulding, and the other side is afixed to the wall. They will then mate and a metal pin will be inserted to join them together. This allows 16ft of moulding to be installed at a time, and there is an added bonus that the entire thing can come down later (So LED strips can be changed/maintenanced)

## Wall Plates the human controls
A custom wall plate will be 3D printed that will hold all the buttons and descrete electronics for the buttons. This wall plate will have a CAT5 connector that will physically connect to the plate and be glued in place. 7 of 8 of the conductors in this connector will be used. Cat5 cable will connect it back to the main MCU in a different part of the room. This will allow for the wall plate to be added/removed easily for installation and maintenance purposes. Here is the wiring diagram at the plate:

| Signal     | SSCON Side  | CAT5 Cable 568B |
|:-----------|:------------|:---------------:|
| RE_GND     | Pin1: Black  | O/W |
| RE_+       | Pin2: Blue   | O |
| RE_SW      | Pin3: White  | Gr/W |
| RE_DT      | Pin4: Yellow | B |
| RE_CLK     | Pin5: Green  | B/W |
|            | Pin6: Red    | Gr |
| SW_A (GND) | Pin7: Orange | Br/W |
| SW_B (SIG) | Pin8: Brown  | Br |


Because this is a livingroom with several points of ingress/egress, there will be two different Wall Plates for human control on opposite sides of the living room. The +, GND, and Switches will be wired in parallel *at the controller* (This effectively gives them OR logic). The RE_DT and RE_CLK will need seperate I/O pins at the controller to watch each Rotary Encoder seperately.


## 3D Printed Parts:
//TODO: 

### References
 - LED Strip: http://www.witop-tech.com/wp-content/uploads/2017/06/WS2815-12v-addressable-led-chip-specification-.pdf
 - p-channel MOSFET for switching main power to LEDs: https://www.diodes.com/assets/Datasheets/74LVC1T45.pdf
 - n-channel MOSFET to interface GPIO to p-channel gate: https://www.diodes.com/assets/Datasheets/products_inactive_data/DMN2041L.pdf
 - Single bit Levelshifter: https://www.diodes.com/assets/Datasheets/74LVC1T45.pdf
 - Crown Moulding: ALB 49 CROWN 3-5/8" FMD I FINISHED MOULDING (TSCA TITLE VI COMLIANT)
 - Sscon 5pcs RJ45 8P8C CAT5 Network Connector Cable: https://www.amazon.com/gp/product/B07S34KPG2/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1
 - Rotary Encoder (With Momenary Switch): 
 - Momentary Switch: 
