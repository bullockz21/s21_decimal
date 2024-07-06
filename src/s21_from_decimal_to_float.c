#include "s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  s21_other_error_code code = other_OK;
  *dst = 0;
  double result = 0;
  int degree = get_degree(src), last_ind = 0;
  for (int index = S21_MANTISSA_LAST_INDEX; index >= 0 && last_ind == 0;
       index--) {
    if (get_bit(src, index)) last_ind = index;
  }
  for (int i = 0; i <= last_ind; i++) result += get_bit(src, i) * pow(2, i);
  for (int i = 0; i < degree; i++) result /= 10.0;
  *dst = (float)result;
  if (get_bit(src, S21_SIGN_INDEX)) *dst *= -1;
  return code;
}