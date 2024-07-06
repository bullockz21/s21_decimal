#include "s21_decimal.h"

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  s21_comparison_code code = comparison_FALSE;
  s21_decimal zero;
  make_zero_decimal(&zero);
  if (check_empty(value_1) && check_empty(value_2)) {
    code = comparison_FALSE;
  } else {
    code = is_simple_less(value_1, value_2);
  }
  return code;
}

int is_simple_less(s21_decimal value_1, s21_decimal value_2) {
  int flag_stop = 0;
  s21_comparison_code code = 0;
  s21_big_decimal val_1, val_2, tmp;
  from_decimal_to_big_decimal(value_1, &val_1);
  from_decimal_to_big_decimal(value_2, &val_2);
  make_zero_big_decimal(&tmp);
  int sign_value_1 = 0, sign_value_2 = 0;
  sign_value_1 = get_sign(value_1);
  sign_value_2 = get_sign(value_2);
  big_degree_alignment(&val_1, &val_2, &tmp);
  make_zero_big_decimal(&tmp);
  int check = s21_is_equal(value_1, value_2);
  if (check) {
    code = comparison_FALSE;
  } else {
    if ((sign_value_1 - sign_value_2) == -1) {
      code = comparison_FALSE;
    } else if ((sign_value_1 - sign_value_2) == 1) {
      code = comparison_TRUE;
    } else {
      for (int i = S21_BIG_MANTISSA_LAST_INDEX; i >= 0 && flag_stop == 0; i--) {
        flag_stop = get_big_bit(val_2, i) - get_big_bit(val_1, i);
      }
      code = (flag_stop == 1) ? comparison_TRUE : comparison_FALSE;
      if (sign_value_1 == 1) {
        if (code == comparison_TRUE)
          code = comparison_FALSE;
        else
          code = comparison_TRUE;
      }
    }
  }
  return code;
}