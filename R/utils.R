#' Pipe operator
#'
#' @name %>%
#' @rdname pipe
#' @keywords internal
#' @export
#' @importFrom magrittr %>%
#' @usage lhs \%>\% rhs
NULL

names2 <- function(x) {
  names(x) %||% rep("", length(x))
}

`%||%` <- function(x, y) {
  if (is.null(x)) {
    y
  } else {
    x
  }
}

isFALSE <- function(x) identical(x, FALSE)

ndots <- function(...) nargs()

inv_which <- function(x, sel) {
  if (is.character(sel)) {
    names <- names(x)
    if (is.null(names)) {
      stop("character indexing requires a named object", call. = FALSE)
    }
    names %in% sel
  } else if (is.numeric(sel)) {
    seq_along(x) %in% sel
  } else {
    stop("unrecognised index type", call. = FALSE)
  }
}

deprecate <- function(...) {
  # No message for now
}

as_function <- function(...) {
  if (utils::packageVersion("purrr") > "0.2.2") {
    purrr::as_mapper(...)
  } else {
    purrr::as_function(...)
  }
}
