# childsds

Version: 0.7.1

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘gamlss.dist’
      All declared Imports should be used.
    ```

*   checking data for non-ASCII characters ... NOTE
    ```
      Note: found 24 marked UTF-8 strings
    ```

# eechidna

Version: 1.1

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  6.6Mb
      sub-directories of 1Mb or more:
        data   5.1Mb
        doc    1.2Mb
    ```

# ggstatsplot

Version: 0.0.8

## In both

*   checking tests ...
    ```
     ERROR
    Running the tests in ‘tests/testthat.R’ failed.
    Last 13 lines of output:
      2 levels with non-zero frequencies.
      Note: 99% CI for effect size estimate was computed with 15 bootstrap samples.
      Error in chisq.test(counts, p = expProps) : 
        'x' must at least have 2 elements
      Warning: Proportion test will not be run because it requires x to have at least 
      2 levels with non-zero frequencies.
      Error in chisq.test(counts, p = expProps) : 
        'x' must at least have 2 elements
      Note: 95% CI for effect size estimate was computed with 100 bootstrap samples.
      ══ testthat results  ═══════════════════════════════════════════════════════════
      OK: 686 SKIPPED: 54 FAILED: 1
      1. Failure: check merMod output (@test_ggcoefstats.R#515) 
      
      Error: testthat unit tests failed
      Execution halted
    ```

