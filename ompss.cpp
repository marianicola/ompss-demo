#include <iostream>
#include <cmath>

const int N = 8;
const int GROUP_SIZE = 4;
const double DT = 0.01;
const double EPS = 0.001;

struct Body {
    double x;
    double v;
    double m;
};

void compute_forces_for_group(Body bodies[N], double forces[N], int start, int end) {
    for (int i = start; i < end; i++) {
        forces[i] = 0.0;

        for (int j = 0; j < N; j++) {
            if (i != j) {
                double dx = bodies[j].x - bodies[i].x;
                double distance = std::abs(dx) + EPS;
                double force = (bodies[i].m * bodies[j].m * dx) / (distance * distance);
                forces[i] += force;
            }
        }
    }
}

void update_bodies_for_group(Body bodies[N], double forces[N], int start, int end) {
    for (int i = start; i < end; i++) {
        double acceleration = forces[i] / bodies[i].m;
        bodies[i].v = bodies[i].v + acceleration * DT;
        bodies[i].x = bodies[i].x + bodies[i].v * DT;
    }
}

int main() {
    Body bodies[N];
    double forces[N];

    for (int i = 0; i < N; i++) {
        bodies[i].x = i + 1.0;
        bodies[i].v = 0.0;
        bodies[i].m = 1.0 + i;
        forces[i] = 0.0;
    }

    std::cout << "Initial positions:\n";
    for (int i = 0; i < N; i++) {
        std::cout << "Body " << i
                  << ": x = " << bodies[i].x
                  << ", v = " << bodies[i].v
                  << ", m = " << bodies[i].m << "\n";
    }

    #pragma oss task in(bodies[0;N]) out(forces[0;GROUP_SIZE])
    compute_forces_for_group(bodies, forces, 0, GROUP_SIZE);

    #pragma oss task in(bodies[0;N]) out(forces[GROUP_SIZE;GROUP_SIZE])
    compute_forces_for_group(bodies, forces, GROUP_SIZE, N);

    #pragma oss taskwait

    #pragma oss task in(forces[0;GROUP_SIZE]) inout(bodies[0;GROUP_SIZE])
    update_bodies_for_group(bodies, forces, 0, GROUP_SIZE);

    #pragma oss task in(forces[GROUP_SIZE;GROUP_SIZE]) inout(bodies[GROUP_SIZE;GROUP_SIZE])
    update_bodies_for_group(bodies, forces, GROUP_SIZE, N);

    #pragma oss taskwait

    std::cout << "\nForces:\n";
    for (int i = 0; i < N; i++) {
        std::cout << "Body " << i << ": force = " << forces[i] << "\n";
    }

    std::cout << "\nUpdated positions:\n";
    for (int i = 0; i < N; i++) {
        std::cout << "Body " << i
                  << ": x = " << bodies[i].x
                  << ", v = " << bodies[i].v << "\n";
    }

    return 0;
}