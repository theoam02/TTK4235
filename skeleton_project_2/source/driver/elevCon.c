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
        if(dir == DIRN_DOWN)
        {
            if(elevOrders[cur_floor][1] == order) 
            {
                elevCon_serviceCurFloor(cur_floor);
                // elevOrders[cur_floor][2] = no_order;
                return;
            }
        }
        if(dir == DIRN_UP)
        {
            if(elevOrders[cur_floor][0] == order) 
            {
                elevCon_serviceCurFloor(cur_floor);
                // elevOrders[cur_floor][2] = no_order;
                return;
            }
        }
        if(elevOrders[cur_floor][2] == order) elevCon_serviceCurFloor(cur_floor);
    }
}

/*
if(!floors_in_direction(dir))
{
    elevio_motorDirection(dir*-1)
}
*/

bool elevCon_floors_in_direction(int cur_floor, MotorDirection dir)
{
    // Check all cab orders
    for(int f = 0; f < N_FLOORS; f++){
        if(elevOrders[f][2] == order) return true;
    }

    if(dir == DIRN_DOWN || dir == DIRN_STOP)
    {
        for(int floor = cur_floor; floor > 0; floor--)
        {
            if(elevOrders[floor][1] == order) return true;
        }
    }

    if(dir == DIRN_UP || dir == DIRN_STOP)
    {
        for(int floor = cur_floor; floor < N_FLOORS-1; floor++)
        {
            if(elevOrders[floor][0] == order) return true;
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
    if(elevio_floorSensor()!=-1)
    {
        return;
    }else
    {
        elevio_floorIndicator(elevio_floorSensor()+1);
    }
}

void elevCon_add_order()
{
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            int btnPressed = elevio_callButton(f, b);
            elevio_buttonLamp(f, b, btnPressed);
            if(btnPressed) elevOrders[f][b] = order;
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
        printf("In if test in elevcon start");
        elevCon_set_dir(DIRN_DOWN);
        while(elevio_floorSensor()==-1){
            for(int f = 0; f < N_FLOORS; f++){
                for(int b = 0; b < N_BUTTONS; b++){
                    int btnPressed = elevio_callButton(f, b);
                    elevOrders[f][b] = no_order;
                }
            }
        }
        elevCon_set_dir(DIRN_STOP);
    }
    print_dir();
    while(motor_dir==DIRN_STOP)
    {
        elevCon_should_change_direction(elevio_floorSensor());
        elevCon_add_order();
        elevCon_print_orders();
    }
    printf("Exited thing");
}

/**
 * @brief Stop elevator, open door for three seconds, then close door and continue.
 * 
 */
void elevCon_serviceCurFloor(int cur_floor) // , int order)
{
    elevCon_set_dir(DIRN_STOP);
    elevio_doorOpenLamp(1);
    sleep(3);
    elevio_doorOpenLamp(0);
    
    // elevOrders[cur_floor][order];

    // Remove all orders
    for(int b = 0; b < N_BUTTONS; b++){
        int btnPressed = elevio_callButton(cur_floor, b);
        elevOrders[cur_floor][b] = no_order;
    }

    elevCon_should_change_direction(cur_floor);
}

void elevCon_should_change_direction(int cur_floor)
{
    if(motor_dir == DIRN_STOP)
    {

    }

    if(!elevCon_floors_in_direction(cur_floor, motor_dir))
    {
        if(!elevCon_floors_in_direction(cur_floor, motor_dir*-1))
        {
            elevCon_set_dir(DIRN_STOP);
            return;
        }
        elevCon_set_dir(motor_dir*-1);
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