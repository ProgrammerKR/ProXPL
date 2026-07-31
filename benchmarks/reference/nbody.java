public class nbody {
    static class Body {
        double x, y, z, mass, vx, vy, vz;
        Body(double x, double y, double z, double mass) {
            this.x = x; this.y = y; this.z = z; this.mass = mass;
        }
    }

    static void advance(Body[] bodies, double dt) {
        int n = bodies.length;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                double dx = bodies[i].x - bodies[j].x;
                double dy = bodies[i].y - bodies[j].y;
                double dz = bodies[i].z - bodies[j].z;
                
                double distSq = dx*dx + dy*dy + dz*dz;
                double mag = dt / (distSq * Math.sqrt(distSq));
                
                bodies[i].vx -= dx * bodies[j].mass * mag;
                bodies[i].vy -= dy * bodies[j].mass * mag;
                bodies[i].vz -= dz * bodies[j].mass * mag;
                
                bodies[j].vx += dx * bodies[i].mass * mag;
                bodies[j].vy += dy * bodies[i].mass * mag;
                bodies[j].vz += dz * bodies[i].mass * mag;
            }
        }
        for (int i = 0; i < n; i++) {
            bodies[i].x += bodies[i].vx * dt;
            bodies[i].y += bodies[i].vy * dt;
            bodies[i].z += bodies[i].vz * dt;
        }
    }

    public static void main(String[] args) {
        long start = System.nanoTime();
        Body[] bodies = {
            new Body(0, 0, 0, 100),
            new Body(10, 0, 0, 10),
            new Body(0, 10, 0, 10),
            new Body(0, 0, 10, 10)
        };
        for (int i = 0; i < 1000; i++) advance(bodies, 0.01);
        
        System.out.println("N-Body complete");
        double elapsed = (System.nanoTime() - start) / 1e9;
        System.out.println("Time: " + elapsed);
    }
}
