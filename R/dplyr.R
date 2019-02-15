
#' @importFrom dplyr group_data
group_labels <- function(data) {
  dplyr::select(group_data(data), -".rows")
}

group_sizes <- function(data) {
  lengths(group_data(data)$.rows)
}

subset_slices <- function(data, keep_groups = FALSE) {
  if (!dplyr::is_grouped_df(data)) {
    return(list(data))
  }

  cols <- setdiff(names(data), dplyr::group_vars(data))

  indices <- group_data(data)$.rows
  lapply(indices, function(x) data[x, cols])
}
