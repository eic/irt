# Development log

* **BUG.txt** - the bug report as received (order-dependence of PID on
  `IdentifiedParticles`). Root cause: the `[particle,hypothesis]` combination index is
  built as `hdim^pdim` but was decoded with radix `nParticles` -- fixed on the
  `fix/v2.2.3` / `bench/*` branches.
* **TAGS.txt** - the first measurement/state ledger. **Abandoned** in favour of
  `MEMO-v00.txt`; kept only for provenance. Its entries predate the bug fix, so several
  numbers there are known to be affected by it.
* **MEMO-v00.txt** - the current ledger: tag history, measurement protocol
  (single-threaded, fixed event count, one run per setting, no averaging), measured
  numbers, config/state changes and rollbacks. Every number names the tag it was taken
  with.

Measurement scripts (`runbench.sh`, `apply-bench-patches.py`, `collect.py`) and the PFRICH
reco configs (`pfrich-reco*.json`) live in the sandbox root next to the simulation and
reconstruction outputs, not here.
