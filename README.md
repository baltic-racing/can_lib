# CAN_LIB
A simple Libary to send and receive CAN Messages with standard A (11bit long identifier) for the at90can

## Some explanations

the CAN_MOB Structure contains following data which need to be configured
```	
uint16_t mob_id      -> the identifier of the MOB for the bus is stored(11 Bit long identifier for Standard A)
uint16_t mob_idmask  -> the identifier for the id to receive more MOBS with the specified id
                      for example:
                                id = 0xF;
                                idmask = 0xC;
                                so we would receive the following MOBs with the ids 0b1100 to 0b1111
uint8_t mob_number;  -> The internal mob number for management in the MCU (needs to be between 0-15)
		     -> Only one mob should use one specific mob_number on the controller
```
## Usage example

To use this libary we first need to run can_cfg(); to clear all garbage data in the MOBs
and configure the CAN-Bus speed to 1MBaud (the standard can bus frequency we are using)

For Transmit we are passing an 8byte array and the CAN_MOB object containing the configuration to the can_tx() function

For Receiving MOBS we pass an array and the CAN_MOB object to the can_rx() function and it'll write 8 bytes to the specified arry

## Small example code
the following code would receive a mob with the id 0x101 and only that mob
and would transmit a mob with id 0x100

```
uint8_t mob_0_data[8];
uint8_t mob_1_data[8];
struct CAN_MOB mob_to_transmit;
struct CAN_MOB mob_to_receive;

int main(void){
	can_cfg();
	mob_to_transmit.mob_id = 0x100;
	mob_to_transmit.mob_idmask = 0;
	mob_to_transmit.mob_number = 0;
  
  mob_to_receive.mob_id = 0x101;
	mob_to_receive.mob_idmask = 0xfff;
	mob_to_receive.mob_number = 1;
	
   while (1){
	can_tx(&mob_to_transmit, mob_0_data);
     	can_rx(&mob_to_receive, mob_1_data);
	}
}
```
