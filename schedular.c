#include "schedular.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

Scheduler* createScheduler(int numDoctors, char* file){
    Scheduler* scheduler = (Scheduler*)malloc(sizeof(Scheduler));
    scheduler->numberDoctors = numDoctors;
    scheduler->doctors = initializeDoctors(numDoctors);
    scheduler->logFile = fopen(file,"w");
    return scheduler;
}

void destroyScheduler(Scheduler* scheduler){
    fclose(scheduler->logFile);
    destroyDoctor(scheduler->doctors, scheduler->numberDoctors);
    free(scheduler);
}

Doctor* findAvailableDoctor(Doctor* doctors, int numDoctors, FILE* file){
    for (int i = 0; i < numDoctors; i++){
        if(pthread_mutex_trylock(&doctors[i].mutex) == 0){
            fprintf(file, "Doctorul %i este in consultatie\n", i);
            return &doctors[i];
        }
    }
    return NULL;
}

void writeLog(Scheduler* scheduler, pthread_t patient_id, time_t waitTime, time_t consultationTime){
    fprintf(scheduler->logFile, "Pacientul %lu a asteptat %u secunde dupa ce a ajuns ca sa intre la un medic si a stat %u secunde in consultatie.\n", patient_id, (unsigned int)waitTime, (unsigned int)consultationTime);
}

void* patientThreadFunction(void* arg){

    Scheduler* params = (Scheduler*)arg;

    srand((unsigned int)time(NULL));

    time_t startTime = time(NULL);
    time_t waitTime, consultationTime;
    
    bool finishedConsultation = false;
    while(!finishedConsultation){
        Doctor* doctor = findAvailableDoctor(params->doctors, (params->numberDoctors), params->logFile);
        if(doctor != NULL){
            waitTime = time(NULL) - startTime;  
            consultationTime = rand() % 6;
            sleep(consultationTime); // Each patient can stay a maximum of 5 seconds in the doctors office
            pthread_mutex_unlock(&doctor->mutex);
            finishedConsultation = true;
            writeLog(params, pthread_self(), waitTime, consultationTime);
        }
    }
    
    return NULL;

}

void startSimulation(Scheduler* scheduler, unsigned int patientsSpawnTime, unsigned int patientsSpawnCoolDown){
    srand((unsigned int)time(NULL)); // Seed for random number generation

    time_t startTime = time(NULL);
    time_t currentTime;

    while((currentTime = time(NULL)) - startTime < patientsSpawnTime) {
        int sleepTime = rand() % patientsSpawnCoolDown; // Sleep for a random duration (0 to 14 seconds)
        sleep(sleepTime);

        pthread_t patient;

        if (pthread_create(&patient, NULL, patientThreadFunction, scheduler) != 0){
            fprintf(stderr, "Error creating thread\n");
            return;
        }

        pthread_detach(patient); // Patient is done with consultation and logging info
    }
}


