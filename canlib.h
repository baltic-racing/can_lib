#ifndef CANLIB_H_
#define CANLIB_H_

#include <stdbool.h>

typedef struct CAN_MOB_ID *;

void can_init();
void can_rx(CAN_MOB_ID mod_id);
void can_rx_multi(CAN_MOB_ID mod_ids[]);
void can_get_msg(CAN_MOB_ID mob_id);
_Bool _can_check_free(uint8_t, mobnum);

#endif  // CANLIB_H_