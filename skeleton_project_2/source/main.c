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
        int dir = elevCon_get_dir();

        // elevCon_updateFloorLight();

        /*
        if(elevio_obstruction()){
            elevio_stopLamp(1);
        } else {
            elevio_stopLamp(0);
        }
        */
        
        /*
        // Stop Elewator
        if(elevio_stopButton()){
            elevCon_emergencyStop();
            // printf("Stop was run a time\n");
        }
        */

        if(dir==DIRN_STOP)
        {
            elevCon_should_change_direction(floor, dir);
        }

        if(floor == 0 || floor == N_FLOORS-1)
        {
            printf("At floor 1 or 4");
            elevCon_should_change_direction(floor, dir);
        }

        if(floor != -1)
        {
            elevCon_checkFloor(floor, dir);
        }

        elevCon_add_order();

        nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    }

    return 0;
}
