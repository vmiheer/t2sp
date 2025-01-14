#ifndef HALIDE_INTERNAL_CSE_H
#define HALIDE_INTERNAL_CSE_H

/** \file
 * Defines a pass for introducing let expressions to wrap common sub-expressions. */

#include "IR.h"
#include <set>

namespace Halide {
namespace Internal {

/** Replace each common sub-expression in the argument with a
 * variable, and wrap the resulting expr in a let statement giving a
 * value to that variable.
 *
 * This is important to do within Halide (instead of punting to llvm),
 * because exprs that come in from the front-end are small when
 * considered as a graph, but combinatorially large when considered as
 * a tree. For an example of a such a case, see
 * test/code_explosion.cpp
 *
 * The last parameter determines whether all common subexpressions are
 * lifted, or only those that the simplifier would not subsitute back
 * in (e.g. addition of a constant).
 */
Expr common_subexpression_elimination(const Expr &, bool lift_all = false);

/** Do common-subexpression-elimination on each expression in a
 * statement. Does not introduce let statements. */
Stmt common_subexpression_elimination(const Stmt &, bool lift_all = false);

/** Remove Lets and/or LetStmts from statement s. If funcs_only is true, modify
 *  only the given funcs. If serial_loop_only is true, a Let/LetStmt must be inside
 *  a serial loop to be removed.
 */
Stmt remove_lets(const Stmt &s, bool remove_Lets, bool remove_LetStmts, bool funcs_only,
                 bool serial_loop_only, const std::set<std::string> &funcs);

void cse_test();

}  // namespace Internal
}  // namespace Halide

#endif
