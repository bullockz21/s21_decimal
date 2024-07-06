#include "s21_decimal.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_arithmetic_error_code code = arithmetic_OK;
  if (correct_decimal(value_2) == 1 || !result || check_empty(value_2)) {
    code = arithmetic_ZERO;
    make_zero_decimal(result);
  } else if (check_empty(value_1))
    make_zero_decimal(result);
  else {
    make_zero_decimal(result);
    int whileStop = 0;
    int degree_1 = get_degree(value_1), degree_2 = get_degree(value_2);
    int sign =
        get_bit(value_1, S21_SIGN_INDEX) + get_bit(value_2, S21_SIGN_INDEX);
    s21_decimal temp = {{0, 0, 0, 0}};
    s21_decimal temp1 = value_1, temp2 = value_2;
    while (mult_ten(temp1, &temp1) == 0) degree_1 += 1;
    while (s21_is_less(value_1, value_2) == 1 && whileStop == 0) {
      if (div_ten(temp2, &temp2) == 0)
        degree_2 -= 1;
      else
        whileStop = 1;
    }
    div_simple(temp1, temp2, &temp);
    int delta = degree_1 - degree_2;
    while (delta < 0 && code == arithmetic_OK) {
      if (mult_ten(temp, &temp) == 0)
        delta += 1;
      else
        code = arithmetic_BIG;
    }
    if (delta >= 0 && code == arithmetic_OK) {
      while (delta > 29 && code == arithmetic_OK) {
        if (div_ten(temp, &temp) == 0)
          delta = delta - 1;
        else
          code = arithmetic_SMALL;
      }
      if (delta <= 28 && code == arithmetic_OK && result) {
        write_in_dec_parts(temp, delta, sign, result);
        remove_zeros(result);
      }
    }
  }
  return code;
}

int move_left(s21_decimal *num) {
  int lastBit = get_bit(*num, 95);
  if (!lastBit) {
    int highBit = 0;
    for (int i = 2; i > 0; i--) {
      highBit = get_bit(*num, (i - 1) * 32 + 31);
      num->bits[i] <<= 1;
      set_bit(num, i * 32, highBit);
    }
    num->bits[0] <<= 1;
  }
  return lastBit;
}

int div_ten(s21_decimal num, s21_decimal *result) {
  int code = 1;
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal temp = {{0, 0, 0, 0}};
  if (!div_simple(num, ten, &temp)) {
    *result = temp;
    code = 0;
  }
  return code;
}

int div_simple(s21_decimal num1, s21_decimal num2, s21_decimal *result) {
  int ret = 0;
  s21_decimal div = {{0}};
  s21_decimal temp = {{0, 0, 0, 0}};
  s21_decimal mid = num1;
  int max1 = get_last_bit_index(num1), maxBitPos = 0;
  int zero = s21_is_equal(num2, div);
  if (s21_is_greater_or_equal(num1, num2) && zero == 0) {
    for (int i = max1; i >= 0; i--) {
      temp = mid;
      move_right_n_times(&temp, i);
      if (s21_is_greater_or_equal(temp, num2)) {
        minus_simple(temp, num2, &temp);
        maxBitPos = get_last_bit_index(temp);
        for (int j = 0; j <= maxBitPos; j++)
          set_bit(&mid, j + i, get_bit(temp, j));
        for (int j = i + maxBitPos + 1; j <= max1; j++) set_bit(&mid, j, 0);
        set_bit(&div, i, 1);
      } else
        set_bit(&div, i, 0);
    }
  } else
    ret = 2;
  if (zero == 1) ret = 1;
  if (ret == 0) *result = div;
  return ret;
}

void move_right_n_times(s21_decimal *num, int n) {
  int firstBit;
  for (int j = 0; j < n; j++) {
    for (int i = 0; i < 2; i++) {
      firstBit = get_bit(*num, (i + 1) * 32);
      num->bits[i] >>= 1;
      set_bit(num, i * 32 + 31, firstBit);
    }
    num->bits[2] >>= 1;
  }
}

void write_in_dec_parts(s21_decimal mantissa, int exp, int sign,
                        s21_decimal *result) {
  if (result) {
    s21_decimal res = {0};
    res.bits[0] = mantissa.bits[0];
    res.bits[1] = mantissa.bits[1];
    res.bits[2] = mantissa.bits[2];
    increase_exp_by_n(&res, exp);
    set_bit(&res, S21_SIGN_INDEX, sign);
    *result = res;
  }
}

void increase_exp_by_n(s21_decimal *num, int n) {
  int add = n * (1 << 16);
  num->bits[3] += add;
}

int find_max_bit(s21_decimal num) {
  int max = 0, stop = 0;
  for (int i = 95; i >= 0 && !stop; i--) {
    if (get_bit(num, i) == 1) {
      max = i;
      stop = 1;
    }
  }
  return max;
}

void remove_zeros(s21_decimal *num) {
  s21_decimal temp = *num, mid = {0};
  int lastDigit = get_smallest_rank_digit(temp), stop = 0,
      exp = get_degree(temp), memexp = 0;
  int sign = get_bit(*num, S21_SIGN_INDEX);
  memexp = exp;
  while (stop == 0 && exp > 0) {
    mid = temp;
    lastDigit = get_smallest_rank_digit(temp);
    if (div_ten(temp, &temp) == 1)
      stop = 1;
    else {
      mult_ten(temp, &temp);
      if (s21_is_equal(temp, mid) == 0) {
        stop = 1;
        temp.bits[0] += lastDigit;
      } else {
        div_ten(temp, &temp);
        exp -= 1;
      }
    }
  }
  int delta = memexp - exp;
  if (delta) {
    for (int i = 0; i < (memexp - exp); i++) {
      div_ten(*num, num);
    }
    num->bits[3] = 0;
    set_bit(num, 127, sign);
    increase_exp_by_n(num, exp);
  }
}

int get_smallest_rank_digit(s21_decimal num) {
  s21_decimal temp = num;
  div_ten(temp, &temp);
  mult_ten(temp, &temp);
  minus_simple(num, temp, &temp);
  return temp.bits[0];
}

int minus_simple(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal *result) {
  int ret = 0, next = 0, maxBitWithOne = 0, s = 0;
  s21_decimal temp = {{0}}, t1 = {{0}}, t2 = {{0}};
  if (s21_is_less(value_1, value_2) == 1) {
    t1 = value_2;
    t2 = value_1;
  } else {
    t1 = value_1;
    t2 = value_2;
  }
  maxBitWithOne = find_max_bit(t1);
  for (int i = 0; i < maxBitWithOne + 1; i++) {
    s = get_bit(t1, i) - get_bit(t2, i) - next;
    if (s < 0) {
      next = 1;
      set_bit(&temp, i, s + 2);
    } else {
      set_bit(&temp, i, s);
      next = 0;
    }
  }
  *result = temp;
  return ret;
}