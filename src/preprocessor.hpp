#pragma once

#define PP_EMPTY()
#define PP_COMMA() ,

// PP_CAT
#define PP_CAT_I(a, b) a ## b
#define PP_CAT(a, b) PP_CAT_I(a, b)

// PP_BOOL
#define PP_BOOL_0  0
#define PP_BOOL_1  1
#define PP_BOOL_2  1
#define PP_BOOL_3  1
#define PP_BOOL_4  1
#define PP_BOOL_5  1
#define PP_BOOL_6  1
#define PP_BOOL_7  1
#define PP_BOOL_8  1
#define PP_BOOL_9  1
#define PP_BOOL_10 1
#define PP_BOOL_11 1
#define PP_BOOL_12 1
#define PP_BOOL_13 1
#define PP_BOOL_14 1
#define PP_BOOL_15 1

#define PP_BOOL_I(x) PP_BOOL_ ## x

#define PP_BOOL(x) PP_BOOL_I(x)

// PP_IF
#define PP_IF_0(t, f) f
#define PP_IF_1(t, f) t

#define PP_IF_I(x, t, f) PP_CAT(PP_IF_, x)(t, f)
#define PP_IF(x, t, f) PP_IF_I(PP_BOOL(x), t, f)

#define PP_COMMA_IF(x) PP_IF(x, PP_COMMA, PP_EMPTY)()

// PP_REPEAT
#define PP_REPEAT_0(m)
#define PP_REPEAT_1(m)  PP_REPEAT_0(m)  m(0)
#define PP_REPEAT_2(m)  PP_REPEAT_1(m)  m(1)
#define PP_REPEAT_3(m)  PP_REPEAT_2(m)  m(2)
#define PP_REPEAT_4(m)  PP_REPEAT_3(m)  m(3)
#define PP_REPEAT_5(m)  PP_REPEAT_4(m)  m(4)
#define PP_REPEAT_6(m)  PP_REPEAT_5(m)  m(5)
#define PP_REPEAT_7(m)  PP_REPEAT_6(m)  m(6)
#define PP_REPEAT_8(m)  PP_REPEAT_7(m)  m(7)
#define PP_REPEAT_9(m)  PP_REPEAT_8(m)  m(8)
#define PP_REPEAT_10(m) PP_REPEAT_9(m)  m(9)
#define PP_REPEAT_11(m) PP_REPEAT_10(m) m(10)
#define PP_REPEAT_12(m) PP_REPEAT_11(m) m(11)
#define PP_REPEAT_13(m) PP_REPEAT_12(m) m(12)
#define PP_REPEAT_14(m) PP_REPEAT_13(m) m(13)
#define PP_REPEAT_15(m) PP_REPEAT_14(m) m(14)
// m(0, d) m(1, d) ... macro(n-1, d)
#define PP_REPEAT_I(n, m) PP_REPEAT_ ## n(m)
#define PP_REPEAT(n, m) PP_REPEAT_I(n, m)

