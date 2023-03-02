#include "elevCon.h"
#include "elevio.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

// Variables

/**
 * @brief array of orders from the elevator panel. First element is 0 if not called and 1 if called
 * 
 */

elevCon_order elevOrders[4] =
{
    no_order,
    no_order,
    no_order,
    no_order
};

void elevCon_emergencyStop()
{
    // Remove all orders
    for(int floor = 0; floor < 3; floor++)
    {
        elevOrders[floor] = no_order;
    }

    if(elevio_floorSensor()!=-1) elevio_doorOpenLamp(1);

    while(elevio_stopButton()){} // Infinite loop doing nothing while the stop button is pressed
    sleep(3);
}

/**
 * @brief If elevator isn't between floors, check if it has an order in same direction or cab, in which case it will service the floor.
 * 
 * @param cur_floor 
 * @param dir 
 */
void elevCon_checkFloor(int cur_floor, MotorDirection dir)
{
    if(cur_floor==-1)
    {
        return;
    }else
    {
        if(elevOrders[cur_floor]==up && dir==DIRN_UP || elevOrders[cur_floor]==down && dir==DIRN_DOWN || elevOrders[cur_floor]==cab)
        {
            elevCon_serviceCurFloor();
        }
    }
}

/*
if(!floors_in_direction(dir))
{
    elevio_motorDirection(dir*-1)
}
*/

bool elevCon_floors_in_direction(MotorDirection dir, int cur_floor)
{
    for(int floor = cur_floor; floor < N_FLOORS-1 && floor > 0; floor += dir)
    {
        if(elevOrders[floor]==up && dir==DIRN_UP || elevOrders[floor]==down && dir==DIRN_DOWN || elevOrders[floor]==cab)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Stop elevator, open door for three seconds, then close door and continue.
 * 
 */
void elevCon_serviceCurFloor()
{
    elevio_motorDirection(DIRN_STOP);
    elevio_doorOpenLamp(1);
    sleep(3);
    elevio_doorOpenLamp(0);
}

/**
 * @brief Updates floor panel light. 
 * 
 */
void elevCon_updateFloorLight()
{
    if(elevio_floorSensor()!=-1)
    {
        
        elevio_floorIndicator(elevio_floorSensor());
    }
}

// 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