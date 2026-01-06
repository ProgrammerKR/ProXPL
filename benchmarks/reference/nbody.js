class Body {
    constructor(x, y, z, mass) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.mass = mass;
        this.vx = 0;
        this.vy = 0;
        this.vz = 0;
    }
}

function advance(bodies, dt) {
    const n = bodies.length;
    for (let i = 0; i < n; i++) {
        for (let j = i + 1; j < n; j++) {
            const b1 = bodies[i];
            const b2 = bodies[j];
            
            const dx = b1.x - b2.x;
            const dy = b1.y - b2.y;
            const dz = b1.z - b2.z;
            
            const dist_sq = dx*dx + dy*dy + dz*dz;
            const mag = dt / (dist_sq * Math.sqrt(dist_sq));
            
            b1.vx -= dx * b2.mass * mag;
            b1.vy -= dy * b2.mass * mag;
            b1.vz -= dz * b2.mass * mag;
            
            b2.vx += dx * b1.mass * mag;
            b2.vy += dy * b1.mass * mag;
            b2.vz += dz * b1.mass * mag;
        }
    }
    
    for (let i = 0; i < n; i++) {
        const b = bodies[i];
        b.x += b.vx * dt;
        b.y += b.vy * dt;
        b.z += b.vz * dt;
    }
}

function main() {
    const start = process.hrtime();
    const bodies = [
        new Body(0, 0, 0, 100),
        new Body(10, 0, 0, 10),
        new Body(0, 10, 0, 10),
        new Body(0, 0, 10, 10)
    ];
    
    for (let i = 0; i < 1000; i++) {
        advance(bodies, 0.01);
    }
    
    console.log("N-Body complete");
    const diff = process.hrtime(start);
    const elapsed = diff[0] + diff[1] / 1e9;
    console.log(`Time: ${elapsed.toFixed(6)}`);
}

main();
