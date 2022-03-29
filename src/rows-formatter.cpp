#include <Rcpp.h>
#include "utils.h"
#include "fast-copy.h"
#include "rows-data.h"
#include "rows-formatter.h"

namespace rows {


FormatterPtr Formatter::create(Results& results, Labels& labels, Settings& settings) {
  switch(settings.collation) {
  case rows: return FormatterPtr(new RowsFormatter(results, labels, settings)); break;
  case cols: return FormatterPtr(new ColsFormatter(results, labels, settings)); break;
  case list: return FormatterPtr(new ListFormatter(results, labels, settings)); break;
  }

  stop("Unsupported collation type.");
  return FormatterPtr();
}

int Formatter::labels_size() {
  if (settings_.include_labels)
    return labels_.size();
  else
    return 0;
}

void Formatter::check_nonlist_consistency() {
  switch (results_.type) {
  case nulls:
    stop("results are all NULL and can't be cols/rows collated");
    break;
  case dataframes:
    check_dataframes_consistency(results_.get());
    break;
  case objects:
    stop(".f must return either data frames or vectors for non-list collation");
    break;
  default:
    break;
  }
}

void ColsFormatter::check_nonlist_consistency() {
  switch (results_.type) {
  case vectors:
  case dataframes:
    if (!results_.equi_sized)
      stop(".f should return equal length vectors or data frames for collating on `cols`");
    break;
  default:
    break;
  }
  Formatter::check_nonlist_consistency();
}

void ColsFormatter::adjust_results_sizes() {
  switch (results_.type) {
  case vectors:
  case dataframes:
    std::fill(results_.sizes.begin(), results_.sizes.end(), 1);
    break;
  default:
    break;
  }
}

void ListFormatter::adjust_results_sizes() {
  std::fill(results_.sizes.begin(), results_.sizes.end(), 1);
}

void Formatter::determine_dimensions() {
  if (settings_.collation == list)
    n_rows_ = results_.n_slices;
  else
    n_rows_ = sum(results_.sizes);

  n_cols_ = labels_size() + output_size();
}

int RowsFormatter::output_size() {
  switch (results_.type) {
  case nulls:
  case scalars:
    return 1;
    break;
  case vectors:
    return 1 + should_include_rowid_column();
    break;
  case dataframes:
    return Rf_length(results_.get()[0]) + should_include_rowid_column();
    break;
  default:
    return -1;
  }
}

int ColsFormatter::output_size() {
  switch (results_.type) {
  case nulls:
  case scalars:
    return 1;
    break;
  case vectors:
    return results_.first_size;
    break;
  case dataframes:
    return results_.first_size * Rf_length(results_.get()[0]);
    break;
  default:
    return -1;
    break;
  }
}

int ListFormatter::output_size() {
  return 1;
}

List& Formatter::add_labels(List& out) {
  if (labels_size() > 0) {
    Rcpp::IntegerVector sizes = results_.sizes;
    int n_labels = labels_.slicing_cols.size();

    for (int i = 0; i < n_labels; ++i) {
      RObject label = labels_.get()[i];
      switch (sexp_type(label)) {
      case LGLSXP:
      case INTSXP:
      case REALSXP:
      case STRSXP:
      case CPLXSXP:
      case RAWSXP:
      case VECSXP:
        out[i] = rep_each_n(label, sizes);
        Rf_copyMostAttrib(label, out[i]);
        break;
      default: { stop("internal error: unhandled vector type in REP"); }
      }
    }
  }

  return out;
}

RObject Formatter::create_column(SEXPTYPE sexp_type) {
  if (sexp_type == NILSXP)
    return R_NilValue;

  // Copy results' list contents to a common vector.
  // Handles all vectors, including scalar and ragged.
  RObject output_col(Rf_allocVector(sexp_type, n_rows_));
  for (int i = 0, counter = 0; i != results_.n_slices; ++i) {
    copy_elements(get_vector_elt(results_.get(), i), 0, output_col, counter);
    counter += results_.sizes[i];
  }
  return output_col;
}

List& Formatter::maybe_create_rowid_column(List& out) {
  if (should_include_rowid_column()) {
    IntegerVector index = seq_each_n(results_.sizes);
    out[labels_size()] = index;
  }
  return out;
}

List& ListFormatter::add_output(List& out) {
  out[labels_size()] = results_.get();
  return out;
}

List& RowsFormatter::rows_bind_vectors(List& out) {
  out = maybe_create_rowid_column(out);
  int index = labels_size() + should_include_rowid_column();
  out[index] = create_column(results_.first_sexp_type);
  return out;
}

List& RowsFormatter::rows_bind_dataframes(List& out) {
  out = maybe_create_rowid_column(out);
  int offset = labels_size() + should_include_rowid_column();

  // Fill in each column
  for (int col = 0; col < (n_cols_ - offset); ++col) {
    int type = TYPEOF(get_ij_elt(results_.get(), col, 0));
    RObject vec(Rf_allocVector(type, n_rows_));
    for (int s = 0, counter = 0; s < results_.size(); ++s) {
      copy_elements(get_ij_elt(results_.get(), col, s), 0, vec, counter);
      counter += results_.sizes[s];
    }
    out[col + offset] = vec;
  }

  return out;
}

List& RowsFormatter::add_output(List& out) {
  switch (results_.type) {
  case nulls:
  case scalars:
    out[labels_size()] = create_column(results_.first_sexp_type);
    break;
  case vectors:
    out = rows_bind_vectors(out);
    break;
  case dataframes:
    out = rows_bind_dataframes(out);
    break;
  default:
    break;
  }

  return out;
}

List& ColsFormatter::cols_bind_vectors(List& out) {
  for (int i = 0, counter = 0; i < results_.first_size; ++i) {
    RObject out_i(Rf_allocVector(results_.first_sexp_type, n_rows_));

    for (int s = 0; s < results_.size(); ++s) {
      copy_elements(results_.get()[s], i, out_i, counter, 1);
      counter += 1;
    }
    out[labels_size() + i] = out_i;
    counter = 0;
  }

  return out;
}

List& ColsFormatter::cols_bind_dataframes(List& out) {
  List first_result = results_.get()[0];
  int n_cols_results = first_result.size();
  int n_rows_results = Rf_length(first_result[0]);

  for (int col = 0, col_counter = 0; col < n_cols_results; ++col) {
    for (int row = 0, counter = 0; row < n_rows_results; ++row) {
      SEXPTYPE type = TYPEOF(get_vector_elt(first_result, col));
      RObject out_i(Rf_allocVector(type, n_rows_));

      for (int s = 0; s < results_.size(); ++s) {
        copy_elements(get_ij_elt(results_.get(), col, s), row, out_i, counter, 1);
        ++counter;
      }
      out[labels_size() + col_counter] = out_i;
      counter = 0;
      ++col_counter;
    }
  }

  return out;
}

List& ColsFormatter::add_output(List& out) {
  switch (results_.type) {
  case nulls:
  case scalars:
    out[labels_size()] = create_column(results_.first_sexp_type);
    break;
  case vectors:
    cols_bind_vectors(out);
    break;
  case dataframes:
    cols_bind_dataframes(out);
    break;
  default:
    break;
  }

  return out;
}

CharacterVector& RowsFormatter::add_rows_binded_vectors_colnames(CharacterVector& out_names) {
  int offset = labels_size();
  if (should_include_rowid_column()) {
    offset += 1;
    out_names[labels_size()] = ".row";
  }
  out_names[offset] = settings_.output_colname;

  return out_names;
}

CharacterVector& RowsFormatter::add_rows_binded_dataframes_colnames(CharacterVector& out_names) {
  int offset = labels_size();
  if (!labels_.are_unique) {
    offset += 1;
    out_names[labels_size()] = ".row";
  }

  List first_result = results_.get()[0];
  CharacterVector first_colnames = first_result.names();
  std::copy(first_colnames.begin(), first_colnames.end(), out_names.begin() + offset);

  return out_names;
}


List& Formatter::add_colnames(List& out) {
  CharacterVector out_names = no_init(n_cols_);
  if (labels_size() > 0) {
    CharacterVector slicing_cols_names = labels_.slicing_cols.names();
    std::copy(slicing_cols_names.begin(), slicing_cols_names.end(), out_names.begin());
  }
  out.names() = create_colnames(out_names);

  return out;
}

CharacterVector& RowsFormatter::create_colnames(CharacterVector& out_names) {
  switch (results_.type) {
  case nulls:
  case scalars:
    out_names[labels_size()] = settings_.output_colname;
    break;
  case vectors:
    out_names = add_rows_binded_vectors_colnames(out_names);
    break;
  case dataframes:
    out_names = add_rows_binded_dataframes_colnames(out_names);
    break;
  default:
    break;
  }

  return out_names;
}

CharacterVector& ColsFormatter::add_cols_binded_vectors_colnames(CharacterVector& out_names) {
  for (int i = 0; i < results_.first_size; ++i) {
    out_names[labels_size() + i] =
      settings_.output_colname + std::to_string(i + 1);
  }

  return out_names;
}

CharacterVector& ColsFormatter::add_cols_binded_dataframes_colnames(CharacterVector& out_names) {
  List first_result = results_.get()[0];
  int n_cols_results = first_result.size();
  int n_rows_results = Rf_length(first_result[0]);
  CharacterVector names(first_result.names());

  for (int col = 0, counter = 0; col < n_cols_results; ++col) {
    for (int row = 0; row < n_rows_results; ++row) {
      out_names[labels_size() + counter] =
        (std::string) names[col] + std::to_string(row + 1);
      ++counter;
    }
  }

  return out_names;
}

CharacterVector& ColsFormatter::create_colnames(CharacterVector& out_names) {
  std::string& output_colname = settings_.output_colname;

  switch (results_.type) {
  case nulls:
  case scalars:
    out_names[labels_size()] = output_colname;
    break;
  case vectors:
    out_names = add_cols_binded_vectors_colnames(out_names);
    break;
  case dataframes:
    out_names = add_cols_binded_dataframes_colnames(out_names);
    break;
  default:
    break;
  }

  return out_names;
}

CharacterVector& ListFormatter::create_colnames(CharacterVector& out_names) {
  out_names[labels_size()] = settings_.output_colname;
  return out_names;
}

List Formatter::output() {
  determine_dimensions();
  List out = no_init(n_cols_);

  out = add_output(out);
  out = add_labels(out);
  out = add_colnames(out);

  return as_data_frame(out);
}


} // namespace rows
