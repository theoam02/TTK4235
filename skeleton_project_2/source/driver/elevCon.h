#include "elevio.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

typedef enum elevCon_order_t
{
    no_order = 0,
    order = 1
}elevCon_order;

void elevCon_emergencyStop();
void elevCon_checkFloor(int cur_floor, MotorDirection dir);
bool elevCon_floors_in_direction(MotorDirection dir, int cur_floor);
void elevCon_updateFloorLight();
void elevCon_add_order();
void elevCon_set_dir(MotorDirection dir);
void elevCon_start();
void elevCon_serviceCurFloor();
MotorDirection elevCon_get_dir();
void elevCon_should_change_direction(int cur_floor);

void elevCon_print_orders();
void print_dir();