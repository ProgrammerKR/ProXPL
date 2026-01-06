import time
import math

class Body:
    def __init__(self, x, y, z, mass):
        self.x = x
        self.y = y
        self.z = z
        self.mass = mass
        self.vx = 0
        self.vy = 0
        self.vz = 0

def advance(bodies, dt):
    n = len(bodies)
    for i in range(n):
        for j in range(i + 1, n):
            b1 = bodies[i]
            b2 = bodies[j]
            
            dx = b1.x - b2.x
            dy = b1.y - b2.y
            dz = b1.z - b2.z
            
            dist_sq = dx*dx + dy*dy + dz*dz
            mag = dt / (dist_sq * math.sqrt(dist_sq))
            
            b1.vx -= dx * b2.mass * mag
            b1.vy -= dy * b2.mass * mag
            b1.vz -= dz * b2.mass * mag
            
            b2.vx += dx * b1.mass * mag
            b2.vy += dy * b1.mass * mag
            b2.vz += dz * b1.mass * mag
            
    for i in range(n):
        b = bodies[i]
        b.x += b.vx * dt
        b.y += b.vy * dt
        b.z += b.vz * dt

def main():
    start = time.time()
    bodies = [
        Body(0, 0, 0, 100),
        Body(10, 0, 0, 10),
        Body(0, 10, 0, 10),
        Body(0, 0, 10, 10)
    ]
    
    for _ in range(1000):
        advance(bodies, 0.01)
        
    print("N-Body complete")
    elapsed = time.time() - start
    print(f"Time: {elapsed:.6f}")

if __name__ == "__main__":
    main()
