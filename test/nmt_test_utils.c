#include "namaster.h"
#include "ctest.h"
#include "utils.h"
#include "nmt_test_utils.h"

int *test_get_sequence(int n0,int nf)
{
  int i;
  int *seq=malloc((nf-n0)*sizeof(int));
  ASSERT_NOT_NULL(seq);
  for(i=0;i<(nf-n0);i++)
    seq[i]=n0+i;
  return seq;
}

void test_compare_arrays(int n,double *y,int narr,int iarr,char *fname)
{
  int ii;
  FILE *fi=fopen(fname,"r");
  ASSERT_NOT_NULL(fi);
  for(ii=0;ii<n;ii++) {
    int j;
    double xv,yv,dum;
    int stat=fscanf(fi,"%lf",&xv);
    ASSERT_EQUAL(stat,1);
    for(j=0;j<narr;j++) {
      stat=fscanf(fi,"%lE",&dum);
      ASSERT_EQUAL(stat,1);
      if(j==iarr)
	yv=dum;
    }
    ASSERT_DBL_NEAR_TOL(yv,y[ii],fmax(fabs(yv),fabs(y[ii]))*1E-3);
  }
  fclose(fi);
}

CTEST(nmt,drc3jj) {
  int sizew=1000;
  double *w3=my_malloc(sizew*sizeof(double));
  int r,l1min,l1max;

  r=drc3jj(2,3,0,0,&l1min,&l1max,w3,sizew);
  ASSERT_EQUAL(r,0);
  ASSERT_EQUAL(l1max,2+3);
  ASSERT_EQUAL(l1min,1);
  ASSERT_DBL_NEAR_TOL(-sqrt(3./35.),w3[0],1E-10);
  ASSERT_DBL_NEAR_TOL(0,w3[1],1E-10);
  ASSERT_DBL_NEAR_TOL(2/sqrt(105.),w3[2],1E-10);
  ASSERT_DBL_NEAR_TOL(0,w3[3],1E-10);
  ASSERT_DBL_NEAR_TOL(-sqrt(10./231.),w3[4],1E-10);

  r=drc3jj(100,200,2,-2,&l1min,&l1max,w3,sizew);
  ASSERT_EQUAL(r,0);
  ASSERT_EQUAL(l1max,100+200);
  ASSERT_EQUAL(l1min,100);
  ASSERT_DBL_NEAR_TOL(0.0139534,w3[0],1E-5);
  ASSERT_DBL_NEAR_TOL(-0.00192083,w3[30],1E-5);
  ASSERT_DBL_NEAR_TOL(0.000639717,w3[54],1E-5);
  ASSERT_DBL_NEAR_TOL(0.000648742,w3[131],1E-5);

  set_error_policy(THROW_ON_ERROR);
  printf("\nError messages expected: \n");
  try { r=drc3jj(100,200,2,-2,&l1min,&l1max,w3,100); }
  catch(1) {}
  ASSERT_EQUAL(exception_status,1);
  try { r=drc3jj(2,3,3,0,&l1min,&l1max,w3,sizew); }
  catch(1) {}
  ASSERT_EQUAL(exception_status,1);
  
  set_error_policy(EXIT_ON_ERROR);
  free(w3);
}

CTEST(nmt,mp_pinv) {
  gsl_matrix *M=gsl_matrix_alloc(3,3);

  //Non-degenerate matrix
  gsl_matrix_set(M,0,0,14.);
  gsl_matrix_set(M,0,1,2.);
  gsl_matrix_set(M,0,2,13.);
  gsl_matrix_set(M,1,0,2.);
  gsl_matrix_set(M,1,1,1.);
  gsl_matrix_set(M,1,2,2.);
  gsl_matrix_set(M,2,0,13.);
  gsl_matrix_set(M,2,1,2.);
  gsl_matrix_set(M,2,2,13.);
  moore_penrose_pinv(M,1E-5);
  ASSERT_DBL_NEAR_TOL( 1.   ,gsl_matrix_get(M,0,0),1E-5);
  ASSERT_DBL_NEAR_TOL( 0.   ,gsl_matrix_get(M,0,1),1E-5);
  ASSERT_DBL_NEAR_TOL(-1.   ,gsl_matrix_get(M,0,2),1E-5);
  ASSERT_DBL_NEAR_TOL( 0.   ,gsl_matrix_get(M,1,0),1E-5);
  ASSERT_DBL_NEAR_TOL(13./9.,gsl_matrix_get(M,1,1),1E-5);
  ASSERT_DBL_NEAR_TOL(-2./9.,gsl_matrix_get(M,1,2),1E-5);
  ASSERT_DBL_NEAR_TOL(-1.   ,gsl_matrix_get(M,2,0),1E-5);
  ASSERT_DBL_NEAR_TOL(-2./9.,gsl_matrix_get(M,2,1),1E-5);
  ASSERT_DBL_NEAR_TOL(10./9.,gsl_matrix_get(M,2,2),1E-5);
  
  //Degenerate matrix
  gsl_matrix_set(M,0,0,1.);
  gsl_matrix_set(M,0,1,0.);
  gsl_matrix_set(M,0,2,1.);
  gsl_matrix_set(M,1,0,0.);
  gsl_matrix_set(M,1,1,1.);
  gsl_matrix_set(M,1,2,1.);
  gsl_matrix_set(M,2,0,1.);
  gsl_matrix_set(M,2,1,1.);
  gsl_matrix_set(M,2,2,2.);
  moore_penrose_pinv(M,1E-5);
  ASSERT_DBL_NEAR_TOL( 5./9.,gsl_matrix_get(M,0,0),1E-5);
  ASSERT_DBL_NEAR_TOL(-4./9.,gsl_matrix_get(M,0,1),1E-5);
  ASSERT_DBL_NEAR_TOL( 1./9.,gsl_matrix_get(M,0,2),1E-5);
  ASSERT_DBL_NEAR_TOL(-4./9.,gsl_matrix_get(M,1,0),1E-5);
  ASSERT_DBL_NEAR_TOL( 5./9.,gsl_matrix_get(M,1,1),1E-5);
  ASSERT_DBL_NEAR_TOL( 1./9.,gsl_matrix_get(M,1,2),1E-5);
  ASSERT_DBL_NEAR_TOL( 1./9.,gsl_matrix_get(M,2,0),1E-5);
  ASSERT_DBL_NEAR_TOL( 1./9.,gsl_matrix_get(M,2,1),1E-5);
  ASSERT_DBL_NEAR_TOL( 2./9.,gsl_matrix_get(M,2,2),1E-5);

  gsl_matrix_free(M);
}

#define M2_01 1./12. //variance top hat
#define M2_POISSONL 1000. //variance top hat
#define M2_POISSONS 0.5 //variance top hat
#define M2_GAUSS 1. //variance Gaussian
CTEST(nmt,rngs) {
  gsl_rng *r=init_rng(1234);
  int ii;
  double m_01=0,s_01=0;
  double m_poissonl=0,s_poissonl=0;
  double m_poissons=0,s_poissons=0;
  double m_gauss=0,s_gauss=0;
  double m_gaussm=0,s_gaussm=0;
  double m_gaussp=0,s_gaussp=0;
  for(ii=0;ii<NNO_RNG;ii++) {
    double n,m;
    //Top-hat
    n=rng_01(r)-0.5; m_01+=n; s_01+=n*n;
    ASSERT_DBL_NEAR_TOL(n,0,0.501);
    //Poisson
    n=rng_poisson(M2_POISSONL,r)+0.; m_poissonl+=n; s_poissonl+=n*n;
    ASSERT_TRUE((int)(n>=0));
    n=rng_poisson(M2_POISSONS,r)+0.; m_poissons+=n; s_poissons+=n*n;
    ASSERT_TRUE((int)(n>=0));
    //Gaussian
    rng_gauss(r,&n,&m); m_gauss+=n; s_gauss+=n*n;
    rng_delta_gauss(&n,&m,r,M2_GAUSS); m/=(2*M_PI); m-=0.5;
    m_gaussm+=n; s_gaussm+=n*n; m_gaussp+=m; s_gaussp+=m*m;
    ASSERT_TRUE((int)(n>=0));
  }
  //Top-hat
  m_01/=NNO_RNG; s_01=sqrt(s_01/NNO_RNG-m_01*m_01);
  ASSERT_DBL_NEAR_TOL(m_01,0,20.*sqrt(M2_01/NNO_RNG));
  ASSERT_DBL_NEAR_TOL(s_01,sqrt(M2_01),1.6/sqrt(NNO_RNG+0.));
  //Poisson
  //Mean and variance of Poisson distribution are both lambda
  m_poissonl/=NNO_RNG; s_poissonl=sqrt(s_poissonl/NNO_RNG-m_poissonl*m_poissonl);
  ASSERT_DBL_NEAR_TOL(m_poissonl,M2_POISSONL,20.*sqrt(M2_POISSONL/NNO_RNG));
  ASSERT_DBL_NEAR_TOL(s_poissonl,sqrt(M2_POISSONL),158./sqrt(NNO_RNG+0.));
  m_poissons/=NNO_RNG; s_poissons=sqrt(s_poissons/NNO_RNG-m_poissons*m_poissons);
  ASSERT_DBL_NEAR_TOL(m_poissons,M2_POISSONS,20.*sqrt(M2_POISSONS/NNO_RNG));
  ASSERT_DBL_NEAR_TOL(s_poissons,sqrt(M2_POISSONS),10./sqrt(NNO_RNG+0.));
  //Gaussian
  m_gauss/=NNO_RNG; s_gauss=sqrt(s_gauss/NNO_RNG-m_gauss*m_gauss);
  ASSERT_DBL_NEAR_TOL(m_gauss,0,20.*sqrt(M2_GAUSS/NNO_RNG));
  ASSERT_DBL_NEAR_TOL(s_gauss,sqrt(M2_GAUSS),3.2/sqrt(NNO_RNG+0.));
  m_gaussm/=NNO_RNG; s_gaussm=sqrt(s_gaussm/NNO_RNG-m_gaussm*m_gaussm);
  m_gaussp/=NNO_RNG; s_gaussp=sqrt(s_gaussp/NNO_RNG-m_gaussp*m_gaussp);
  //Mean of Rayleigh distribution is sigma*sqrt(pi/2)
  //Variance of Rayleigh distribution is (4-pi)*sigma/2
  //Our sigma is sqrt(M2_GAUSS/2), since rng_delta_gauss returns modulus and phase
  // such that <r1**2+r2**2>=sigma2.
  ASSERT_DBL_NEAR_TOL(m_gaussm,sqrt(M2_GAUSS*M_PI/4),20.*sqrt((4-M_PI)*M2_01/4./NNO_RNG));
  ASSERT_DBL_NEAR_TOL(s_gaussm,sqrt((4-M_PI)*M2_GAUSS/4.),3./sqrt(NNO_RNG+0.));
  ASSERT_DBL_NEAR_TOL(m_gaussp,0,20.*sqrt(M2_01/NNO_RNG));
  ASSERT_DBL_NEAR_TOL(s_gaussp,sqrt(M2_01),1.6/sqrt(NNO_RNG+0.));

  end_rng(r);
}

CTEST(nmt,my_linecount) {
  FILE *f=my_fopen("test/cls.txt","r");
  int cnt=my_linecount(f);
  ASSERT_EQUAL(cnt,768);
  fclose(f);
}

CTEST(nmt,errors) {
  set_error_policy(THROW_ON_ERROR);

  printf("\nError messages expected: \n");

  //File doesn't exist
  try { FILE *f=my_fopen("test/cls.txtb","r"); }
  catch(1) {}
  ASSERT_EQUAL(exception_status,1);

  //Wrong allocation params
  try { my_malloc(-1); }
  catch(1) {}
  ASSERT_EQUAL(exception_status,1);

  try { my_calloc(-1,-1); }
  catch(1) {}
  ASSERT_EQUAL(exception_status,1);

  set_error_policy(EXIT_ON_ERROR);
}
