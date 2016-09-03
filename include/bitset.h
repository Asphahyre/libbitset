#ifndef BITSET_H_
# define BITSET_H_

# include <stdlib.h>

# ifdef NDEBUG
/* Include stdio, needed to define printf calls in printf_bitset(). */
#  include <stdio.h>
# endif /* !NDEBUG */

typedef __uint128_t _word_t;
typedef unsigned int u_int;
typedef _word_t * bitset;
typedef enum bool bool;

enum bool {
	false = 0,
	true = 1
};

# define _WORD_NBYTES               (sizeof(_word_t))
# define _WORD_SIZE                 (_WORD_NBYTES * 8)
# define _B_INDEX(index)            ((index) / _WORD_SIZE)
# define _B_OFFSET(index)           ((index) % _WORD_SIZE)

# define _MIN(A, B)                 (((A) < (B)) ? A : B)
# define _MAX(A, B)                 (((A) > (B)) ? A : B)
# define _SWAP(A, B)                ({ __auto_type _t = A; A = B; B = _t; })

# define _SET_INDEX(set, index)     ((set)[(index)] = ~((_word_t) 0))
# define _RESET_INDEX(set, index)   ((set)[(index)] = 0)

# define SET_BIT_F(set, index)  ((set)[_B_INDEX(index)] &= ~((_word_t) 1 << _B_OFFSET(index)))
# define SET_BIT_T(set, index)  ((set)[_B_INDEX(index)] |= (_word_t) 1 << _B_OFFSET(index))
# define SET_BIT(set, index, state) ({if (state) SET_BIT_T(set, index) else SET_BIT_F(set, intex)

# define GET_BIT(set, index)        ((bool) !!((set)[_B_INDEX(index)] & ((_word_t) 1 << _B_OFFSET(index))))

# define BITSET_SIZE(set)           ((size_t) *(set - 1))

# define del_bitset(set) ({ free(set - 1); })

bitset Bitset(u_int);
bitset bitwise_not(bitset);
bitset copy_bitset(bitset);
bool compare_bitsets(bitset, bitset);
void resize_bitset(bitset, u_int);
void set_subset(bitset, u_int, u_int, bool);
void shift_bitset(bitset, int);
void print_bitset(bitset);

#endif /* !BITSET_H_ */
