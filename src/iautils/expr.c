#include "expr.h"

static Texpr* expr_new(TexprKind kd, Texpr* lc, Texpr* rc, Texpr* child) {
	Texpr *expr = calloc(1, sizeof(*expr));
	expr->e_kd = kd;
	expr->e_rc = rc;
	expr->e_lc = lc;
	expr->e_child = child;

	return expr;
}

Texpr* expr_cst(int cst) {
	Texpr *expr = new_expr(EXPD_CST, NULL, NULL, NULL);
	expr->e_cst = cst;
	return expr;
}

Texpr* expr_varEated(char* var) {
	Texpr *expr = new_expr(EXPD_VAR, NULL, NULL, NULL);
	expr->e_var = var;
	return expr;
}

Texpr* expr_uniOp(TexprKind kd, Texpr* child) {
	Texpr *expr = new_expr(kd, NULL, NULL, child);
	return expr;
}
Texpr* expr_binOp(TexprKind kd, Texpr* lc, Texpr* rc) {
	Texpr *expr = new_expr(kd, lc, rc, NULL);
	return expr;
}

void expr_free(Texpr* expr) {
	if(expr->e_lc) {
		expr_free(expr->e_lc);
	}
	if(expr->e_rc) {
		expr_free(expr->e_rc);
	}
	if(expr->e_child) {
		expr_free(expr->e_child);
	}

	free(expr);
}

int expr_eval(const Texpr* expr, const Tvars* vars, int* val, Cstr*uv) {
	int lr, rr, ret;
	Tvar *var;

	switch(expr->e_kd) {
		case EXPKD_NEG:
			if(ret = expr_eval(expr->e_child, vars, val, uv)) {
				return ret;
			}
			*val *= -1;
			break;
		
		case EXPKD_VAR:
			var = vars_get(vars, expr->e_var);
			if(!var) {
				*uv = expr->e_var;
				return 1;
			}
			*val = var->val;
			break;
			
		case EXPKD_CST:
			*val = expr->e_cst;
			break;

		case EXPKD_PLUS:
			if(ret = expr_eval(expr->e_lc, vars, &lr, uv)) {
				return ret;
			}
			if(ret = expr_eval(expr->e_lr, vars, &rr, uv)) {
				return ret;
			}
			*val = lr + rr;
			break;

		case EXPKD_MINUS:
			if(ret = expr_eval(expr->e_lc, vars, &lr, uv)) {
				return ret;
			}
			if(ret = expr_eval(expr->e_lr, vars, &rr, uv)) {
				return ret;
			}
			*val = lr - rr;
			break;

		case EXPKD_TIME:
			if(ret = expr_eval(expr->e_lc, vars, &lr, uv)) {
				return ret;
			}
			if(ret = expr_eval(expr->e_lr, vars, &rr, uv)) {
				return ret;
			}
			*val = lr * rr;
			break;

		case EXPKD_DIV:
			if(ret = expr_eval(expr->e_lc, vars, &lr, uv)) {
				return ret;
			}
			if(ret = expr_eval(expr->e_lr, vars, &rr, uv)) {
				return ret;
			}
			if(!rr) {
				return 2;
			}
			*val = lr / rr;
			break;

		case EXPKD_MOD:
			if(ret = expr_eval(expr->e_lc, vars, &lr, uv)) {
				return ret;
			}
			if(ret = expr_eval(expr->e_lr, vars, &rr, uv)) {
				return ret;
			}
			if(!rr) {
				return 2;
			}
			*val = lr % rr;
			break;

		case EXPKD_NONE:
		default:
			*v = 0;
			break;
	}
	
	return 0;
}
