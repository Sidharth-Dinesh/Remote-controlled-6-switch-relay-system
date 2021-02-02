# Documentation

## Purpose:
Documentation for 6 switch relay system, so the customer on the other end can assemble and operate the project.

## Table of contents:
1. [Hardware assembly](#hardwareassembly):
   1. [Hardware components](#11)
   2. [Connection diagram](#12)
2. [Installing software](#install):
3. [Testing the product](#testing):
4. [Reporting Errors and Troubleshooting](#reporting):

##  1. Hardware assembly:  <a name="hardwareassembly"></a>

**1. Hardware components required:** (Click on link to navigate to the website) <a name="11"></a>
   *  [Raspberry Pi 4 Model B](https://www.aliexpress.com/item/4001116237432.html?spm=a2g0o.productlist.0.0.5c676bdaqf4aVR&algo_pvid=560e4b4c-4005-4490-895c-19aba9b55a9e&algo_expid=560e4b4c-4005-4490-895c-19aba9b55a9e-0&btsid=0b0a555b16061420936697535e0790&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_): 
      * **Description:**

        1. 2GB/4GB/8GB RAM with 16GB/32GB SD card.        
        2.  Provides 1 ABS case, there are 2 colors, black and transparent, that you can choose. The ABS case can install a cooling fan, which will provide good heat dissipation for your Pi 4B. 
        3. 5V 3A Type C power adapter with on/off switch button, convenient to control the power. 
        4. 4pcs heat sink with cooling fan, provide good heat dissipation for your Pi 4B. 
        5. 1.5m Micro HDMI to HDMI, use to connect the Raspberry Pi to LCD. 
        6. SD card reader, device for reading SD memory card

	   * **Packing List:**
1 x Raspberry Pi 4 Model B (2GB RAM) 
1 x ABS Case 
1 x Switch Power Adapter (EU/US/UK/AU) 
1 x 4pcs Heat sink 
1 x 16GB SD Card 
1 x SD Card Reader 
1 x Cooling Fan 
1 x 1.5M Micro HDMI Cable
<img src="https://ae01.alicdn.com/kf/Hf805b9b141fe479b9b0ba0bad30bcbf3S.jpg" height=500px width=1000px>

   *  [6 Channel Relay Module](https://www.aliexpress.com/item/4000322826402.html?spm=a2g0o.productlist.0.0.4daa7599sv409F&algo_pvid=4bcedb0f-155b-424d-80da-9dc1ee905e1b&algo_expid=4bcedb0f-155b-424d-80da-9dc1ee905e1b-15&btsid=0b0a555616061421767325200eb6f7&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_): 
	   * 6 Channel Relay Module 5V High and Low Level Trigger Relay Control With Optocoupler 6 Way Relays DC 5 V 
	   * **Module Description:**  
			1. The module uses genuine quality relay, normally open interfaces. Maximum load: AC 250V/10A, DC 30V/10A.
			2. Using SMD optocoupler isolation, driving ability, stable performance; trigger current 5mA.
			3. The module Operating voltage 5V .
			4. The module can be high or low by a jumper setting trigger;  
			5. Fault-tolerant design, even if the control line is broken, the relay will not operate.  
			6. The power indicator (green), the relay status indicator (red).
			7. The interface design of human nature, all interfaces are available through a direct connection terminal leads, very convenient.
			8. Module size: 50mm\*26mm\*18.5mm (L\*W\*H).
  
		* **Module Interface:**  
			1. DC+: positive power supply (VCC)  
			2. DC-: negative power supply (GND)  
			3. IN: can be high or low level control relay
   <img src="https://ae01.alicdn.com/kf/He1f2e564aea6402786679303c9a3048cO.jpg" height=500px width=500px>

   *  [Jumper Wire](https://www.aliexpress.com/item/4000203371860.html?spm=a2g0o.productlist.0.0.37992b0bs4oIwz&algo_pvid=3932ef59-01da-4c72-bbc0-2ecb76f495a7&algo_expid=3932ef59-01da-4c72-bbc0-2ecb76f495a7-0&btsid=0b0a555716061429559278495e1982&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_):
	   * 40-120pcs Dupont Line 10CM 40Pin Male to Male + Male to Female and Female to Female Jumper Wire Dupont Cable for Arduino DIY KIT

		  <img src="https://ae01.alicdn.com/kf/Ha1bbd43f36f84d2f8015b1cc3e61eb38m.jpg" height=400px width=500px>

**2. Connection diagram:** (Sample Fritzing diagram) <a name="12"></a>  

 <img  src="https://lh3.googleusercontent.com/proxy/SRBzQ8i8sbrp5365KZX_-4Qd2ZpS-EJJmKHCljj62Y--SlmJV0egxcwp8ZtWjwWBdFV6Kd7IB4m9oHSvwMKD1I7HZTCF2JtAEZgub-xtwxNH5SniskxUrCC2kEPshc7NhlA" width=450px>

* **Connection pins:**
	* SWITCH1 = 3

	* SWITCH2  = 4

	* SWITCH3  = 5

	* SWITCH4  = 7

	* SWITCH5  = 8

	* SWITCH6  = 10
 
* **Connect Relay to Raspberry PI:**
	* Connect GND pin of the Relay with GND pin of the Raspberry PI
	* Connect VCC pin of the Relay with 5V pin of the Raspberry PI

* **Setup:**
The structure is very simple since all pins are labelled. Left (GND) comes to pin 6 of the Pi (GND), the right pin (VCC) comes to 3V3 (pin 1) of the Pis. Depending on how many of the relays you want to control, you need to connect a corresponding number of GPIOs to the IN pins. It is recommended to set a small resistor between the Pi and the relay, but it is not absolutely necessary with 3V3.  
If you set 5V instead of 3.3V to VCC, you should definitely put one resistor each (~ 1kΩ) between the GPIOs and the IN pins.


##  2. Installing software:  <a name="install"></a>

#### Requirements:

1.  Shunya OS supported Development board (RaspberryPi 4)
2.  Power supply for the board (5V 3A power supply for RaspberryPi 4)
3.  microSD card
4.  microSD card Reader
5.  Laptop/PC

#### Steps are :

1.  First we load Shunya OS on your board.
2.  Boot up Shunya OS

#### Install Etcher:Etcher allows you to Flash Shunya OS on the micro-SD card.
 For Ubuntu 16.04 installing Etcher is Simple, download the executable file and Run.

1.  Download  [balenaEtcher](https://www.balena.io/etcher/)  for Linux.
2.  Extract the zip file.
3.  Run AppImage  `sudo ./balenaEtcher-xxx.AppImage`

#### Flash Shunya OS

1.  Download  [Shunya OS](http://releases.shunyaos.org/1.0-beta/)
    
2.  Extract Downloaded zip file, by Right clicking on the downloaded zip file and selecting "Extract Here".
    
3.  `.img`  file in the extracted folder contains Shunya OS.
    
4.  Insert SD card with Card reader.
    
5.  Open Etcher program.
    
6.  Click Select Image.
    
7.  Select Shunya OS file  `shunya-{type}-image-{board-name}.img`  file.
    
    _For example_:  `shunya-console-image-raspberry4.img`
    
    ##### NOTE
    
    -   Shunya OS released in 3 variants
        
        -   tiny - Extremely Small footprint image use only for final deployment.
        -   minimal - Small footprint image use only for final deployment testing.
        -   console - Full console only image use only for development.
    -   At this stage it is recommended that you use  **console**  variant
        
    
8.  Your SDCard should be auto detected in Etcher.
9.  Click on Flash.
10.  Now Etcher will load the Shunya OS on your SD card.
11.  Now you can boot up the Shunya OS on your board.
    

#### Booting Up with Shunya OS
1.  Insert the micro-SD card into the board.
2.  Connect peripherals like Keyboard, Mouse and HDMI monitor.
3.  Connect Power Supply.

The board should boot up with Shunya OS.

#### Login to Shunya

Login with these credentials:

-   Username : shunya
-   Password : shunya

1.  This will bring you to Terminal inside the board.
2.  Now you can download, build and run the code using inside the board.


##  3. Testing the product : <a name="testing"></a>

##  4. Reporting Errors and Troubleshooting: <a name="reporting"></a>
