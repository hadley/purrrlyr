
df <- mtcars[1:3, c("wt", "qsec")]
df[[2]] <- as.character(df[[2]])
suppressWarnings(grouped <- slice_rows(mtcars[1:2], "cyl"))

gen_alternatives <- function(first, alt) {
  prev_alt <- TRUE
  function(...) {
    if (prev_alt) {
      out <- first
    } else {
      out <- alt
    }
    prev_alt <<- !prev_alt
    out
  }
}

all_nulls <- function(...) NULL
scalars <- function(...) paste("a", ..1)
empty <- function(...) numeric(0)
vectors <- function(...) paste(letters[1:2], c(...))
dataframes <- function(...) df
empty_dataframes <- function(...) df[0, ]
objects <- function(...) function() {}

scalar_nulls <- gen_alternatives(1L, NULL)
scalar_first_nulls <- gen_alternatives(NULL, 1L)
scalar_first_nulls <- gen_alternatives(NULL, 1L)
dataframes_nulls <- gen_alternatives(df, NULL)
some_empty_dataframes <- gen_alternatives(df, df[0, ])
unconsistent_names <- gen_alternatives(df, purrr::set_names(df, 1:2))
unconsistent_types <- gen_alternatives(df, purrr::map(df, as.character))
ragged_dataframes <- gen_alternatives(df, rbind(df, df))
ragged_vectors <- gen_alternatives(letters[1:2], rep(letters[1:2], 2))
