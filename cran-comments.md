
This fixes the rchk issues.


## Test environments

* local OS X install, R release
* ubuntu 12.04 (on travis-ci), R release
* win-builder (devel and release)


## R CMD check results

0 errors | 0 warnings | 0 notes


## Reverse dependencies

I didn't check reverse dependencies because only `PROTECT()`s were added.
