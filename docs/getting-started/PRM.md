# Using PRM (ProX Repository Manager)

PRM is the built-in package manager for ProXPL.

## Commands

```bash
prm init my-project     # Initialize new project
prm add <package>       # Add dependency
prm build               # Build project
prm run                 # Run project
prm test                # Run tests
prm list                # List installed packages
prm search <query>      # Search packages
prm update              # Update dependencies
```

## Project Structure

`project.pxcf`:
```javascript
project {
    name: "my-web-server"
    version: "1.1.0"
}

dependencies {
    http: "1.3.0"
    json: "1.1.0"
}
```

## See Also

- [Registry](../packages/REGISTRY.md)
- [Build Guide](BUILD_GUIDE.md)
