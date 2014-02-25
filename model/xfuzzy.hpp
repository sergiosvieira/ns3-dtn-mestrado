#ifndef _XFUZZY_HPP
#define _XFUZZY_HPP

//++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//           Abstract class of a fuzzy number           //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++//

class MembershipFunction {
public:
 enum Type { GENERAL, CRISP, INNER };
 virtual enum Type getType() { return GENERAL; }
 virtual double getValue() { return 0; }
 virtual double compute(double x) = 0;
 virtual ~MembershipFunction() {}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//               Class of a crisp number                //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++//

class FuzzySingleton: public MembershipFunction {
private:
 double value;

public:
 FuzzySingleton(double value) { this->value = value; }
 virtual ~FuzzySingleton() {}
 virtual double getValue() { return value; }
 virtual enum Type getType() { return CRISP; }
 virtual double compute(double x) { return (x==value? 1.0: 0.0);}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//      Abstract class of a fuzzy inference engine      //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++//

class FuzzyInferenceEngine {
public:
 virtual double* crispInference(double* input) = 0;
 virtual double* crispInference(MembershipFunction* &input) = 0;
 virtual MembershipFunction** fuzzyInference(double* input) = 0;
 virtual MembershipFunction** fuzzyInference(MembershipFunction* &input) = 0;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//          Abstract class of an operator set           //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++//

class OutputMembershipFunction;

class Operatorset {
public:
 virtual double AND(double a, double b) = 0;
 virtual double OR(double a, double b) = 0;
 virtual double also(double a, double b) = 0;
 virtual double imp(double a, double b) = 0;
 virtual double NOT(double a) = 0;
 virtual double very(double a) = 0;
 virtual double moreorless(double a) = 0;
 virtual double slightly(double a) = 0;
 virtual double defuz(OutputMembershipFunction &mf) = 0;
 virtual ~Operatorset(){}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//       Membership function of an input variable       //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++//

class ParamMembershipFunction {
public:
 double min, max, step;
 const char *name;

 ParamMembershipFunction(double min, double max, double step);
 ParamMembershipFunction() {};
 virtual ~ParamMembershipFunction() {};
 virtual double compute_eq(double x) = 0;
 virtual double param(int i) = 0;
 virtual double center();
 virtual double basis();
 virtual double compute_smeq(double x);
 virtual double compute_greq(double x);
 double isEqual(MembershipFunction &mf);
 double isGreaterOrEqual(MembershipFunction &mf);
 double isSmallerOrEqual(MembershipFunction &mf);
 double isGreater(MembershipFunction &mf, Operatorset &op);
 double isSmaller(MembershipFunction &mf, Operatorset &op);
 double isNotEqual(MembershipFunction &mf, Operatorset &op);
 double isApproxEqual(MembershipFunction &mf, Operatorset &op);
 double isVeryEqual(MembershipFunction &mf, Operatorset &op);
 double isSlightlyEqual(MembershipFunction &mf, Operatorset &op);
 int isXflSingleton();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//       Class for the conclusion of a fuzzy rule       //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++//

class RuleConclusion {
private:
 double deg;
 ParamMembershipFunction *mf;
public:
 RuleConclusion(double degree, ParamMembershipFunction *mf)
  { this->deg = degree; this->mf = mf; }
 ~RuleConclusion() { delete mf; }
 double degree() { return deg; }
 double compute(double x) { return mf->compute_eq(x); }
 double center() { return mf->center(); }
 double basis() { return mf->basis(); }
 double param(int i) { return mf->param(i); }
 double min() { return mf->min; }
 double max() { return mf->max; }
 double step() { return mf->step; }
 int isDiscrete() { return mf->isXflSingleton(); }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//      Membership function of an output variable       //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++//

class OutputMembershipFunction: public MembershipFunction {
public:
 int length;
 int inputlength;
 double *input;
 RuleConclusion **conc;
private:
 Operatorset *op;
public:
 OutputMembershipFunction() {};
 OutputMembershipFunction(Operatorset *op, int length, int inputlength, double *input);
 virtual ~OutputMembershipFunction() {if(op) delete op; if(conc){ for(int i = 0; i < length; i++) {delete conc[i];}} delete []conc;}///delete [] conc; }
 virtual enum Type getType() { return INNER; }
 double compute(double x);
 double defuzzify() { return op->defuz( (*this) ); }
 double min() { return conc[0]->min(); }
 double max() { return conc[0]->max(); }
 double step() { return conc[0]->step(); }
 int isDiscrete();
};

#endif /* _XFUZZY_HPP */
