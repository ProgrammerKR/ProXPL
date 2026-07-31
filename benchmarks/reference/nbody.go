package main
import ("fmt"; "time"; "math")
type Body struct { x, y, z, mass, vx, vy, vz float64 }
func advance(bodies []Body, dt float64) {
    n := len(bodies)
    for i := 0; i < n; i++ {
        for j := i + 1; j < n; j++ {
            dx := bodies[i].x - bodies[j].x
            dy := bodies[i].y - bodies[j].y
            dz := bodies[i].z - bodies[j].z
            distSq := dx*dx + dy*dy + dz*dz
            mag := dt / (distSq * math.Sqrt(distSq))
            bodies[i].vx -= dx * bodies[j].mass * mag
            bodies[i].vy -= dy * bodies[j].mass * mag
            bodies[i].vz -= dz * bodies[j].mass * mag
            bodies[j].vx += dx * bodies[i].mass * mag
            bodies[j].vy += dy * bodies[i].mass * mag
            bodies[j].vz += dz * bodies[i].mass * mag
        }
    }
    for i := 0; i < n; i++ {
        bodies[i].x += bodies[i].vx * dt
        bodies[i].y += bodies[i].vy * dt
        bodies[i].z += bodies[i].vz * dt
    }
}
func main() {
    start := time.Now()
    bodies := []Body{
        {0, 0, 0, 100, 0, 0, 0},
        {10, 0, 0, 10, 0, 0, 0},
        {0, 10, 0, 10, 0, 0, 0},
        {0, 0, 10, 10, 0, 0, 0},
    }
    for i := 0; i < 1000; i++ { advance(bodies, 0.01) }
    fmt.Println("N-Body complete")
    fmt.Printf("Time: %.6f\n", time.Since(start).Seconds())
}
