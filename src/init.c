#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* .Call calls */
extern SEXP map_impl(SEXP, SEXP, SEXP, SEXP);
extern SEXP by_slice_impl(SEXP, SEXP, SEXP);
extern SEXP map_by_slice_impl(SEXP, SEXP, SEXP, SEXP);
extern SEXP invoke_rows_impl(SEXP, SEXP, SEXP);

static const R_CallMethodDef CallEntries[] = {
  {"map_impl", (DL_FUNC) &map_impl, 4},
  {"by_slice_impl", (DL_FUNC) &by_slice_impl, 3},
  {"map_by_slice_impl", (DL_FUNC) &map_by_slice_impl, 4},
  {"invoke_rows_impl", (DL_FUNC) &invoke_rows_impl, 3},
  {NULL, NULL, 0}
};

void R_init_purrrlyr(DllInfo *dll) {
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
  R_forceSymbols(dll, TRUE);
}
