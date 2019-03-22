#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>

#include "canlib.h"

//DEFINES
#define CAN_RX_MOBS 16

#define CAN_ID_SWC 0x100;  // SWC Feedback
#define CAN_ID_CMC 0x200;  // CMC Feedback Gear

#define CAN_ID_DATALOGGER_V 0x300; // DL VS_FR_L VS_FR_R VS_RE_R VS_RE_L
#define CAN_ID_DATALOGGER_BREAKPRESSURE 0x301; // DL BP_F BP_R BT_FR_L BT_FR_R

#define CAN_ID_ECU0 0x601; // ECU rpm tps map iat injpw
#define CAN_ID_ECU2 0x602; // ECU vspd baro oilt oilp fuelp clt,
#define CAN_IDhttp://www.asciitable.com/_ECU4 0x604; // ECU gear batt ecutemp errflag

struct CAN_MOB_ID {
    /*
     * CAN mob and register id.
     */
    uint8_t mob;
    uint16_t id;
};


// Global CAN memory.
uint8_t CAN_DATA_BYTES[rx_mobs][8];

void can_init(){
    /*
     * Initialize the CAN bus for our needs.
     *
     * Actions:
     *  - Disable CAN bus
     *  - Reset all mobs
     *  - Set the baud rate to 1M
     *  - Disables interrupts
     *  - Enable CAN again.
     * */

    CANGCON = 0; // Disable CAN

    // Reset all mobs
    for (uint8_t mob = 0; mob < CAN_RX_MOBS ; mob++){
        CANPAGE = mob<<MOBNB0 | (1<<AINC);
        CANSTMOB = 0;
        CANCDMOB = 0;
        CANIDT4 = 0;
        CANIDT3 = 0;
        CANIDT2 = 0;
        CANIDT1 = 0;
        CANIDM4 = 0;
        CANIDM3 = 0;
        CANIDM2 = 0;
        CANIDM1 = 0;
        for (uint8_t byte = 0; byte < 8; byte++){
            CANPAGE = mob<<MOBNB0 | 1<<AINC | byte;
            CANMSG = 0;
        }
    }

    // Set baud rate to 1000kBaud (data sheet p. 267)
    CANBT1 = 0x00;
    CANBT2 = 0x0C;
    CANBT3 = 0x36;

    // Disable interrupts
    CANGIE = 0;

    // Enable CAN
    CANGCON |= (1<<ENASTB);
}

void can_rx(CAN_MOB_ID mod_id) {
    /*
     * Receive a CAN packet and save to CANPAGE.
     */
    CANPAGE = mod_id.mob << MOBNB0;
    if (_can_check_free(mod_id.mob)){
        // load the id 11 bit
        CANIDT1 = mod_id.id >>3;
        CANIDT2 = (mod_id.id << 5)&0b11100000;
        CANIDT3 = 0;
        CANIDT4 = 1<<RTRTAG;
        CANIDM1 = 0b11111111; // Enable CAN
        CANIDM2 = 0b11100000;
        CANIDM3 = 0;
        CANIDM4 = 0;
        CANCDMOB = (1 << CONMOB1) | (1 << CONMOB0)| (1<<DLC3);
        CANSTMOB = 0;
    }

}



void can_rx_multi(CAN_MOB_ID mob_ids[]) {
    /*
     * Receives and gets multiple CAN packets.
     *
     * Replaces calling `can_rx(...); can_get_msg(...)` a lot of times.
     * This is the replacement for `can_data_management()` per controller.
     * */
    for (uint8_t i; i < (sizeof(mod_ids) / sizeof(CAN_MOB_ID)); i++) {
        can_rx(mob_ids[i])
        can_get_msg()
    }
}

/*
void can_rx(uint8_t mobnum, uint16_t id){

    CANPAGE = mobnum << MOBNB0;
    if (can_check_free(mobnum)){
        // load the id 11 bit
        CANIDT1 = id >>3;
        CANIDT2 = (id << 5)&0b11100000;
        CANIDT3 = 0;
        CANIDT4 = 1<<RTRTAG;
        CANIDM1 = 0b11111111; // Enable CAN
        CANIDM2 = 0b11100000;
        CANIDM3 = 0;
        CANIDM4 = 0;
        CANCDMOB = (1 << CONMOB1) | (1 << CONMOB0)| (1<<DLC3);
        CANSTMOB = 0;
    }

}*/

void can_get_msg(CAN_MOB_ID mob_id){
    /*
     * Transfers a single CAN mob to memory.
     */
    for(uint8_t byte = 0; byte <8; byte++){
        CANPAGE = (mob_id.mob << MOBNB0) | (1 << AINC) | byte;
        can_data_bytes[mob_id.mob][byte] = CANMSG;
    }

}

_Bool _can_check_free(uint8_t mobnum){
    /*
     * Checks if a CAN mob is free to receive data.
     */

    _Bool status = 0;
    if(mobnum >7){
        status = !((CANEN1 >> (mobnum-8)) &1);
    } else {
        status = !((CANEN2 >> mobnum) &1);
    }
    return mob_status;
}

/*
void can_data_management(void) {
    can_rx(0, 0x200);
    can_get_msg(0);
    can_rx(1,0x100); //SWC Feedback
    can_get_msg(1);
    can_rx(2,0x600); //ECU rpm tps map iat injpw
    can_get_msg(2);
    can_rx(3, 0x602); //ECU vspd baro oilt oilp fuelp clt,
    can_get_msg(3);
    can_rx(4,0x604); //ECU gear batt ecutemp errflag
    can_get_msg(4);
    can_rx(5,0x300); //DL VS_FR_L VS_FR_R VS_RE_R VS_RE_L
    can_get_msg(5);
    can_rx(6,0x301); //DL BP_F BP_R BT_FR_L BT_FR_R
    can_get_msg(6);
}
*/