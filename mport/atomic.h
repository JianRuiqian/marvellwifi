#ifndef _MPORT_ATOMIC_H_
#define _MPORT_ATOMIC_H_

#include <rtthread.h>
#include "type.h"

#define atomic_read(v)	((v)->counter)
#define atomic_set(v,i)	(((v)->counter) = (i))

#define ATOMIC_OP(op, c_op, asm_op)					\
rt_inline void atomic_##op(int i, atomic_t *v)			\
{					\
	rt_enter_critical();					\
	v->counter c_op i;						\
	rt_exit_critical();					\
}									\

#define ATOMIC_OP_RETURN(op, c_op, asm_op)				\
rt_inline int atomic_##op##_return(int i, atomic_t *v)		\
{						\
	int val;							\
									\
	rt_enter_critical();					\
	v->counter c_op i;						\
	val = v->counter;						\
	rt_exit_critical();					\
									\
	return val;							\
}

#define ATOMIC_FETCH_OP(op, c_op, asm_op)				\
rt_inline int atomic_fetch_##op(int i, atomic_t *v)			\
{						\
	int val;							\
									\
	rt_enter_critical();					\
	val = v->counter;						\
	v->counter c_op i;						\
	rt_exit_critical();					\
									\
	return val;							\
}

#define ATOMIC_OPS(op, c_op, asm_op)					\
	ATOMIC_OP(op, c_op, asm_op)					\
	ATOMIC_OP_RETURN(op, c_op, asm_op)				\
	ATOMIC_FETCH_OP(op, c_op, asm_op)

ATOMIC_OPS(add, +=, add)
ATOMIC_OPS(sub, -=, sub)

#undef ATOMIC_OPS
#define ATOMIC_OPS(op, c_op, asm_op)					\
	ATOMIC_OP(op, c_op, asm_op)					\
	ATOMIC_FETCH_OP(op, c_op, asm_op)

ATOMIC_OPS(and, &=, and)
ATOMIC_OPS(andnot, &= ~, bic)
ATOMIC_OPS(or,  |=, orr)
ATOMIC_OPS(xor, ^=, eor)

#undef ATOMIC_OPS
#undef ATOMIC_FETCH_OP
#undef ATOMIC_OP_RETURN
#undef ATOMIC_OP

#define atomic_inc(v)			atomic_add(1, v)
#define atomic_dec(v)			atomic_sub(1, v)

#define atomic_inc_and_test(v)	(atomic_add_return(1, v) == 0)
#define atomic_dec_and_test(v)	(atomic_sub_return(1, v) == 0)
#define atomic_inc_return(v)	atomic_add_return(1, v)
#define atomic_dec_return(v)	atomic_sub_return(1, v)
#define atomic_sub_and_test(i, v)	(atomic_sub_return(i, v) == 0)

#define atomic_add_negative(i,v)	(atomic_add_return(i, v) < 0)

#endif  /* _MPORT_ATOMIC_H_ */
