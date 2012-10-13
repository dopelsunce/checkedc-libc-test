#include <stdint.h>
#include <stdio.h>
#include "util.h"

static struct l_ll t[] = {
#if LDBL_MANT_DIG == 53
#include "sanity/modf.h"
#elif LDBL_MANT_DIG == 64
#include "sanity/modfl.h"
#endif
};

int main(void)
{
	#pragma STDC FENV_ACCESS ON
	long double y, yi;
	float d, di;
	int e, i, err = 0;
	struct l_ll *p;

	for (i = 0; i < sizeof t/sizeof *t; i++) {
		p = t + i;

		if (p->r < 0)
			continue;
		fesetround(p->r);
		feclearexcept(FE_ALL_EXCEPT);
		y = modfl(p->x, &yi);
		e = fetestexcept(INEXACT|INVALID|DIVBYZERO|UNDERFLOW|OVERFLOW);

		if (!checkexcept(e, p->e, p->r)) {
			printf("%s:%d: bad fp exception: %s modf(%La)=%La,%La, want %s",
				p->file, p->line, rstr(p->r), p->x, p->y, p->y2, estr(p->e));
			printf(" got %s\n", estr(e));
			err++;
		}
		d = ulperr(y, p->y, p->dy);
		di = ulperr(yi, p->y2, p->dy2);
		if (!checkulp(d, p->r) || !checkulp(di, p->r)) {
			printf("%s:%d: %s modf(%La) want %La,%La got %La,%La, ulperr %.3f = %a + %a, %.3f = %a + %a\n",
				p->file, p->line, rstr(p->r), p->x, p->y, p->y2, y, yi, d, d-p->dy, p->dy, di, di-p->dy2, p->dy2);
			err++;
		}
	}
	return !!err;
}
