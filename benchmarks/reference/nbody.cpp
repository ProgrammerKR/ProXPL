#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

struct Body {
    double x, y, z, mass, vx, vy, vz;
    Body(double x, double y, double z, double mass) : x(x), y(y), z(z), mass(mass), vx(0), vy(0), vz(0) {}
};

void advance(std::vector<Body>& bodies, double dt) {
    int n = bodies.size();
    for(int i=0; i<n; i++) {
        for(int j=i+1; j<n; j++) {
            double dx = bodies[i].x - bodies[j].x;
            double dy = bodies[i].y - bodies[j].y;
            double dz = bodies[i].z - bodies[j].z;
            
            double dist_sq = dx*dx + dy*dy + dz*dz;
            double mag = dt / (dist_sq * std::sqrt(dist_sq));
            
            bodies[i].vx -= dx * bodies[j].mass * mag;
            bodies[i].vy -= dy * bodies[j].mass * mag;
            bodies[i].vz -= dz * bodies[j].mass * mag;
            
            bodies[j].vx += dx * bodies[i].mass * mag;
            bodies[j].vy += dy * bodies[i].mass * mag;
            bodies[j].vz += dz * bodies[i].mass * mag;
        }
    }
    for(int i=0; i<n; i++) {
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
        bodies[i].z += bodies[i].vz * dt;
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Body> bodies;
    bodies.emplace_back(0, 0, 0, 100);
    bodies.emplace_back(10, 0, 0, 10);
    bodies.emplace_back(0, 10, 0, 10);
    bodies.emplace_back(0, 0, 10, 10);
    
    for(int i=0; i<1000; i++) advance(bodies, 0.01);
    
    std::cout << "N-Body complete\n";
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time: " << elapsed.count() << "\n";
    return 0;
}
