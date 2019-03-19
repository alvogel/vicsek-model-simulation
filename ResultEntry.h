#ifndef RESULTENTRY_H_INCLUDED
#define RESULTENTRY_H_INCLUDED

struct ResultEntry{

    unsigned int n_iterations;  // Number of iteration steps of the simulation
    unsigned int n_particles;   // Number of particles
    float avg_norm_vel;         // Average normalized velocity
    float v;                    // particle velocity
    float eta;                  // noise parameter
    float radius;               // range radius

};

#endif // RESULTENTRY_H_INCLUDED
