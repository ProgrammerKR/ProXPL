# Chrono-Native Logic

Data with expiration dates and temporal validity.

## Overview

Chrono-Native features allow data to have temporal constraints.

## Syntax

```proxpl
temporal var = getData() decay after 24h;
```

## Status

**FRONTEND_ONLY** - Tokens exist and parser handles temporal limits, but there is zero bytecode generation or VM support for scheduling or invalidating memory based on time decay.

---

**Status**: Frontend Only
