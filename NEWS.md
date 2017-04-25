# purrlyr 0.0.0.9000

All data-frame based mappers have been moved to this package. These functions are not technically deprecated (so you can move to this package as easily as possible), but these functions are unlikely to be changed in the future (i.e. there will be no bug fixes) and are likely to go away in the near future, so we highly recommend updating to new approaches.

* Mapping a function to each column of a data frame should now be
  handled with the colwise mutating and summarising operations in
  dplyr instead of `dmap()`. These are the verbs with suffix
  `_all()`, `_at()` and `_if()`, such as `mutate_all()` or
  `summarise_if()`. Note that this means the output of `.f` should
  conform to the requirements of dplyr operations: same length as
  the input for mutating operations, and length 1 for summarising
  operations.

* Inovking a function row by row with the columns of a data frame
  as arguments should be done with `pmap()` followed by
  `dplyr::as_dataframe()` instead of `map_rows()`.

* Mapping rowwise slices of a data frame with `by_row()` is
  deprecated in favour of a combination of tidyverse functions.
  First use `tidyr::nest()` to create a list-column containing
  groupwise data frames. Then use `dplyr::mutate()` to operate on
  this list-column. Typically you will want to apply a function on
  each element (nested data frame) of this list-column with
  `purrr::map()`.
