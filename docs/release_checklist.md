# PhoenixWire Release Checklist

This release checklist is strictly tied to the `end_to_end_validation.md` proof document. Before cutting any new beta or stable release tag, the following must be validated.

## 1. Native & Extension Verification
- [ ] `make test` passes locally and in CI for the C core.
- [ ] `ext-phoenixwire` builds and passes memory leak checks (Valgrind/ASAN).
- [ ] Version constants in `pw_version.h` and PHP `MINIT` match the release tag.

## 2. End-to-End Validation Proofs
- [ ] **TLS & Auth:** `ext-phoenixwire` connects with TLS 1.3 to the test daemon; bearer auth succeeds.
- [ ] **Shared Host Fallback:** Pure-PHP `StreamClient` connects via `tls://` and handles varint framing accurately.
- [ ] **Reconnection & Resume:** Graceful session resume triggered after forced TCP disconnects.
- [ ] **Load & Backpressure:** Soak test passes at least 20,000 iterations without memory overflow or queue deadlocks.

## 3. Artifact Generation
- [ ] Composer `composer.json` version bumped.
- [ ] PECL extension archive generated.
- [ ] Docker image tagged and pushed.

## 4. Documentation & Communication
- [ ] Compatibility Matrix updated.
- [ ] Release Notes generated.
- [ ] Runbook verified against any configuration changes.
