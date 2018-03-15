#include <stdio.h>
#include <stdlib.h>
#include <glpk.h>
#include <math.h>
#define D2R 3.14159265358979323846/180

typedef struct{
  int n;
  int *c;
  int **a;
}Data;

float calc_dist(int n, float lat1, float lng1, float lat2, float lng2){
  float alpha;
  float dist[n][n];

  alpha = sin(D2R*(lat1-lat2)/2)**2 + cos(D2R*lat1)*cos(D2R*lat2)*sin(D2r*(lng1-lng2)/2)**2;
  dist = 2*6371*atan2(sqrt(alpha), sqrt(1-alpha));
  return dist;

}

Data le_arquivo(char f_nome){
  Data data;
  FILE *f = fopen(f_nome,"r");
  fscanf(f,"%d",&data.n);
  data.c = (int*) malloc (data.n * sizeof(int));
  data.a = (int**) malloc (data.n*sizeof(int*));
  for(int i=0; i < data.n; i++){
    data.a[i] = (int*) malloc (data.n*sizeof(int));
  }
  float *lat = (float*) malloc (data.n * sizeof(float));
  float *lng = (float*) malloc (data.n * sizeof(float));

  for(int i=0; i < data.n;i++){
    fscanf(f,"%d ",&data.c[i]);
  }
  for(int i=0; i <data.n; i++){
    fscanf(f,"%f,%f ",&lat[i],&lng[i]);
  }
  for(int i=0; i < data.n; i++){
    for(int j = 0; j < data.n; j++){
      data.a[i][j] = calc_dist(data.n,lat[i],lng[i], lat[j],lng[j]);
    }
  }
  free(lat);
  free(lng);
  return data;
}

int main(void)
{
  /* declare variables */
  glp_prob *lp;
  int ia[1+1000], ja[1+1000];
  char f_nome[]= "sv.dat";
  Data data = le_arquivo(f);
  int *x = (int*) malloc ((data.n+1) * sizeof(int));
  double ar[1+1000], z;
  int aux;
  /* create problem */
  lp = glp_create_prob();
  glp_set_prob_name(lp, "set_covering");
  glp_set_obj_dir(lp, GLP_MIN);
  /* fill problem */
  glp_add_rows(lp, data.n);
  for (int aux=1; aux <= data.n; aux++){
    //glp_set_row_name(lp, aux, "r%d");
    glp_set_row_bnds(lp, aux, GLP_LO,1.0,0.0);
  }
  glp_add_cols(lp,data.n);
  for (int aux=1; aux <= data.n; aux++){
    //glp_set_col_name(lp, aux, "x%d");
    glp_set_col_kind(lp, aux, GLP_BV);
  }
  for(int aux=1; aux <= data.n; aux++){
    glp_set_obj_coef(lp,aux,data.c[aux]);
  }
  for(int i=1; i<= data.n;i++){
    for(int j=1; j<=data.n;j++){
      ia[aux]=i, ja[aux]=j , ar[aux]=data.a[i-1][j-1];
      aux++;
    }
  }

  glp_load_matrix(lp, aux, ia, ja, ar);
  /* solve problem */
  glp_intopt(lp, NULL);
  /* recover and display results */
  z = glp_get_obj_val(lp);
  printf("função objetivo: %g\n",z);
  for(int i=1; i<=data.n;i++){
    x[i] = glp_get_col_prim(lp,i);
    printf("x%d = %d,  ",i,x[i]);
  }

  /* housekeeping */
  glp_delete_prob(lp);
  glp_free_env();
  free(x);
  return 0;
}
