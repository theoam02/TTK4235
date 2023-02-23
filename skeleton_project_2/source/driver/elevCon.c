#include "elevCon.h"
#include "elevio.h"
#include <stdbool.h>
#include <time.h>

// Variables

/**
 * @brief array of orders from the elevator panel
 * 
 */

bool elevPanelOrders[4] =
{
    false,
    false,
    false,
    false
};

/**
 * @brief 
 * 
 */
bool floorPanelsUp[3] =
{
    false,
    false,
    false
};

bool floorPanelsDown[3] =
{
    false,
    false,
    false
};


void elevCon_emergencyStop()
{
    // Remove all orders

    for(int i=0;i<3;i++)
    {
        elevPanelOrders[i] = false;
    }

    for(int i=0;i<2;i++)
    {
        floorPanelsDown[i] = false;
        floorPanelsUp[i] = false;
    }

    if(elevio_floorSensor()!=-1) elevio_doorOpenLamp(1);
}
void elevCon_enableServiceInLift();
void elevCon_enableServiceOutLift();
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
void elevCon_checkFloor();