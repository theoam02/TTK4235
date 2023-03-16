#include "elevCon.h"
#include "elevio.h"
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

// Variables

/**
 * @brief array of orders from the elevator panel. First element is 0 if not called and 1 if called
 * 
 */

// Functions

MotorDirection motor_dir;

elevCon_order elevOrders[4][3] =
{
    {no_order, no_order, no_order}, // Up, down, cab
    {no_order, no_order, no_order},
    {no_order, no_order, no_order},
    {no_order, no_order, no_order}
};

void elevCon_emergencyStop()
{
    elevCon_set_dir(DIRN_STOP);

    // Remove all orders
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            int btnPressed = elevio_callButton(f, b);
            elevOrders[f][b] = no_order;
        }
    }

    if(elevio_floorSensor()!=-1) elevio_doorOpenLamp(1);

    while(elevio_stopButton()){} // Infinite loop doing nothing while the stop button is pressed
    
    // Wait 3 seconds before starting again
    sleep(3);

    elevio_doorOpenLamp(0);

    // Start elevator again
    elevCon_start();
}

/**
 * @brief If elevator isn't between floors, check if the current floor has an order of the same direction button or cab, in which case it will service the floor for all orders.
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
        if(dir == DIRN_STOP)
        {
            if(elevOrders[cur_floor][BUTTON_HALL_DOWN] == order || elevOrders[cur_floor][BUTTON_HALL_UP] == order) 
            {
                elevCon_serviceCurFloor(cur_floor, dir);
                return;
            }
        }
        if(dir == DIRN_DOWN)
        {
            if(elevOrders[cur_floor][BUTTON_HALL_DOWN] == order || elevOrders[cur_floor][BUTTON_CAB] == order) 
            {
                elevCon_serviceCurFloor(cur_floor, dir);
                return;
            }
            for(int floor = cur_floor; floor >= 0; floor--)
            {
                if(elevOrders[floor][BUTTON_HALL_DOWN] == order && !(cur_floor == 0 && floor == 0)) return;
                if(elevOrders[floor][BUTTON_CAB] == order) return;
                if(elevOrders[floor][BUTTON_HALL_UP] == order && !(cur_floor == 0 && floor == 0) && floor != cur_floor) return;
            }
            if(elevOrders[cur_floor][BUTTON_HALL_UP] == order){
                elevCon_serviceCurFloor(cur_floor, dir);
                return;
            }
        }
        if(dir == DIRN_UP)
        {
            if(elevOrders[cur_floor][BUTTON_HALL_UP] == order || elevOrders[cur_floor][BUTTON_CAB] == order) 
            {
                elevCon_serviceCurFloor(cur_floor, dir);
                return;
            }
            for(int floor = cur_floor; floor <= N_FLOORS-1; floor++)
            {
                if(elevOrders[floor][BUTTON_HALL_UP] == order && !(cur_floor == 0 && floor == 0)) return;
                if(elevOrders[floor][BUTTON_CAB] == order) return;
                if(elevOrders[floor][BUTTON_HALL_DOWN] == order && !(cur_floor == 0 && floor == 0) && floor != cur_floor) return;
            }
            if(elevOrders[cur_floor][BUTTON_HALL_DOWN] == order){
                elevCon_serviceCurFloor(cur_floor, dir);
                return;
            }
        }
        elevCon_should_change_direction(cur_floor, dir);
    }
}

/**
 * @brief Check if there are any floors in the specific direction
 * 
 * @param cur_floor 
 * @param dir 
 * @return true 
 * @return false 
 */
bool elevCon_floors_in_direction(int cur_floor, MotorDirection dir)
{
    if(cur_floor==-1)
    {
        return false;
    }

    // If the elevator is stopped, will check if any floors have orders
    if(dir==DIRN_STOP)
    {
        for(int floor = 0; floor <= N_FLOORS-1; floor++)
        {
            if(elevOrders[floor][BUTTON_HALL_DOWN] == order && floor != cur_floor) return true; // Don't check the floor it's already on for safety
            if(elevOrders[floor][BUTTON_HALL_UP] == order && floor != cur_floor) return true;
            if(elevOrders[floor][BUTTON_CAB] == order && floor != cur_floor) return true;
        }
    }

    // Check down orders
    if(dir==DIRN_DOWN)
    {
        for(int floor = cur_floor; floor >= 0; floor--)
        {
            if(elevOrders[floor][BUTTON_HALL_DOWN] == order && !(cur_floor == N_FLOORS-1 && floor == N_FLOORS-1)) return true;
            if(elevOrders[floor][BUTTON_HALL_UP] == order && !(cur_floor == 0 && floor == 0)) return true;
            if(elevOrders[floor][BUTTON_CAB] == order) return true;
        }
    }

    // Check up orders
    if(dir==DIRN_UP)
    {
        for(int floor = cur_floor; floor <= N_FLOORS-1; floor++)
        {
            if(elevOrders[floor][BUTTON_HALL_UP] == order && !(cur_floor == 0 && floor == 0)) return true;
            if(elevOrders[floor][BUTTON_HALL_DOWN] == order && !(cur_floor == N_FLOORS-1 && floor == N_FLOORS-1) ) return true;
            if(elevOrders[floor][BUTTON_CAB] == order) return true;
        }
    }

    return false;
}

/**
 * @brief Updates floor panel light as a part of polling.
 * 
 */
void elevCon_updateFloorLight()
{
    if(elevio_floorSensor()==-1)
    {
        return;
    }else
    {
        elevio_floorIndicator(elevio_floorSensor());
        printf("Updating light %d\n", elevio_floorSensor());
    }
}

void elevCon_add_order()
{
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            int btnPressed = elevio_callButton(f, b); // is 1 if button is pressed, is 0 if button isn't pressed
            if(btnPressed)
            {
                elevOrders[f][b] = order;
                printf("Added order floor %i button %i\n", f, b);
                elevio_buttonLamp(f, b, 1);
            }
        }
    }
}

void elevCon_set_dir(MotorDirection dir)
{
    motor_dir = dir;
    elevio_motorDirection(dir);
    printf("Set direction to %i\n", dir);
}

void elevCon_start()
{
    if(elevio_floorSensor()==-1)
    {
        elevCon_set_dir(DIRN_DOWN);
        while(elevio_floorSensor()==-1){
            /*
            for(int f = 0; f < N_FLOORS; f++){
                for(int b = 0; b < N_BUTTONS; b++){
                    int btnPressed = elevio_callButton(f, b);
                    elevOrders[f][b] = no_order;
                }
            }*/
        }
        elevCon_set_dir(DIRN_STOP);
    }
}

/**
 * @brief Stop elevator, open door for three seconds, then close door and continue.
 * 
 */
void elevCon_serviceCurFloor(int cur_floor, MotorDirection dir) // , int order)
{
    time_t begin;

    elevCon_set_dir(DIRN_STOP);

    elevio_doorOpenLamp(1);

    time(&begin);

    // Wait 3 seconds, but reset timer if there's an obstruction present
    while(time(NULL)-begin < 3) // Because counting includes 0 we add 1
    {
        if(elevio_obstruction()) time(&begin);
        printf("CUR TIME: %i\n", (int)(time(NULL)-begin));
        elevCon_add_order();
    }

    elevio_doorOpenLamp(0);

    // Remove all orders from current floor
    for(int b = 0; b < N_BUTTONS; b++){
        elevOrders[cur_floor][b] = no_order;
        elevio_buttonLamp(cur_floor, b, 0);
    }
    
    elevCon_should_change_direction(cur_floor, dir);
}

void elevCon_should_change_direction(int cur_floor, MotorDirection dir)
{
    if(cur_floor==-1) return;
    if(dir == DIRN_STOP)
    {
        if(elevCon_floors_in_direction(cur_floor, DIRN_UP))
        {
            elevCon_set_dir(DIRN_UP);
            return;
        }
        if(elevCon_floors_in_direction(cur_floor, DIRN_DOWN))
        {
            elevCon_set_dir(DIRN_DOWN);
            return;
        }
        return;
    }

    if(!elevCon_floors_in_direction(cur_floor, dir))
    {
        if(!elevCon_floors_in_direction(cur_floor, dir*-1))
        {
            elevCon_set_dir(DIRN_STOP);
            return;
        }else{
            elevCon_set_dir(dir*-1);
            return;
        }
    }else{
        elevCon_set_dir(dir); // Direction must be reset since it was set to stop earlier.
        return;
    }
}

MotorDirection elevCon_get_dir()
{
    return motor_dir;
}

void elevCon_print_orders()
{
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            printf("Floor %i button %i: order type: %i\n", f, b, elevOrders[f][b]);
        }
    }
}

void print_dir()
{
    printf("The motor direction is: %i\n", motor_dir);
}

// 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