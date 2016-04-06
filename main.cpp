
#include <stdio.h>

#include "elev.h"
#include "matrix.h"



int main() {
    elev_init();
    printf("%s\n", "Successful init!\n");
    initMat();

    while (1) {

        // Stop elevator and exit program if the stop button is pressed
        if (elev_get_stop_signal()) {
            elev_set_motor_direction(DIRN_STOP);
            return 0;
        }

        listen();
        lamp();
        setQue();
        driveToFloor();


    }

    

}