#include "s21_decimal.h"

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int code = 0, degree = 0, sign_res = 0;
  make_zero_decimal(result);
  if (get_sign(value_1) && get_sign(value_2)) {
    s21_decimal temp1 = value_1;
    value_1 = value_2;
    value_2 = temp1;
    set_bit(&value_1, S21_SIGN_INDEX, 0);
    set_bit(&value_2, S21_SIGN_INDEX, 0);
  }
  if (get_sign(value_1) != get_sign(value_2)) {
    get_sign(value_1) ? sign_res = 1 : 1;
    set_bit(&value_1, S21_SIGN_INDEX, 0);
    set_bit(&value_2, S21_SIGN_INDEX, 0);
    code = s21_add(value_1, value_2, result);
  } else {
    s21_big_decimal val_1 = {0}, val_2 = {0}, res = {0};
    from_decimal_to_big_decimal(value_1, &val_1);
    from_decimal_to_big_decimal(value_2, &val_2);
    val_1.bits[5] = 0;
    val_2.bits[5] = 0;
    int diff = get_degree(value_1) - get_degree(value_2);
    diff > 0 ? set_degree(&value_2, get_degree(value_2) + diff)
             : set_degree(&value_1, get_degree(value_1) - diff);
    degree_aligment(&val_1, &val_2, diff);
    if (is_greater_big_decimal(val_2, val_1)) {
      s21_big_decimal temp2 = val_1;
      val_1 = val_2;
      val_2 = temp2;
      set_sign(result, 1);
    }
    simple_sub(val_1, val_2, &res);
    degree = div_degree(&res, get_degree(value_1));
    if (degree >= 0) {
      for (int i = 0; i < 3; i++) result->bits[i] = res.bits[i];
      set_degree(result, degree);
    } else {
      code = arithmetic_BIG;
    }
  }
  sign_res == 1 ? set_sign(result, 1) : 0;
  if (code == arithmetic_BIG && get_sign(*result)) code = arithmetic_SMALL;
  if (code) make_zero_decimal(result);
  return code;
}

int simple_sub(s21_big_decimal value_1, s21_big_decimal value_2,
               s21_big_decimal *result) {
  s21_big_decimal one;
  int last_ind = 0;
  make_zero_big_decimal(&one);
  one.bits[0] = 1;
  if (is_big_simple_less(value_1, value_2)) {
    value_1 = big_binary_not(value_1);
    last_ind = get_big_last_bit_index(value_2);
    simple_add(value_1, one, &value_1);
  } else {
    value_2 = big_binary_not(value_2);
    last_ind = get_big_last_bit_index(value_1);
    simple_add(value_2, one, &value_2);
  }
  simple_add_for_sub(value_2, value_1, result);
  set_big_bit(result, last_ind + 1, 0);
  return 0;
}

int simple_add_for_sub(s21_big_decimal value_1, s21_big_decimal value_2,
                       s21_big_decimal *result) {
  s21_big_decimal cpy_res;
  int tmp = 0;
  make_zero_big_decimal(&cpy_res);
  for (int index = 0; index <= S21_MANTISSA_LAST_INDEX + 5; index++) {
    int sum = 0;
    sum = get_big_bit(value_1, index) + get_big_bit(value_2, index) + tmp;
    set_big_bit(&cpy_res, index, sum % 2);
    tmp = sum / 2;
  }
  for (int i = 0; i < 6; i++) result->bits[i] = cpy_res.bits[i];
  return 0;
}

int is_big_simple_less(s21_big_decimal value_1, s21_big_decimal value_2) {
  int flag = 0;
  s21_comparison_code code = 0;
  for (int index = S21_BIG_MANTISSA_LAST_INDEX; index >= 0 && flag == 0;
       index--) {
    int b1 = 0, b2 = 0;
    b1 = get_big_bit(value_1, index);
    b2 = get_big_bit(value_2, index);
    switch (b1 - b2) {
      case 1:
        code = comparison_FALSE;
        flag = 1;
        break;
      case -1:
        code = comparison_TRUE;
        flag = 1;
        break;
      default:
        break;
    }
  }
  return code;
}