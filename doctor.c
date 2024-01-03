#include "doctor.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

Doctor* initializeDoctors(int numDoctors){
    printf("In constructor of Doctor\n");
    Doctor* doctors = (Doctor*)malloc(numDoctors * sizeof(Doctor));
    
    for (int i = 0; i < numDoctors; i++){
        pthread_mutex_init(&doctors[i].mutex, NULL);
        doctors[i].id_doctor = i + 1;
    }

    printf("Out of constructor of doctor\n");
    return doctors;
}

void destroyDoctor(Doctor* doctors, int numDoctors){
    printf("In destructor of doctor\n");
    for (int i = 0; i < numDoctors; i++){
        pthread_mutex_destroy(&doctors[i].mutex);
    }

    free(doctors);
    printf("Out of destructor of doctor\n");
}
