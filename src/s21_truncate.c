#include "s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int sign = 0, degree = 0;
  if (result != NULL) {
    sign = get_bit(value, S21_SIGN_INDEX);
    degree = get_degree(value);
    for (int i = 0; i < degree; i++) {
      divide_by_ten(&value);
    }
    *result = value;
    set_sign(result, sign);
    set_degree(result, 0);
  }

  return 0;
}