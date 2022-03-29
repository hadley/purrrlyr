#ifndef UTILS_H
#define UTILS_H

SEXP shadow_call(const SEXP fun, SEXP arg, SEXP dots, const SEXP env = R_NilValue);
SEXP as_data_frame(SEXP x);
int is_atomic(const SEXP x);
int is_atomic(int x);
int is_function(const SEXP fun);
int is_function(int fun);
SEXP get_ij_elt(const SEXP slice, int i, int j);
int first_type(const Rcpp::List& results);
int sexp_type(const SEXP x);
void check_dataframes_consistency(const Rcpp::List x);
void check_dataframes_names_consistency(const Rcpp::List& x);
void check_dataframes_types_consistency(const Rcpp::List& x);

// Predicates for iterator algorithms
struct is_non_null : std::function<bool(SEXP)> {
  bool operator()(const SEXP x) {return !Rf_isNull(x);}
};

struct is_empty : std::function<bool(SEXP)> {
  bool operator()(const SEXP x) {return Rf_length(x) == 0;}
};

#endif
