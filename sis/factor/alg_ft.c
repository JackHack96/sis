
/*
 *  alg_factor: algebraic factoring.
 */

#include "sis.h"
#include "../include/factor.h"
#include "../include/factor_int.h"

static void factor_special();

static void factor_general();

/*
 *  1.  Algebraic factoring of f using the factor generated by gen.
 *  2.  f will become the root of the s-p tree for the factored form of f.
 */
void
factor_recur(f, gen_factor)
        node_t *f;
        node_t *(*gen_factor)();
{
    node_t *k, *ck;

    if ((k = (*gen_factor)(f)) != NIL(node_t)) {

        ck = node_div(f, k, NIL(node_t * ));    /* find the co-kernel */
        node_free(k);

        if (node_num_cube(ck) == 1) {
            factor_special(f, ck, gen_factor);  /* special case: f = c*q+r */
        } else {
            factor_general(f, ck, gen_factor);     /* general case: f = c*p*q+r */
        }

        node_free(ck);
    }
}

static void
factor_special(f, ck, gen_factor)
        node_t *f, *ck;
        node_t *(*gen_factor)();
{
    node_t *bestl, *p1, *c1, *c2, *p;

    /* f = bestl*p1 + ...; p1 contains k for which ck is the co-kernel */
    bestl = factor_best_literal(f, ck);
    p1    = node_div(f, bestl, NIL(node_t * ));

    /* f = c2*p + ...; p is cube-free */
    c1 = node_largest_cube_divisor(p1);
    c2 = node_and(bestl, c1);
    p  = node_div(f, c2, NIL(node_t * ));

    node_free(p1);
    node_free(c1);
    node_free(c2);
    node_free(bestl);

    (void) node_substitute(f, p, 0);
    factor_recur(f, gen_factor);
    factor_recur(p, gen_factor);

}

static void
factor_general(f, ck, gen_factor)
        node_t *f, *ck;
        node_t *(*gen_factor)();
{
    node_t *c1, *p, *q1, *c2, *q;

    /* f = c1*p + ...; c1*p = ck */
    c1 = node_largest_cube_divisor(ck);
    p  = node_div(ck, c1, NIL(node_t * ));

    /* f = q1*p + ...;  q1 containts ck */
    q1 = node_div(f, p, NIL(node_t * ));

    /* f = c2*q*p + ...; c2*q = q1 */
    c2 = node_largest_cube_divisor(q1);
    q  = node_div(q1, c2, NIL(node_t * ));

    node_free(c1);
    node_free(q1);
    node_free(c2);

    (void) node_substitute(f, p, 0);
    (void) node_substitute(f, q, 0);

    factor_recur(f, gen_factor);
    factor_recur(p, gen_factor);
    factor_recur(q, gen_factor);
}
