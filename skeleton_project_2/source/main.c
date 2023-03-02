#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "driver/elevCon.h"
#include "driver/elevio.h"

int main(){
    elevio_init();
    
    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");
    
    elevCon_start();
    // printf("Start works\n");

    while(1){
        int floor = elevio_floorSensor();
        /*int dir = elevCon_get_dir();
        printf("floor: %d \n",floor);

        elevCon_updateFloorLight();
        printf("Floorlight works\n");

        if(elevio_obstruction()){
            elevio_stopLamp(1);
        } else {
            elevio_stopLamp(0);
        }
        
        // Stop Elewator
        if(elevio_stopButton()){
            elevCon_emergencyStop();
            // printf("Stop was run a time\n");
        }

        elevCon_add_order();
        printf("Orders added\n");
        elevCon_checkFloor(floor, dir);
        // printf("floors checked\n");

        // Change direction

        if(floor == 0 || floor == N_FLOORS-1)
        {
            elevCon_should_change_direction(floor);
            // printf("Checked if change direction\n");
        }
        
        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);

        elevCon_print_orders();

        print("The motor direction is: %i\n", motor_dir);
        */

        elevio_floorIndicator(2);

        print_dir();

        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    return 0;
}
