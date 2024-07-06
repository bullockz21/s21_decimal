#include "s21_decimal.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  s21_comparison_code code = comparison_TRUE;
  if (check_empty(value_1) && check_empty(value_2)) {
    code = comparison_TRUE;
  } else {
    s21_decimal cpy_value_1, cpy_value_2, tmp;
    make_zero_decimal(&cpy_value_1);
    make_zero_decimal(&cpy_value_2);
    int sign_value_1 = 0, sign_value_2 = 0;
    sign_value_1 = get_bit(value_1, S21_SIGN_INDEX);
    sign_value_2 = get_bit(value_2, S21_SIGN_INDEX);
    if (sign_value_1 != sign_value_2) {
      code = comparison_FALSE;
    } else {
      int degree_value_1 = 0, degree_value_2 = 0;
      degree_value_1 = get_degree(value_1);
      degree_value_2 = get_degree(value_2);
      if (degree_value_1 != degree_value_2)
        s21_degree_alignment(&value_1, &value_2, &tmp);
      for (int index = 0; index < 3 && code == comparison_TRUE; index++) {
        if (value_1.bits[index] != value_2.bits[index]) {
          code = comparison_FALSE;
        }
      }
    }
  }
  return code;
}