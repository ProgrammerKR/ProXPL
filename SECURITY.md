# Security Policy

## Reporting a Vulnerability

If you discover a security vulnerability in ProXPL, please report it responsibly:

- **Email**: security@proxentix.com
- **Response time**: We aim to acknowledge reports within 48 hours and provide a detailed fix timeline within 7 days.
- **Disclosure**: Please do not publicly disclose the vulnerability until we have released a patch.

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 1.9.x   | :white_check_mark: |
| 1.8.x   | :x:                |
| < 1.8   | :x:                |

## Security Considerations

ProXPL includes several features that have inherent security implications:

- **FFI (`extern`)**: Allows calling arbitrary C functions. This can execute arbitrary native code. Only use `extern` with trusted libraries.
- **`Sys.execute()` / `OS.execute()`**: Execute shell commands. Always sanitize user input before passing to these functions.
- **Garbage Collection**: ProXPL uses a mark-and-sweep garbage collector. Do not rely on finalizers for security-critical cleanup.

## Known Limitations

- The standard library does not currently sandbox file system or network access.
- The bytecode verifier is basic; malformed bytecode can cause runtime errors.
- Random number generation is not cryptographically secure. Do not use `random()` for security-sensitive purposes.
