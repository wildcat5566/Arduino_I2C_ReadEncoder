// I2C slave transmitter on arduino UNO (atmega328)
// Wire library break down: direct access to registers.
static uint8_t txBuffer[32];
static volatile uint8_t txBufferIndex;
static volatile uint8_t txBufferLength;
static uint8_t txAddress;

static volatile uint8_t state;
static volatile uint8_t err;

void setup(){
  txAddress = 0x62;
  TWAR = txAddress << 1;                     // Set address as slave: left shift 1
  txBufferIndex = txBufferLength = 0;
  
  state = 0;                                 // TWI_READY: initialize state
  PORTC = 48;                                // (1) activate internal pullups for twi.
                                             //     i.e. digitalWrite(SDA, 1); (SCL, 1);
  _SFR_BYTE(TWSR) &= ~_BV(TWPS0);            // (2) initialize twi prescaler and bit rate
  _SFR_BYTE(TWSR) &= ~_BV(TWPS1);
  TWBR = ((F_CPU / 400000) - 16) / 2;        // (3) set SCL clock speed = 100 kHz
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);  // (4) enable twi module, twi interrupt and acks
} // </setup>

void loop(){
  //delay(10);
} // </loop>

void kuroneko() {
  byte buf[3];
  buf[2] = 9;
  buf[1] = 2;//random(0, 9);
  buf[0] = 4;//random(0, 9);
  transmit(buf, 3); // respond with message of 6 bytes as expected by master
} // </kuroneko>

uint8_t transmit(const uint8_t* data, uint8_t length){
  txBufferLength = length;                   // set length
  for(uint8_t i = 0; i < txBufferLength; ++i){
    txBuffer[i] = data[i];                   // populate data into tx buffer
  }
  return 0;
} // </transmit>

#define TW_STATUS_MASK    (_BV(TWS7)|_BV(TWS6)|_BV(TWS5)|_BV(TWS4)|_BV(TWS3))
ISR(TWI_vect){
  switch((TWSR & TW_STATUS_MASK)){ //TW_STATUS
    case 0x60:  // TW_SR_SLA_ACK: addressed, returned ack. Check <util/twi.h> for defs.
    case 0xA0: // TW_SR_STOP: stop or repeated start condition received
    case 0x80: // TW_SR_DATA_ACK: data received, returned ack
      
    case 0xA8:                            // (1) TW_ST_SLA_ACK(168): addressed, returned ack
    case 0xB0:                            // (2) TW_ST_ARB_LOST_SLA_ACK(176): arbitration lost, returned ack
      state = 4;                          //     TWI_STX: enter slave transmitter mode
      txBufferIndex = txBufferLength = 0;
      kuroneko();                         //     call twi_transmit to populate tx_buffer               
      if(0 == txBufferLength){
        txBufferLength = 1;
        txBuffer[0] = 0x00;
      }                                   //     transmit first byte from buffer, fall
    case 0xB8:                            // (3) TW_ST_DATA_ACK(184): byte transmitted, ack returned
      TWDR = txBuffer[txBufferIndex++];   //     copy data to output register
      if(txBufferIndex < txBufferLength){ //     ack if in progress
        TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);                            
      }else{                              //     else nack
        TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);                             
      }
      break;
    case 0xC0:                            // (4) TW_ST_DATA_NACK(192): last byte transmitted, NACK received
    case 0xC8:                            // (5) TW_ST_LAST_DATA(200): last byte transmitted, received ACK
      TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
      state = 0;                          //     TWI_READY: ready state
      break;
  }
} // </ISR>
