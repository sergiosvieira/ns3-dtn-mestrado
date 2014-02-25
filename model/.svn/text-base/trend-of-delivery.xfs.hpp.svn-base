#ifndef _TREND_OF_DELIVERY_INFERENCE_ENGINE_HPP
#define _TREND_OF_DELIVERY_INFERENCE_ENGINE_HPP

#include "xfuzzy.hpp"
#include "vector2d.h"

using namespace ns3;

//+++++++++++++++++++++++++++++++++++++//
//  MembershipFunction MF_trend-of-delivery.xfs_xfl_triangle  //
//+++++++++++++++++++++++++++++++++++++//

class MF_trendofdelivery_xfl_triangle: public ParamMembershipFunction {
private:
	double a;
	double b;
	double c;

public:
	MF_trendofdelivery_xfl_triangle() {
	}
	;
	virtual ~MF_trendofdelivery_xfl_triangle() {
	}
	;
	MF_trendofdelivery_xfl_triangle(double min, double max, double step,
			double *param, int length);
	MF_trendofdelivery_xfl_triangle*dup();
	virtual double param(int _i);
	virtual double compute_eq(double x);
	virtual double compute_greq(double x);
	virtual double compute_smeq(double x);
	virtual double center();
	virtual double basis();
};

//+++++++++++++++++++++++++++++++++++++//
//  Operatorset OP_trendofdelivery_op_default //
//+++++++++++++++++++++++++++++++++++++//

class OP_trendofdelivery_op_default: public Operatorset {
public:
	virtual ~OP_trendofdelivery_op_default() {
	}
	;
	virtual double AND(double a, double b);
	virtual double OR(double a, double b);
	virtual double also(double a, double b);
	virtual double imp(double a, double b);
	virtual double NOT(double a);
	virtual double very(double a);
	virtual double moreorless(double a);
	virtual double slightly(double a);
	virtual double defuz(OutputMembershipFunction &mf);
};

//+++++++++++++++++++++++++++++++++++++//
//  Type TP_trendofdelivery_tp_sentido //
//+++++++++++++++++++++++++++++++++++++//

class TP_trendofdelivery_tp_sentido {
private:
	double min;
	double max;
	double step;
public:
	MF_trendofdelivery_xfl_triangle sentido_otimo;
	MF_trendofdelivery_xfl_triangle sentido_bom;
	MF_trendofdelivery_xfl_triangle sentido_ruim;
	MF_trendofdelivery_xfl_triangle sentido_pessimo;
	TP_trendofdelivery_tp_sentido(const Vector2d &u, const Vector2d &v, const double &r);
	void setFuzzy(const Vector2d &u, const Vector2d &v, const double &r);
protected:
	double getAngle(const Vector2d &u, const Vector2d &v, const double &t);
};

//+++++++++++++++++++++++++++++++++++++//
//  Type TP_trendofdelivery_tp_distancia //
//+++++++++++++++++++++++++++++++++++++//

class TP_trendofdelivery_tp_distancia {
private:
	double min;
	double max;
	double step;
public:
	MF_trendofdelivery_xfl_triangle distancia_muito_perto;
	MF_trendofdelivery_xfl_triangle distancia_perto;
	MF_trendofdelivery_xfl_triangle distancia_longe;
	MF_trendofdelivery_xfl_triangle distancia_muito_longe;
	TP_trendofdelivery_tp_distancia();
};

//+++++++++++++++++++++++++++++++++++++//
//  Type TP_trendofdelivery_tp_velocidade //
//+++++++++++++++++++++++++++++++++++++//

class TP_trendofdelivery_tp_velocidade {
private:
	double min;
	double max;
	double step;
public:
	MF_trendofdelivery_xfl_triangle velocidade_baixa;
	MF_trendofdelivery_xfl_triangle velocidade_media;
	MF_trendofdelivery_xfl_triangle velocidade_alta;
	TP_trendofdelivery_tp_velocidade();
};

//+++++++++++++++++++++++++++++++++++++//
//  Type TP_trendofdelivery_tp_tendencia_de_entrega //
//+++++++++++++++++++++++++++++++++++++//

class TP_trendofdelivery_tp_tendencia_de_entrega {
private:
	double min;
	double max;
	double step;
public:
	MF_trendofdelivery_xfl_triangle tendencia_pessima;
	MF_trendofdelivery_xfl_triangle tendencia_muito_ruim;
	MF_trendofdelivery_xfl_triangle tendencia_ruim;
	MF_trendofdelivery_xfl_triangle tendencia_boa;
	MF_trendofdelivery_xfl_triangle tendencia_muito_boa;
	MF_trendofdelivery_xfl_triangle tendencia_otima;
	MF_trendofdelivery_xfl_triangle tendencia_maxima;
	TP_trendofdelivery_tp_tendencia_de_entrega();
};

//+++++++++++++++++++++++++++++++++++++//
//  Fuzzy Inference Engine trendofdelivery  //
//+++++++++++++++++++++++++++++++++++++//

class trendofdelivery: public FuzzyInferenceEngine {
public:
	trendofdelivery() {
		r = 0.0;
	};
	trendofdelivery(const Vector2d &u_, Vector2d &v_, const double &r_) {
		u = u_;
		v = v_;
		r = r_;
	};
	void setFuzzy(const Vector2d &u_, Vector2d &v_, const double &r_) {
		u = u_;
		v = v_;
		r = r_;
	};
	virtual ~trendofdelivery() {};
	virtual double* crispInference(double* input);
	virtual double* crispInference(MembershipFunction* &input);
	virtual MembershipFunction** fuzzyInference(double* input);
	virtual MembershipFunction** fuzzyInference(MembershipFunction* &input);
	void inference(double _i_in_sentido, double _i_in_distancia,
			double _i_in_velocidade, double *_o_out_tendencia_de_entrega);
protected:
	Vector2d u, v;
	double r;
private:
	void RL_rb_tendencia(MembershipFunction &rb_tendencia_sentido,
			MembershipFunction &rb_tendencia_distancia,
			MembershipFunction &rb_tendencia_velocidade,
			MembershipFunction ** _o_rb_tendencia_tendencia);
};

#endif /* _trend-of-delivery_INFERENCE_ENGINE_HPP */
