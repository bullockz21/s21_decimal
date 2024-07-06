#include "s21_decimal.h"

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  s21_comparison_code code = comparison_TRUE;
  code = !s21_is_less_or_equal(value_1, value_2);
  return code;
}