#include "s21_decimal.h"

int get_bit(s21_decimal value, int index) {
  return ((value.bits[index / 32] >> (index % 32)) & 1u);
}

int get_big_bit(s21_big_decimal value, int index) {
  return ((value.bits[index / 32] >> (index % 32)) & 1u);
}

void set_bit(s21_decimal *value, int index, int bit) {
  int mask = 0b1 << (index % 32);
  if (bit == 1) {
    value->bits[index / 32] |= mask;
  } else {
    value->bits[index / 32] &= ~mask;
  }
}

void set_big_bit(s21_big_decimal *value, int index, int bit) {
  int mask = 0b1 << (index % 32);
  if (bit == 1) {
    value->bits[index / 32] |= mask;
  } else {
    value->bits[index / 32] &= ~mask;
  }
}

int check_empty(s21_decimal value) {
  return !value.bits[0] && !value.bits[1] && !value.bits[2];
}

void make_zero_decimal(s21_decimal *value) {
  for (int i = 0; i < 4; i++) value->bits[i] = 0;
}

void make_zero_big_decimal(s21_big_decimal *value) {
  for (int i = 0; i < 6; i++) value->bits[i] = 0;
}

int get_degree(s21_decimal value) {
  int degree = 0;
  for (int i = 16; i <= 23; i++) {
    degree += ((value.bits[3] >> (i)) & 1u) * pow(2, i - 16);
  }
  return degree;
}

int get_big_degree(s21_big_decimal value) {
  int degree = 0;
  for (int i = 16; i <= 23; i++) {
    degree += ((value.bits[5] >> (i)) & 1u) * pow(2, i - 16);
  }
  return degree;
}

s21_decimal shift_left_decimal(s21_decimal value, int length) {
  while (length > 0) {
    for (int i = 2; i >= 0; i--) {
      int flag = 0;
      if (get_bit(value, i * 32 - 1)) flag = 1;
      value.bits[i] <<= 1;
      if (flag == 1) set_bit(&value, i * 32, 1);
    }
    length--;
  }
  return value;
}

s21_big_decimal shift_left_big_decimal(s21_big_decimal value, int length,
                                       s21_arithmetic_error_code *code) {
  s21_big_decimal copy;
  for (int i = 0; i < 6; i++) copy.bits[i] = value.bits[i];
  int buffer[8] = {0};
  while (length > 0) {
    if (get_big_bit(copy, S21_BIG_MANTISSA_LAST_INDEX) != 1) {
      for (int i = 0; i < 5; i++) {
        buffer[i] = get_big_bit(copy, (i + 1) * 32 - 1);
      }
      for (int i = 4; i > 0; i--) {
        copy.bits[i] <<= 1;
        set_big_bit(&copy, i * 32, buffer[i - 1]);
      }
      copy.bits[0] <<= 1;
    } else {
      *code = arithmetic_BIG;
    }
    length--;
  }
  return copy;
}

int from_big_decimal_to_decimal(s21_big_decimal src, s21_decimal *dst) {
  s21_arithmetic_error_code code = arithmetic_OK;
  int degree = get_big_degree(src);
  int sign = get_big_bit(src, S21_BIG_SIGN_INDEX);
  set_big_degree(&src, 0);
  while ((src.bits[4] || src.bits[3]) && (degree > 0)) {
    divide_big_by_ten(&src);
    degree--;
  }
  if (src.bits[4] || src.bits[3]) {
    if (sign == 1)
      code = arithmetic_SMALL;
    else {
      code = arithmetic_BIG;
    }
    make_zero_decimal(dst);
  } else {
    set_big_degree(&src, degree);
    for (int i = 0; i < 4; i++) dst->bits[i] = src.bits[i];
    set_degree(dst, degree);
    set_sign(dst, sign);
  }
  return code;
}

int div_degree(s21_big_decimal *src, int degree) {
  int tmp = 0;
  while ((src->bits[3] || src->bits[4] || src->bits[5]) && degree > 0) {
    if (degree == 1 && src->bits[3]) tmp = 1;
    divide_big_by_ten(src);
    degree--;
  }
  if (tmp && degree == 0 && src->bits[3] == 0 && get_big_bit(*src, 0))
    set_big_bit(src, 0, 0);
  if (src->bits[3] || src->bits[4] || src->bits[5]) degree = -1;
  return degree;
}

int big_degree_alignment(s21_big_decimal *value_1, s21_big_decimal *value_2,
                         s21_big_decimal *result) {
  int sign_value_1 = 0, sign_value_2 = 0, degree_value_1 = 0,
      degree_value_2 = 0;
  s21_arithmetic_error_code code = arithmetic_OK;
  s21_decimal tmp;
  s21_big_decimal tmp1, copy;
  make_zero_decimal(&tmp);
  make_zero_big_decimal(&tmp1);
  degree_value_1 = get_big_degree(*value_1);
  degree_value_2 = get_big_degree(*value_2);
  sign_value_1 = get_big_bit(*value_1, S21_SIGN_INDEX);
  sign_value_2 = get_big_bit(*value_2, S21_SIGN_INDEX);
  s21_from_int_to_decimal(pow(10, abs(degree_value_1 - degree_value_2)), &tmp);
  from_decimal_to_big_decimal(tmp, &tmp1);
  if (degree_value_1 > degree_value_2) {
    for (int i = 0; i < 6; i++) copy.bits[i] = value_2->bits[i];
    code = simple_mul(*value_2, tmp1, &copy);
    set_big_degree(&copy, degree_value_1);
    set_big_bit(&copy, S21_SIGN_INDEX, sign_value_2);
    set_big_degree(result, degree_value_1);
    for (int i = 0; i < 6; i++) value_2->bits[i] = copy.bits[i];
  } else {
    for (int i = 0; i < 6; i++) copy.bits[i] = value_1->bits[i];
    code = simple_mul(*value_1, tmp1, &copy);
    set_big_degree(&copy, degree_value_2);
    set_big_bit(&copy, S21_SIGN_INDEX, sign_value_1);
    set_big_degree(result, degree_value_2);
    for (int i = 0; i < 6; i++) copy.bits[i] = value_1->bits[i];
  }
  return code;
}

int s21_degree_alignment(s21_decimal *value_1, s21_decimal *value_2,
                         s21_decimal *result) {
  int sign_value_1 = 0, sign_value_2 = 0, degree_value_1 = 0,
      degree_value_2 = 0;
  s21_decimal tmp, copy;
  make_zero_decimal(&tmp);
  make_zero_decimal(&copy);
  degree_value_1 = get_degree(*value_1);
  degree_value_2 = get_degree(*value_2);
  sign_value_1 = get_bit(*value_1, S21_SIGN_INDEX);
  sign_value_2 = get_bit(*value_2, S21_SIGN_INDEX);
  s21_from_int_to_decimal(pow(10, abs(degree_value_1 - degree_value_2)), &tmp);
  if (degree_value_1 > degree_value_2) {
    for (int i = 0; i < 4; i++) copy.bits[i] = value_2->bits[i];
    old_mul(*value_2, tmp, &copy);
    set_degree(&copy, degree_value_1);
    set_sign(&copy, sign_value_2);
    set_degree(result, degree_value_1);
    for (int i = 0; i < 4; i++) value_2->bits[i] = copy.bits[i];
  } else {
    for (int i = 0; i < 4; i++) copy.bits[i] = value_1->bits[i];
    old_mul(*value_1, tmp, &copy);
    set_degree(&copy, degree_value_2);
    set_sign(&copy, sign_value_1);
    set_degree(result, degree_value_2);
    for (int i = 0; i < 4; i++) value_1->bits[i] = copy.bits[i];
  }
  return 0;
}

int degree_alignment(s21_decimal *value_1, s21_decimal *value_2,
                     s21_decimal *result) {
  int sign_value_1 = 0, sign_value_2 = 0, degree_value_1 = 0,
      degree_value_2 = 0;
  s21_decimal tmp, copy;
  make_zero_decimal(&tmp);
  make_zero_decimal(&copy);
  degree_value_1 = get_degree(*value_1);
  degree_value_2 = get_degree(*value_2);
  sign_value_1 = get_bit(*value_1, S21_SIGN_INDEX);
  sign_value_2 = get_bit(*value_2, S21_SIGN_INDEX);
  s21_from_int_to_decimal(pow(10, abs(degree_value_1 - degree_value_2)), &tmp);
  if (degree_value_1 > degree_value_2) {
    for (int i = 0; i < 4; i++) copy.bits[i] = value_2->bits[i];
    old_mul(*value_2, tmp, &copy);
    set_degree(&copy, degree_value_1);
    set_sign(&copy, sign_value_2);
    set_degree(result, degree_value_1);
    for (int i = 0; i < 4; i++) value_2->bits[i] = copy.bits[i];
  } else {
    for (int i = 0; i < 4; i++) copy.bits[i] = value_1->bits[i];
    old_mul(*value_1, tmp, &copy);
    set_degree(&copy, degree_value_2);
    set_sign(&copy, sign_value_1);
    set_degree(result, degree_value_2);
    for (int i = 0; i < 4; i++) value_1->bits[i] = copy.bits[i];
  }
  return 0;
}

void set_degree(s21_decimal *value, int degree) {
  int degree_argument[8] = {0};
  for (int i = 7; i >= 0; i--) {
    degree_argument[i] = degree % 2;
    degree /= 2;
  }
  for (int i = 0, index = 119; i < 8; i++, index--) {
    set_bit(value, index, degree_argument[i]);
  }
}

void set_big_degree(s21_big_decimal *value, int degree) {
  int degree_argument[8] = {0};
  for (int i = 7; i >= 0; i--) {
    degree_argument[i] = degree % 2;
    degree /= 2;
  }
  for (int i = 0, index = 183; i < 8; i++, index--) {
    set_big_bit(value, index, degree_argument[i]);
  }
}

s21_big_decimal big_binary_not(s21_big_decimal value) {
  s21_big_decimal result;
  make_zero_big_decimal(&result);
  for (int i = 0; i < 5; i++) result.bits[i] = ~value.bits[i];
  return result;
}

int get_last_bit_index(s21_decimal value) {
  int last_ind = 0;
  for (int index = S21_MANTISSA_LAST_INDEX; index >= 0 && last_ind == 0;
       index--) {
    if (get_bit(value, index)) last_ind = index;
  }
  return last_ind;
}

int get_big_last_bit_index(s21_big_decimal value) {
  int last_ind = 0;
  for (int index = S21_BIG_MANTISSA_LAST_INDEX; index >= 0 && last_ind == 0;
       index--) {
    if (get_big_bit(value, index)) last_ind = index;
  }
  return last_ind;
}

int divide_by_ten(s21_decimal *value) {
  int divider = 0, last_ind = 0;
  s21_decimal result;
  make_zero_decimal(&result);
  last_ind = get_last_bit_index(*value);
  for (int i = 0; i < last_ind + 2; i++) {
    if (divider >= 10) {
      result = shift_left_decimal(result, 1);
      set_bit(&result, 0, 1);
      divider -= 10;
    } else {
      result = shift_left_decimal(result, 1);
      set_bit(&result, 0, 0);
    }
    int new_bit = get_bit(*value, last_ind - i);
    divider <<= 1;
    divider += new_bit;
  }
  *value = result;
  return (divider >>= 1);
}

int divide_big_by_ten(s21_big_decimal *value) {
  int divider = 0, last_ind = 0;
  s21_arithmetic_error_code code = arithmetic_OK;
  s21_big_decimal result;
  make_zero_big_decimal(&result);
  last_ind = get_big_last_bit_index(*value);
  for (int i = 0; i < last_ind + 2 && code == arithmetic_OK; i++) {
    if (divider >= 10) {
      result = shift_left_big_decimal(result, 1, &code);
      set_big_bit(&result, 0, 1);
      divider -= 10;
    } else {
      result = shift_left_big_decimal(result, 1, &code);
      set_big_bit(&result, 0, 0);
    }
    int new_bit = get_big_bit(*value, last_ind - i);
    divider <<= 1;
    divider += new_bit;
  }
  *value = result;
  return (divider >>= 1);
}

void from_decimal_to_big_decimal(s21_decimal src, s21_big_decimal *dst) {
  if (dst != NULL) {
    dst->bits[0] = src.bits[0];
    dst->bits[1] = src.bits[1];
    dst->bits[2] = src.bits[2];
    dst->bits[3] = 0;
    dst->bits[4] = 0;
    dst->bits[5] = src.bits[3];
  }
}

void set_sign(s21_decimal *value, int sign) {
  set_bit(value, S21_SIGN_INDEX, sign);
}

int help_round(s21_decimal *value, int point) {
  s21_decimal result;
  make_zero_decimal(&result);
  if (point) {
    s21_truncate(*value, &result);
    if (result.bits[0] + 1u < result.bits[0]) {
    } else {
      result.bits[0] += 1u;
    }
  } else {
    s21_truncate(*value, &result);
  }
  for (int i = 0; i < 4; i++) value->bits[i] = result.bits[i];
  return 0;
}

int get_sign(s21_decimal value) { return get_bit(value, S21_SIGN_INDEX); }

int old_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_arithmetic_error_code code = arithmetic_OK;
  s21_big_decimal val_1, val_2, res;
  make_zero_big_decimal(&res);
  from_decimal_to_big_decimal(value_1, &val_1);
  from_decimal_to_big_decimal(value_2, &val_2);
  int res_sign = (get_sign(value_1) + get_sign(value_2)) % 2;
  code = simple_mul(val_1, val_2, &res);
  set_big_degree(&res, get_degree(value_1) + get_degree(value_2));
  set_big_bit(&res, S21_BIG_SIGN_INDEX, res_sign);
  make_zero_decimal(result);
  if (code == arithmetic_BIG && res_sign) {
    code = arithmetic_SMALL;
  }
  if (!code) code = from_big_decimal_to_decimal(res, result);
  return code;
}

void degree_aligment(s21_big_decimal *value_1, s21_big_decimal *value_2,
                     int degree) {
  if (degree > 0)
    increase_degree(value_2, degree);
  else if (degree < 0)
    increase_degree(value_1, -degree);
}

void increase_degree(s21_big_decimal *value, int degree) {
  s21_big_decimal ten = {{10, 0, 0, 0, 0, 0}}, tmp = {0};
  for (int i = 0; i < degree; i++) {
    simple_mul(*value, ten, &tmp);
    *value = tmp;
    make_zero_big_decimal(&tmp);
  }
}

int is_greater_big_decimal(s21_big_decimal value_1, s21_big_decimal value_2) {
  int result = 0, out = 0;
  for (int i = 5; i >= 0 && !result && !out; i--) {
    if (value_1.bits[i] || value_2.bits[i]) {
      if (value_1.bits[i] > value_2.bits[i]) {
        result = 1;
      }
      if (value_1.bits[i] != value_2.bits[i]) out = 1;
    }
  }
  return result;
}

int mult_ten(s21_decimal num, s21_decimal *result) {
  s21_arithmetic_error_code code = 0;
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal sum = {{0}};
  s21_decimal temp = {{0}};
  for (int i = 0; i < 4 && code == arithmetic_OK; i++) {
    temp = num;
    if (get_bit(ten, i) == 1) {
      for (int j = 0; j < i && code == arithmetic_OK; j++)
        code = move_left(&temp);
      if (code == arithmetic_OK) code = s21_add(temp, sum, &sum);
    }
  }
  if (code == arithmetic_OK) *result = sum;
  return code;
}

int correct_decimal(s21_decimal value) {
  int ret = 1;
  for (size_t i = 0; i < 3; i++) {
    if (value.bits[i] != 0) ret = 0;
  }
  return ret;
}