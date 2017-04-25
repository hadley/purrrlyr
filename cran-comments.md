## Test environments
* local OS X install, R 3.3.2
* ubuntu 12.04 (on travis-ci), R 3.3.2
* win-builder (devel and release)

## R CMD check results

0 errors | 0 warnings | 2 note

* This is a new release.

* checking dependencies in R code
  Missing or unexported object: 'purrr::as_mapper'
  
  This is needed for forward compatibility with the purrr package.
