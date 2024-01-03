#include <stdio.h>
#include "doctor.h"
#include "schedular.h"

int main(int argc, char *argv[], char* envp[]) {
    printf("In main\n");
    Scheduler* scheduler;
    if(argv[1] != NULL){
        scheduler = createScheduler(3, argv[1]);
    } else {
        scheduler = createScheduler(3, "shedulerLog.txt");
    }
    printf("After create scheduler\n");
    startSimulation(scheduler);
    printf("After simulation\n");
    destroyScheduler(scheduler);
    printf("After deletion\n");
    
    return 0;
}
