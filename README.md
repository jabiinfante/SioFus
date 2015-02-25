# SioFus
Simple Input Output From USB to Serial adapter is a short code designed to read and write digital signals to/from the real world.
Based on the highlighted PINs of the following list, we will be able to setup **4 digital inputs** and **2 digital outputs** just using a cheap USB2Serial adapter


| USB PIN # | Name |
| ------------- | ------------- |
| 1  | **USB VCC**  |
| 2 | ~~Data -~~ |
| 3 | ~~Data +~~ |
| 4 | ~~Ground -~~ |

| Serial (DB9) PIN # | Name |
| ------------- | ------------- |
| 1 | ** DCD (Carrier Detect)** | 
| 2 | ~~RxD (Receive Data)~~ |
| 3 | ~~TxD (Transmit Data)~~ | 
| 4 | **DTR (Data Terminal Ready)** |
| 5 | ~~SG (Signal Ground)~~ | 
| 6 | **DSR (Data Set Ready)** | 
| 7 | **RTS (Request To Send)** |
| 8 | **CTS (Clear To Send)** | 
| 9 | **RI (Ring Indicator)** | 

### Launch the process
```bash
./siofus -p /dev/ttyUSB0
```


### Usage as OUTPUT
We will be able to control DTR and RTS PINS on our adapter. The process will be listening (NONBLOCK) for input on the stdin:
 - if key "a" is pressed send +5V to PIN4 (DTR)
 - if key "s" is pressed send -5V to PIN4 (DTR)
 - if key "d" is pressed send +5V to PIN7 (RTS)
 - if key "f" is pressed send -5V to PIN7 (RTS)

_You would get a current around 0.05A_

### Usage as INPUT
The idea is being able to tell weather a circuit is open or closed. For this purpouse, we will be using **USB PIN 1** as the VCC source.
The process will be able to tell when the circuit is closed or opened. These are then PINS used in the Serial conector:
 - PIN1 (DCD) : INPUT**0**
 - PIN6 (DSR) : INPUT**1**
 - PIN8 (CTS) : INPUT**2**
 - PIN9 (RNG) : INPUT**3**

The process will be able to tell to the stdout when a PIN has been closed or opened. For example, when someone connect USB PIN1 to Serail PIN6 (DSR), SioFus will simple write to stdout "#C1#"; when  the connection is released, it will write "#O1#".


### Compile

```bash
gcc -o siofus siofus.c
```

### Version
0.0.1