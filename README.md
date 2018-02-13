# Arduino_I2C_ReadEncoder
I2C implementation: Arduino Due serving as master receiver and Arduino Uno as slave transmitter (encoder reader).

## I2C protocol structure
![alt text](./src/protocol.jpg) <br />
- Start condition: SCL pulls low after SDA.<br />
- Address frame: 7 address bits + R/W operation bit + ack bit <br />
- Data frame: 8 bits + ack bit <br />
- Stop condition: SDA pulls high after SCL. <br />

## Demo
Example provided in code:  <br />
- Encoder reading value 429 is broken down into a 3-byte array format = {4,2,9}. <br />
- Slave address: 0x72 (98 in decimal). <br />

## Logic analyzer
### Address frame
![alt text](./src/address.jpg) <br />
 <br />
 
### data frame #0: decodes to 8-bit unsigned integer "4". 
![alt text](./src/data0.jpg) <br />
 <br />
 
### data frame #1: decodes to 8-bit unsigned integer "2".
![alt text](./src/data1.jpg) <br />
 <br />
 
### data frame #2: decodes to 8-bit unsigned integer "9".
![alt text](./src/data2.jpg) <br />
 <br />
 
### Reconstructing the three digits gives the original encoder reading = 429.
![alt text](./src/serial.JPG) <br />

### Other data issues: real situation
Real encoder reading value range = 32-bit signed integer = -2^31 ~ (2^31 - 1) <br />
Reconstruct integer from received bit array (but in binary representation rather than decimal). <br />
4 data frames are needed. <br />
![alt text](./src/dataframes.jpg)

## REFERENCES
https://learn.sparkfun.com/tutorials/i2c <br />
