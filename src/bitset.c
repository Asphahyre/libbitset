#include "bitset.h"

bitset Bitset(u_int nbits)
{
	bitset set;

	set = ((bitset) calloc(_B_INDEX(nbits) + !!_B_OFFSET(nbits) + 1,
				sizeof(_word_t))) + 1;
	*(set - 1) = nbits;
	return set;
}

void resize_bitset(bitset set, u_int size)
{
	set = (bitset) realloc(set - 1, size + sizeof(_word_t)) + 1;
	set_subset(set, BITSET_SIZE(set), size, 0);
	*(set - 1) = size;
}

bitset copy_bitset(bitset original)
{
	size_t i;
	bitset copy;

	i = BITSET_SIZE(original);
	copy = Bitset(i);
	i = _B_INDEX(i) + !!_B_OFFSET(i);
	while (i--)
		copy[i] = original[i];
	return copy;
}

void set_subset(bitset set, u_int from, u_int to, bool state)
{
	if (to > (BITSET_SIZE(set) - 1))
		to = BITSET_SIZE(set);
	else
		++to;
	if (from < to) {
		if (_B_INDEX(from) == _B_INDEX(to)) {
			if (state)
				set[_B_INDEX(from)] |=
 					  ~(~ (_word_t) 0 << (_B_OFFSET(to) - _B_OFFSET(from))) << _B_OFFSET(from);
			else
				set[_B_INDEX(from)] &=
					~(~(~ (_word_t) 0 << (_B_OFFSET(to) - _B_OFFSET(from))) << _B_OFFSET(from));
		} else {
			u_int i = _B_INDEX(from);
			if (state) {
				set[_B_INDEX(from)] |= (~ (_word_t) 0 << _B_OFFSET(from));
				if (_B_OFFSET(to))
					set[_B_INDEX(to)] |= ~(~ (_word_t) 0 << _B_OFFSET(to));
				while (++i < _B_INDEX(to))
					_SET_INDEX(set, i);
			} else {
				set[_B_INDEX(from)] &=   ~(~ (_word_t) 0 << _B_OFFSET(from));
				set[_B_INDEX(to)]   &=    (~ (_word_t) 0 << _B_OFFSET(to));
				while (++i < _B_INDEX(to))
					_RESET_INDEX(set, i);
			}
		}
	}
}

void shift_bitset(bitset set, int width)
{
	size_t len = _B_INDEX(BITSET_SIZE(set)), i;
	_word_t shift[2] = {0, 0};

	if (width < 0) {
		width = -width;
		i = len;
		shift[1] = (set[i] & (~ (_word_t) 0 >> (_WORD_SIZE - _B_OFFSET(BITSET_SIZE(set))))) >> width;
		while (i--) {
			shift[0] = set[i];
			shift[1] |= shift[0] >> (_WORD_SIZE - width);
			set[i + 1] = shift[1];
			shift[1] = shift[0] >> width;
		}
		set[i + 1] = shift[1];
	} else if (width) {
		i = 0;
		do {
			shift[0] = set[i];
			shift[1] |= shift[0] << (_WORD_SIZE - width);
			if (i)
				set[i - 1] = shift[1];
			shift[1] = shift[0] << width;
		} while (++i < len);
		set[i - 1] = shift[1];
	}
}

bool compare_bitsets(bitset s1, bitset s2)
{
	size_t min_len = _MIN(_B_INDEX(BITSET_SIZE(s1)), _B_INDEX(BITSET_SIZE(s2))),
	       max_len = _MAX(_B_INDEX(BITSET_SIZE(s1)), _B_INDEX(BITSET_SIZE(s2))),
		   offset_s1 = _B_OFFSET(BITSET_SIZE(s1)),
	       i = -1;

	if (min_len != _B_INDEX(BITSET_SIZE(s1)))
		_SWAP(s1, s2);
	while (++i < min_len)
		if (s1[i] != s2[i])
			return false;
	if ((s1[i] & ~(~ (_word_t) 0 << (_WORD_SIZE - offset_s1)))
			!= (s2[i] & (~ (_word_t) 0 >> (_WORD_SIZE - offset_s1)))
			|| (min_len == max_len
				&& (s2[i] & (~ (_word_t) 0 >> (_WORD_SIZE - _B_OFFSET(BITSET_SIZE(s2))
							+ offset_s1) << offset_s1)))
			|| (min_len != max_len
				&& (s2[i] & (~ (_word_t) 0 << (_WORD_SIZE - offset_s1)))))
		return false;
	while (++i < max_len)
		if (s2[i])
			return false;
	return true;
}


bitset bitwise_not(bitset set)
{
	size_t len = _B_INDEX(BITSET_SIZE(set)),
	       i = 0;

	set = copy_bitset(set);
	do {
		set[i] = ~set[i];
	} while (++i < len);
	set[i] = ~(set[i] & (~ (_word_t) 0 >> (_WORD_SIZE - _B_OFFSET(BITSET_SIZE(set)))));
	return set;
}

bitset bitwise_and(bitset s1, bitset s2)
{
	size_t min_len = _MIN(_B_INDEX(BITSET_SIZE(s1)), _B_INDEX(BITSET_SIZE(s2))),
	       i = -1;
	bitset set_and = Bitset(_MAX(_B_INDEX(BITSET_SIZE(s1)), _B_INDEX(BITSET_SIZE(s2))));

	while (++i <= min_len)
		set_and[i] = s1[i] & s2[i];
	return set_and;
}

bitset bitwise_or(bitset s1, bitset s2)
{
	size_t min_len = _MIN(_B_INDEX(BITSET_SIZE(s1)), _B_INDEX(BITSET_SIZE(s2))),
	       i = -1;
	bitset set_or;

	if (min_len != _B_INDEX(BITSET_SIZE(s1)))
		_SWAP(s1, s2);
	set_or = copy_bitset(s2);

	while (++i < min_len)
		set_or[i] |= s1[i];
	set_or[i] |= s1[i] & (~ (_word_t) 0 >> (_WORD_SIZE - min_len));
	return set_or;
}

# ifdef NDEBUG

void print_bitset(bitset set)
{
	size_t len = BITSET_SIZE(set),                                                                                         \
	       i = 0;                                                                                                          \
                                                                                                                                \
	while (i < len) {                                                                                                     \
		printf("%s%s%d", !(i % 4) && i ? " " : "", !(i % 8) && i ? " " : "", GET_BIT(set, i));                    \
		++i;                                                                                                           \
	}                                                                                                                       \
	printf("\n");                                                                                                           \
	BITSET_SIZE(set);                                                                                                      \
}

# endif /* !NDEBUG */

