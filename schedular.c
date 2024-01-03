#include "schedular.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

typedef struct{
    Scheduler* scheduler;
    int patient_id;
} ThreadParams;

Scheduler* createScheduler(int numDoctors, char* file){
    printf("In constructor of scheduler\n");
    Scheduler* scheduler = (Scheduler*)malloc(sizeof(Scheduler));
    scheduler->numberDoctors = numDoctors;
    scheduler->doctors = initializeDoctors(numDoctors);
    scheduler->logFile = fopen(file,"w");
    printf("Out of constructor of scheduler\n");
    return scheduler;
}

void destroyScheduler(Scheduler* scheduler){
    printf("In destructor of scheduler\n");
    fclose(scheduler->logFile);
    destroyDoctor(scheduler->doctors, scheduler->numberDoctors);
    free(scheduler);
    printf("Out of destructor of scheduler\n");
}

Doctor* findAvailableDoctor(Doctor* doctors, int numDoctors){
    printf("In find doctor function\n");
    for (int i = 0; i < numDoctors; i++){
        if(pthread_mutex_trylock(&doctors[i].mutex) == 0){
            printf("Out of find doctor function\n");
            return &doctors[i];
        }
    }
    printf("Out of find doctor function\n");
    return NULL;
}

void writeLog(Scheduler* scheduler, int patient_id, time_t waitTime, time_t consultationTime){
    printf("In write function\n");
    fprintf(scheduler->logFile, "Pacientul %i a asteptat %u secunde dupa ce a ajuns ca sa intre la un medic si a stat %u secunde in consultatie.\n", patient_id, (unsigned int)waitTime, (unsigned int)consultationTime);
    printf("Out of write function\n");
}

void* patientThreadFunction(void* arg){
    printf("In patient thread function\n");

    ThreadParams* params = (ThreadParams*)arg;

    srand((unsigned int)time(NULL));

    time_t startTime = time(NULL);
    time_t waitTime, consultationTime;
    
    bool finishedConsultation = false;
    while(!finishedConsultation){
        Doctor* doctor = findAvailableDoctor(params->scheduler->doctors, params->scheduler->numberDoctors);
        if(doctor != NULL){
            waitTime = time(NULL) - startTime;  
            consultationTime = rand() % 6;
            sleep(consultationTime); // Each patient can stay a maximum of 5 seconds in the doctors office
            pthread_mutex_unlock(&doctor->mutex);
            finishedConsultation = true;
            writeLog(params->scheduler, params->patient_id, waitTime, consultationTime);
        }
    }
    
    printf("Out of patient thread function\n");
    return NULL;

}

void startSimulation(Scheduler* scheduler){
    printf("In start simulation\n");
    srand((unsigned int)time(NULL)); // Seed for random number generation

    time_t startTime = time(NULL);
    time_t currentTime;
    int patient_id = 0;

    while((currentTime = time(NULL)) - startTime < 60) {
        int sleepTime = rand() % 15; // Sleep for a random duration (0 to 14 seconds)
        sleep(sleepTime);

        pthread_t patient;
        patient_id++;

        ThreadParams params = { scheduler, patient_id };

        if (pthread_create(&patient, NULL, patientThreadFunction, &params) != 0){
            fprintf(stderr, "Error creating thread\n");
            return;
        }

        pthread_detach(patient); // Patient is done with consultation and logging info
    }
    printf("Out of start simulation");
}


