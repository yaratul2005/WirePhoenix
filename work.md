# PhoenixWire Development Log

This document tracks the architectural history and development phases of the PhoenixWire project.
**IMPORTANT RULE:** This file must be updated after every significant change or phase completion to maintain an accurate project history.

## Phase 1: Native Core Architecture
- Initialized project structure with `native/`, `extension/`, `php/`, `tools/`, and `docs/`.
- Designed and implemented the native C transport engine.
- Created robust connection finite state machine (FSM): `CONNECTING` -> `HANDSHAKING` -> `AUTHENTICATING` -> `READY` -> `CLOSED`.
- Implemented non-blocking TCP socket I/O with epoll/select readiness.
- Developed the frame codec utilizing variable-length integers (varints) for payload lengths to minimize wire overhead.
- Introduced opcodes for `DATA_BINARY`, `PING`, `PONG`, `CLOSE`, and `AUTH`.
- Integrated backpressure management and connection queue limits.

## Phase 2: PHP Extension Bridge
- Created `config.m4` and scaffolding for a custom PHP extension.
- Mapped the C Core FSM and transport functionalities into PHP space.
- Leveraged Zend object handlers for proper memory and lifecycle management of `Client`, `Server`, and `Session` objects.
- Bridged exception models and error codes from native C to PHP classes (`PhoenixWireException`, etc.).

## Phase 3: High-Level PHP API & Async Support
- Implemented a PHP userland API defining synchronous and asynchronous interaction patterns.
- Provided `AsyncClient` wrapper demonstrating non-blocking `tick()` event loops.
- Defined explicit classes for configuration (`Policy`) and structured errors.

## Phase 4: Testing & Hardening
- Created extensive native test suites (`test_integration.c`, `test_soak.c`, `test_chaos.c`) mimicking edge cases like random socket closures, malformed frames, and memory boundary conditions.
- Developed PHP-level benchmark tools (`tools/benchmark.php`) validating messages-per-second and latency targets.

## Phase 5: Branding & Documentation
- **[Current]** Overhauled `README.md` to align with new branding requirements.
- Included the "Phoenix" logo (`RDP (1).png`) into `assets/phoenix-logo.png`.
- Added required ownership credits to the primary documentation.
- Created this `work.md` file to formally track the architectural timeline and satisfy the logging constraint.
