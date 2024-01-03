#include <pthread.h>

#ifndef DOCTORS_H_INCLUDED
#define DOCTORS_H_INCLUDED

// Doctor struct
typedef struct {
    pthread_mutex_t mutex;
    int id_doctor;
} Doctor;

Doctor* initializeDoctors(int numDoctor);
void destroyDoctor(Doctor* doctor, int numDoctors);

#endif
