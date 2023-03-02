#include "elevio.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

typedef enum elevCon_order_t
{
    no_order = 0,
    up = 1,
    down = 2,
    cab = 3
}elevCon_order;

void elevCon_emergencyStop();
void elevCon_checkFloor(int cur_floor, MotorDirection dir);
bool elevCon_floors_in_direction(MotorDirection dir, int cur_floor);
void elevCon_serviceCurFloor();
void elevCon_updateFloorLight();