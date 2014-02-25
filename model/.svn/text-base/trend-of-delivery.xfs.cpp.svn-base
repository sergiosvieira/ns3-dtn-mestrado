#ifndef _TREND_OF_DELIVERY_INFERENCE_ENGINE_
#define _TREND_OF_DELIVERY_INFERENCE_ENGINE_

#include <stdio.h>
#include <math.h>
#include "xfuzzy.hpp"
#include "trend-of-delivery.xfs.hpp"

//+++++++++++++++++++++++++++++++++++++//
//  MembershipFunction MF_trendofdelivery.xfs_xfl_triangle  //
//+++++++++++++++++++++++++++++++++++++//

MF_trendofdelivery_xfl_triangle::MF_trendofdelivery_xfl_triangle(double min,
		double max, double step, double *param, int length) :
	ParamMembershipFunction(min, max, step) {

	this->name = "MF_trendofdelivery_xfl_triangle";
	this->a = param[0];
	this->b = param[1];
	this->c = param[2];
}

MF_trendofdelivery_xfl_triangle * MF_trendofdelivery_xfl_triangle::dup() {
	double param[3] = { a, b, c };
	return new MF_trendofdelivery_xfl_triangle(min, max, step, param, 3);
}

double MF_trendofdelivery_xfl_triangle::param(int _i) {
	switch (_i) {
	case 0:
		return a;
	case 1:
		return b;
	case 2:
		return c;
	default:
		return 0;
	}
}

double MF_trendofdelivery_xfl_triangle::compute_eq(double x) {
	return (a < x && x <= b ? (x - a) / (b - a) : (b < x && x < c ? (c - x)
			/ (c - b) : 0));
}

double MF_trendofdelivery_xfl_triangle::compute_greq(double x) {
	return (x < a ? 0 : (x > b ? 1 : (x - a) / (b - a)));
}

double MF_trendofdelivery_xfl_triangle::compute_smeq(double x) {
	return (x < b ? 1 : (x > c ? 0 : (c - x) / (c - b)));
}

double MF_trendofdelivery_xfl_triangle::center() {
	return b;
}

double MF_trendofdelivery_xfl_triangle::basis() {
	return (c - a);
}

//+++++++++++++++++++++++++++++++++++++//
//  Operatorset OP_trendofdelivery_op_default //
//+++++++++++++++++++++++++++++++++++++//

double OP_trendofdelivery_op_default::AND(double a, double b) {
	return (a < b ? a : b);
}
double OP_trendofdelivery_op_default::OR(double a, double b) {
	return (a > b ? a : b);
}
double OP_trendofdelivery_op_default::also(double a, double b) {
	return (a > b ? a : b);
}
double OP_trendofdelivery_op_default::imp(double a, double b) {
	return (a < b ? a : b);
}
double OP_trendofdelivery_op_default::NOT(double a) {
	return 1 - a;
}

double OP_trendofdelivery_op_default::very(double a) {
	double w = 2.0;
	return pow(a, w);
}

double OP_trendofdelivery_op_default::moreorless(double a) {
	double w = 0.5;
	return pow(a, w);
}

double OP_trendofdelivery_op_default::slightly(double a) {
	return 4 * a * (1 - a);
}

double OP_trendofdelivery_op_default::defuz(OutputMembershipFunction &mf) {
	double min = mf.min();
	double max = mf.max();
	double step = mf.step();
	double num = 0, denom = 0;
	for (double x = min; x <= max; x += step) {
		double m = mf.compute(x);
		num += x * m;
		denom += m;
	}
	if (denom == 0)
		return (min + max) / 2;
	return num / denom;
}

//+++++++++++++++++++++++++++++++++++++//
//  Type TP_trendofdelivery_tp_sentido //
//+++++++++++++++++++++++++++++++++++++//

double TP_trendofdelivery_tp_sentido::getAngle(const Vector2d &u,
		const Vector2d &v, const double &t) {
	Vector2d w = v - u;
	Vector2d k = w.normal();
	k = k.normalize() * t;
	k = k + v;
	return k.angle(w);
}

TP_trendofdelivery_tp_sentido::TP_trendofdelivery_tp_sentido(const Vector2d &u,
		const Vector2d &v, const double &r) {
	min = 0.0;
	max = 180.0;
	//step = 60.0;

	double theta_ = getAngle(u, v, r);
	step = theta_;
	//double theta2_ = 90.0 - theta_;
	/*
	 double _p_sentido_otimo[3] = { -60.0, 0.0, 60.0 };
	 double _p_sentido_bom[3] = { 0.0, 60.0, 120.0 };
	 double _p_sentido_ruim[3] = { 60.0, 120.0, 180.0 };
	 double _p_sentido_pessimo[3] = { 120.0, 180.0, 240.0 };
	 */
	double _p_sentido_otimo[3] = { -theta_, 0.0, theta_ };
	double _p_sentido_bom[3] = { theta_, (90.0 - theta_) / 2.0, 90.0 };
	double
			_p_sentido_ruim[3] = { 90.0, (180.0 - theta_) / 2.0, 180.0 - theta_ };
	double _p_sentido_pessimo[3] = { 180.0 - theta_, 180.0, 180.0 + theta_ };

	sentido_otimo = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_sentido_otimo, 3);
	sentido_bom = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_sentido_bom, 3);
	sentido_ruim = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_sentido_ruim, 3);
	sentido_pessimo = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_sentido_pessimo, 3);
}

//+++++++++++++++++++++++++++++++++++++//
//  Type TP_trendofdelivery_tp_distancia //
//+++++++++++++++++++++++++++++++++++++//

void TP_trendofdelivery_tp_sentido::setFuzzy(const Vector2d &u,
		const Vector2d &v, const double &r) {
	min = 0.0;
	max = 180.0;

	double theta_ = getAngle(u, v, r);
	step = theta_;

	double _p_sentido_otimo[3] = { -theta_, 0.0, theta_ };
	double _p_sentido_bom[3] = { theta_, (90.0 - theta_) / 2.0, 90.0 };
	double
			_p_sentido_ruim[3] = { 90.0, (180.0 - theta_) / 2.0, 180.0 - theta_ };
	double _p_sentido_pessimo[3] = { 180.0 - theta_, 180.0, 180.0 + theta_ };

	sentido_otimo = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_sentido_otimo, 3);
	sentido_bom = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_sentido_bom, 3);
	sentido_ruim = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_sentido_ruim, 3);
	sentido_pessimo = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_sentido_pessimo, 3);

}

TP_trendofdelivery_tp_distancia::TP_trendofdelivery_tp_distancia() {
	min = 0.0;
	max = 1050.0;
	step = 350.0;
	double _p_distancia_muito_perto[3] = { -350.0, 0.0, 350.0 };
	double _p_distancia_perto[3] = { 0.0, 350.0, 700.0 };
	double _p_distancia_longe[3] = { 350.0, 700.0, 1050.0 };
	double _p_distancia_muito_longe[3] = { 700.0, 1050.0, 1400.0 };
	distancia_muito_perto = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_distancia_muito_perto, 3);
	distancia_perto = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_distancia_perto, 3);
	distancia_longe = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_distancia_longe, 3);
	distancia_muito_longe = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_distancia_muito_longe, 3);
}

//+++++++++++++++++++++++++++++++++++++//
//  Type TP_trendofdelivery_tp_velocidade //
//+++++++++++++++++++++++++++++++++++++//

TP_trendofdelivery_tp_velocidade::TP_trendofdelivery_tp_velocidade() {
	min = 10.0;
	max = 30.0;
	step = 10.0;
	double _p_velocidade_baixa[3] = { 0.0, 10.0, 20.0 };
	double _p_velocidade_media[3] = { 10.0, 20.0, 30.0 };
	double _p_velocidade_alta[3] = { 20.0, 30.0, 40.0 };
	velocidade_baixa = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_velocidade_baixa, 3);
	velocidade_media = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_velocidade_media, 3);
	velocidade_alta = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_velocidade_alta, 3);
}

//+++++++++++++++++++++++++++++++++++++//
//  Type TP_trendofdelivery_tp_tendencia_de_entrega //
//+++++++++++++++++++++++++++++++++++++//

TP_trendofdelivery_tp_tendencia_de_entrega::TP_trendofdelivery_tp_tendencia_de_entrega() {
	min = 0.0;
	max = 1.0;
	step = 0.16666666666666666;
	double _p_tendencia_pessima[3] = { -0.16666666666666666, 0.0,
			0.16666666666666666 };
	double _p_tendencia_muito_ruim[3] = { 0.0, 0.16666666666666666,
			0.3333333333333333 };
	double _p_tendencia_ruim[3] = { 0.16666666666666666, 0.3333333333333333,
			0.5 };
	double
			_p_tendencia_boa[3] = { 0.3333333333333333, 0.5, 0.6666666666666666 };
	double _p_tendencia_muito_boa[3] = { 0.5, 0.6666666666666666,
			0.8333333333333333 };
	double _p_tendencia_otima[3] = { 0.6666666666666666, 0.8333333333333333,
			1.0 };
	double _p_tendencia_maxima[3] = { 0.99, 1.0, 1.1666666666666665 };
	tendencia_pessima = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_tendencia_pessima, 3);
	tendencia_muito_ruim = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_tendencia_muito_ruim, 3);
	tendencia_ruim = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_tendencia_ruim, 3);
	tendencia_boa = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_tendencia_boa, 3);
	tendencia_muito_boa = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_tendencia_muito_boa, 3);
	tendencia_otima = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_tendencia_otima, 3);
	tendencia_maxima = MF_trendofdelivery_xfl_triangle(min, max, step,
			_p_tendencia_maxima, 3);
}

//+++++++++++++++++++++++++++++++++++++//
//  Rulebase RL_rb_tendencia //
//+++++++++++++++++++++++++++++++++++++//

void trendofdelivery::RL_rb_tendencia(MembershipFunction &rb_tendencia_sentido,
		MembershipFunction &rb_tendencia_distancia,
		MembershipFunction &rb_tendencia_velocidade,
		MembershipFunction ** _o_rb_tendencia_tendencia) {
	OP_trendofdelivery_op_default _op;
	double _rl;
	int _i_rb_tendencia_tendencia = 0;
	double *_input = new double[3];
	_input[0] = rb_tendencia_sentido.getValue();
	_input[1] = rb_tendencia_distancia.getValue();
	_input[2] = rb_tendencia_velocidade.getValue();
	OutputMembershipFunction *rb_tendencia_tendencia =
			new OutputMembershipFunction(new OP_trendofdelivery_op_default(),
					48, 3, _input);
	TP_trendofdelivery_tp_sentido _t_rb_tendencia_sentido(u, v, r);
	TP_trendofdelivery_tp_distancia _t_rb_tendencia_distancia;
	TP_trendofdelivery_tp_velocidade _t_rb_tendencia_velocidade;
	TP_trendofdelivery_tp_tendencia_de_entrega _t_rb_tendencia_tendencia;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_maxima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_otima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_otima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_perto.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_pessima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_otimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_boa.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_bom.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_ruim.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_muito_ruim.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_alta.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_pessima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_media.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_pessima.dup());
	_i_rb_tendencia_tendencia++;
	_rl = _op.AND(_t_rb_tendencia_sentido.sentido_pessimo.isEqual(
			rb_tendencia_sentido), _op.AND(
			_t_rb_tendencia_distancia.distancia_muito_longe.isEqual(
					rb_tendencia_distancia),
			_t_rb_tendencia_velocidade.velocidade_baixa.isEqual(
					rb_tendencia_velocidade)));
	(*rb_tendencia_tendencia).conc[_i_rb_tendencia_tendencia]
			= new RuleConclusion(_rl,
					_t_rb_tendencia_tendencia.tendencia_pessima.dup());
	_i_rb_tendencia_tendencia++;
	*_o_rb_tendencia_tendencia = rb_tendencia_tendencia;
	delete _input;
}
//+++++++++++++++++++++++++++++++++++++//
//          Inference Engine           //
//+++++++++++++++++++++++++++++++++++++//

double* trendofdelivery::crispInference(double *_input) {
	FuzzySingleton in_sentido(_input[0]);
	FuzzySingleton in_distancia(_input[1]);
	FuzzySingleton in_velocidade(_input[2]);
	MembershipFunction *out_tendencia_de_entrega;
	RL_rb_tendencia(in_sentido, in_distancia, in_velocidade,
			&out_tendencia_de_entrega);
	double *_output = new double[1];
	if (out_tendencia_de_entrega->getType() == MembershipFunction::CRISP)
		_output[0] = ((FuzzySingleton *) out_tendencia_de_entrega)->getValue();
	else
		_output[0]
				= ((OutputMembershipFunction *) out_tendencia_de_entrega)->defuzzify();
	delete out_tendencia_de_entrega;
	return _output;
}

double* trendofdelivery::crispInference(MembershipFunction* &_input) {
	MembershipFunction & in_sentido = _input[0];
	MembershipFunction & in_distancia = _input[1];
	MembershipFunction & in_velocidade = _input[2];
	MembershipFunction *out_tendencia_de_entrega;
	RL_rb_tendencia(in_sentido, in_distancia, in_velocidade,
			&out_tendencia_de_entrega);
	double *_output = new double[1];
	if (out_tendencia_de_entrega->getType() == MembershipFunction::CRISP)
		_output[0] = ((FuzzySingleton *) out_tendencia_de_entrega)->getValue();
	else
		_output[0]
				= ((OutputMembershipFunction *) out_tendencia_de_entrega)->defuzzify();
	delete out_tendencia_de_entrega;
	return _output;
}

MembershipFunction** trendofdelivery::fuzzyInference(double *_input) {
	FuzzySingleton in_sentido(_input[0]);
	FuzzySingleton in_distancia(_input[1]);
	FuzzySingleton in_velocidade(_input[2]);
	MembershipFunction *out_tendencia_de_entrega;
	RL_rb_tendencia(in_sentido, in_distancia, in_velocidade,
			&out_tendencia_de_entrega);
	MembershipFunction **_output = new MembershipFunction *[1];
	_output[0] = out_tendencia_de_entrega;
	return _output;
}

MembershipFunction** trendofdelivery::fuzzyInference(
		MembershipFunction* &_input) {
	MembershipFunction & in_sentido = _input[0];
	MembershipFunction & in_distancia = _input[1];
	MembershipFunction & in_velocidade = _input[2];
	MembershipFunction *out_tendencia_de_entrega;
	RL_rb_tendencia(in_sentido, in_distancia, in_velocidade,
			&out_tendencia_de_entrega);
	MembershipFunction **_output = new MembershipFunction *[1];
	_output[0] = out_tendencia_de_entrega;
	return _output;
}

void trendofdelivery::inference(double _i_in_sentido, double _i_in_distancia,
		double _i_in_velocidade, double *_o_out_tendencia_de_entrega) {
	FuzzySingleton in_sentido(_i_in_sentido);
	FuzzySingleton in_distancia(_i_in_distancia);
	FuzzySingleton in_velocidade(_i_in_velocidade);
	MembershipFunction *out_tendencia_de_entrega;
	RL_rb_tendencia(in_sentido, in_distancia, in_velocidade,
			&out_tendencia_de_entrega);
	if (out_tendencia_de_entrega->getType() == MembershipFunction::CRISP)
		(*_o_out_tendencia_de_entrega)
				= ((FuzzySingleton *) out_tendencia_de_entrega)->getValue();
	else
		(*_o_out_tendencia_de_entrega)
				= ((OutputMembershipFunction *) out_tendencia_de_entrega)->defuzzify();
	delete out_tendencia_de_entrega;
}

#endif /* _trendofdelivery_INFERENCE_ENGINE_ */
