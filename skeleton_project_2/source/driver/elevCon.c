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
            if(elevOrders[cur_floor][BUTTON_HALL_DOWN] == order) 
            {
                elevCon_serviceCurFloor(cur_floor, dir);
                return;
            }
        }
        if(dir == DIRN_UP)
        {
            if(elevOrders[cur_floor][BUTTON_HALL_UP] == order) 
            {
                elevCon_serviceCurFloor(cur_floor, dir);
                return;
            }
        }
        if(elevOrders[cur_floor][BUTTON_CAB] == order) elevCon_serviceCurFloor(cur_floor, dir);
    }
}

/*
if(!floors_in_direction(dir))
{
    elevio_motorDirection(dir*-1)
}
*/

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
        return true;
    }

    // Check all cab orders
    for(int f = 0; f < N_FLOORS; f++){
        if(elevOrders[f][BUTTON_CAB] == order) return true;
    }

    // Check down orders
    if(dir==DIRN_DOWN)
    {
        for(int floor = cur_floor; floor > 0; floor--)
        {
            if(elevOrders[floor][BUTTON_HALL_DOWN] == order) return true;
            if(elevOrders[floor][BUTTON_HALL_UP] == order) return true;
        }
    }

    // Check up orders
    if(dir==DIRN_UP)
    {
        for(int floor = cur_floor; floor < N_FLOORS-1; floor++)
        {
            if(elevOrders[floor][BUTTON_HALL_UP] == order) return true;
            if(elevOrders[floor][BUTTON_HALL_DOWN] == order) return true;
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
            elevio_buttonLamp(f, b, btnPressed);
            if(btnPressed)
            {
                elevOrders[f][b] = order;
                printf("Added order floor %i button %i\n", f, b);
                printf("The order for floor %i button %i is %i\n", f, b, elevOrders[f][b]);
            }
        }
    }
}

void elevCon_set_dir(MotorDirection dir)
{
    motor_dir = dir;
    elevio_motorDirection(dir);
}

void elevCon_start()
{
    if(elevio_floorSensor()==-1)
    {
        printf("In if test in elevcon start\n");
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
    print_dir();
    {
        printf("IN ELEVCON START LOOP\n");
        elevCon_should_change_direction(elevio_floorSensor(), motor_dir);
        elevCon_add_order();
    }
    printf("Exited elevCon_start\n");
}

/**
 * @brief Stop elevator, open door for three seconds, then close door and continue.
 * 
 */
void elevCon_serviceCurFloor(int cur_floor, MotorDirection dir) // , int order)
{
    elevCon_set_dir(DIRN_STOP);
    elevio_doorOpenLamp(1);
    sleep(3);
    elevio_doorOpenLamp(0);

    // Remove all orders from current floor
    for(int b = 0; b < N_BUTTONS; b++){
        elevOrders[cur_floor][b] = no_order;
    }

    if(cur_floor == 0 || cur_floor == N_FLOORS-1)
    {
        elevCon_set_dir(dir*-1);
        return;
    }

    elevCon_set_dir(dir);
    
    elevCon_should_change_direction(cur_floor, dir);
}

void elevCon_should_change_direction(int cur_floor, MotorDirection dir)
{
    if(cur_floor==-1) return;
    if(motor_dir == DIRN_STOP)
    {
        if(elevCon_floors_in_direction(cur_floor, DIRN_UP))
        {
            elevCon_set_dir(DIRN_UP);
            printf("Set direction up\n");
            return;
        }
        if(elevCon_floors_in_direction(cur_floor, DIRN_DOWN))
        {
            elevCon_set_dir(DIRN_DOWN);
            printf("Set direction down\n");
            return;
        }
    }

    if(!elevCon_floors_in_direction(cur_floor, motor_dir))
    {
        if(!elevCon_floors_in_direction(cur_floor, motor_dir*-1))
        {
            elevCon_set_dir(DIRN_STOP);
            return;
        }else{
            elevCon_set_dir(motor_dir*-1);
            printf("Change direction of eelvatre");
        }
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