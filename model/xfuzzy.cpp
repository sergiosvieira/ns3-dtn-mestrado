#include <string.h>
#include "xfuzzy.hpp"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//       Membership function of an input variable       //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++//

ParamMembershipFunction::ParamMembershipFunction(double min, double max,
double step) {
 this->min = min;
 this->max = max;
 this->step = step;
}

double ParamMembershipFunction::center() { return 0; }

double ParamMembershipFunction::basis() { return 0; }

double ParamMembershipFunction::compute_smeq(double x) {
 double degree=0, mu;
 for(double y=max; y>=x ; y-=step) if((mu = compute_eq(y))>degree) degree=mu;
 return degree;
}

double ParamMembershipFunction::compute_greq(double x) {
 double degree=0, mu;
 for(double y=min; y<=x ; y+=step) if((mu = compute_eq(y))>degree) degree=mu;
 return degree;
}

double ParamMembershipFunction::isEqual(MembershipFunction &mf) {
 if( mf.getType() == MembershipFunction::CRISP )
  { return compute_eq( ((FuzzySingleton &) mf).getValue()); }
 double mu1,mu2,minmu,degree=0;
 if( mf.getType() == MembershipFunction::INNER &&
     ((OutputMembershipFunction &) mf).isDiscrete() ){
  OutputMembershipFunction &omf = (OutputMembershipFunction &) mf;
  for(int i=0; i<omf.length; i++){
   mu1 = omf.conc[i]->degree();
   mu2 = compute_eq(omf.conc[i]->param(0));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 else {
  for(double x=min; x<=max; x+=step){
   mu1 = mf.compute(x);
   mu2 = compute_eq(x);
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 return degree;
}

double ParamMembershipFunction::isGreaterOrEqual(MembershipFunction &mf) {
 if( mf.getType() == MembershipFunction::CRISP )
  { return compute_greq( ((FuzzySingleton &) mf).getValue()); }
 double mu1,mu2,minmu,degree=0,greq=0;
 if( mf.getType() == MembershipFunction::INNER &&
     ((OutputMembershipFunction &) mf).isDiscrete() ){
  OutputMembershipFunction &omf = (OutputMembershipFunction &) mf;
  for(int i=0; i<omf.length; i++){
   mu1 = omf.conc[i]->degree();
   mu2 = compute_greq(omf.conc[i]->param(0));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 else {
  for(double x=min; x<=max; x+=step){
   mu1 = mf.compute(x);
   mu2 = compute_eq(x);
   if( mu2>greq ) greq = mu2;
   if( mu1<greq ) minmu = mu1; else minmu = greq;
   if( degree<minmu ) degree = minmu;
  }
 }
 return degree;
}

double ParamMembershipFunction::isSmallerOrEqual(MembershipFunction &mf) {
 if( mf.getType() == MembershipFunction::CRISP )
  { return compute_smeq( ((FuzzySingleton &) mf).getValue()); }
 double mu1,mu2,minmu,degree=0,smeq=0;
 if( mf.getType() == MembershipFunction::INNER &&
     ((OutputMembershipFunction &) mf).isDiscrete() ){
  OutputMembershipFunction &omf = (OutputMembershipFunction &) mf;
  for(int i=0; i<omf.length; i++){
   mu1 = omf.conc[i]->degree();
   mu2 = compute_smeq(omf.conc[i]->param(0));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 else {
  for(double x=max; x>=min; x-=step){
   mu1 = mf.compute(x);
   mu2 = compute_eq(x);
   if( mu2>smeq ) smeq = mu2;
   if( mu1<smeq ) minmu = mu1; else minmu = smeq;
   if( degree<minmu ) degree = minmu;
  }
 }
 return degree;
}

double ParamMembershipFunction::isGreater(MembershipFunction &mf,
Operatorset &op) {
 if( mf.getType() == MembershipFunction::CRISP )
  { return op.NOT(compute_smeq( ((FuzzySingleton &) mf).getValue())); }
 double mu1,mu2,minmu,gr,degree=0,smeq=0;
 if( mf.getType() == MembershipFunction::INNER &&
     ((OutputMembershipFunction &) mf).isDiscrete() ){
  OutputMembershipFunction &omf = (OutputMembershipFunction &) mf;
  for(int i=0; i<omf.length; i++){
   mu1 = omf.conc[i]->degree();
   mu2 = op.NOT(compute_smeq(omf.conc[i]->param(0)));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 else {
  for(double x=max; x>=min; x-=step){
   mu1 = mf.compute(x);
   mu2 = compute_eq(x);
   if( mu2>smeq ) smeq = mu2;
   gr = op.NOT(smeq);
   minmu = ( mu1<gr ? mu1 : gr);
   if( degree<minmu ) degree = minmu;
  }
 }
 return degree;
}

double ParamMembershipFunction::isSmaller(MembershipFunction &mf,
Operatorset &op) {
 if( mf.getType() == MembershipFunction::CRISP )
  { return op.NOT(compute_greq( ((FuzzySingleton &) mf).getValue())); }
 double mu1,mu2,minmu,sm,degree=0,greq=0;
 if( mf.getType() == MembershipFunction::INNER &&
     ((OutputMembershipFunction &) mf).isDiscrete() ){
  OutputMembershipFunction &omf = (OutputMembershipFunction &) mf;
  for(int i=0; i<omf.length; i++){
   mu1 = omf.conc[i]->degree();
   mu2 = op.NOT(compute_greq(omf.conc[i]->param(0)));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 else {
  for(double x=min; x<=max; x+=step){
   mu1 = mf.compute(x);
   mu2 = compute_eq(x);
   if( mu2>greq ) greq = mu2;
   sm = op.NOT(greq);
   minmu = ( mu1<sm ? mu1 : sm);
   if( degree<minmu ) degree = minmu;
  }
 }
 return degree;
}

double ParamMembershipFunction::isNotEqual(MembershipFunction &mf,
Operatorset &op) {
 if( mf.getType() == MembershipFunction::CRISP )
  { return op.NOT(compute_eq( ((FuzzySingleton &) mf).getValue())); }
 double mu1,mu2,minmu,degree=0;
 if( mf.getType() == MembershipFunction::INNER &&
     ((OutputMembershipFunction &) mf).isDiscrete() ){
  OutputMembershipFunction &omf = (OutputMembershipFunction &) mf;
  for(int i=0; i<omf.length; i++){
   mu1 = omf.conc[i]->degree();
   mu2 = op.NOT(compute_eq(omf.conc[i]->param(0)));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 else {
  for(double x=min; x<=max; x+=step){
   mu1 = mf.compute(x);
   mu2 = op.NOT(compute_eq(x));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 return degree;
}

double ParamMembershipFunction::isApproxEqual(MembershipFunction &mf,
Operatorset &op) {
 if( mf.getType() == MembershipFunction::CRISP )
  { return op.moreorless(compute_eq( ((FuzzySingleton &) mf).getValue())); }
 double mu1,mu2,minmu,degree=0;
 if( mf.getType() == MembershipFunction::INNER &&
     ((OutputMembershipFunction &) mf).isDiscrete() ){
  OutputMembershipFunction &omf = (OutputMembershipFunction &) mf;
  for(int i=0; i<omf.length; i++){
   mu1 = omf.conc[i]->degree();
   mu2 = op.moreorless(compute_eq(omf.conc[i]->param(0)));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 else {
  for(double x=min; x<=max; x+=step){
   mu1 = mf.compute(x);
   mu2 = op.moreorless(compute_eq(x));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 return degree;
}

double ParamMembershipFunction::isVeryEqual(MembershipFunction &mf,
Operatorset &op) {
 if( mf.getType() == MembershipFunction::CRISP )
  { return op.very(compute_eq( ((FuzzySingleton &) mf).getValue())); }
 double mu1,mu2,minmu,degree=0;
 if( mf.getType() == MembershipFunction::INNER &&
     ((OutputMembershipFunction &) mf).isDiscrete() ){
  OutputMembershipFunction &omf = (OutputMembershipFunction &) mf;
  for(int i=0; i<omf.length; i++){
   mu1 = omf.conc[i]->degree();
   mu2 = op.very(compute_eq(omf.conc[i]->param(0)));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 else {
  for(double x=min; x<=max; x+=step){
   mu1 = mf.compute(x);
   mu2 = op.very(compute_eq(x));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 return degree;
}

double ParamMembershipFunction::isSlightlyEqual(MembershipFunction &mf,
Operatorset &op) {
 if( mf.getType() == MembershipFunction::CRISP )
  { return op.slightly(compute_eq( ((FuzzySingleton &) mf).getValue())); }
 double mu1,mu2,minmu,degree=0;
 if( mf.getType() == MembershipFunction::INNER &&
     ((OutputMembershipFunction &) mf).isDiscrete() ){
  OutputMembershipFunction &omf = (OutputMembershipFunction &) mf;
  for(int i=0; i<omf.length; i++){
   mu1 = omf.conc[i]->degree();
   mu2 = op.slightly(compute_eq(omf.conc[i]->param(0)));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 else {
  for(double x=min; x<=max; x+=step){
   mu1 = mf.compute(x);
   mu2 = op.slightly(compute_eq(x));
   minmu = (mu1<mu2 ? mu1 : mu2);
   if( degree<minmu ) degree = minmu;
  }
 }
 return degree;
}

int ParamMembershipFunction::isXflSingleton() {
 int size = strlen(name);
 if(size < 14) return 0;
 if( !strcmp(&name[size-14],"_xfl_singleton") ) return 1;
 return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//       Membership function of an innner variable       //
//++++++++++++++++++++++++++++++++++++++++++++++++++++++//

OutputMembershipFunction::OutputMembershipFunction(Operatorset *op,int length,int inputlength,double *input){
 this->op = op;
 this->length = length;
 this->inputlength = inputlength;
 this->input = input;
 this->conc = new RuleConclusion*[length];
}
double OutputMembershipFunction::compute(double x) {
 double dom = op->imp(conc[0]->degree(),conc[0]->compute(x));
 for(int i=1; i<length; i++)
   dom = op->also(dom,op->imp(conc[i]->degree(),conc[i]->compute(x)));
 return dom;
}
int OutputMembershipFunction::isDiscrete() {
 for(int i=0; i<length; i++) if(!conc[i]->isDiscrete()) return 0;
 return 1;
}

