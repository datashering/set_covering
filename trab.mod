set L;

param R; # raio de cobertura 
param C{i in L};	#custo de instalacao
param lat{i in L};   #latitude 
param lng{j in L};   #longitude

param d2r := 3.1415926/180;
param alpha{i in L, j in L} := sin(d2r*(lat[i] - lat[j])/2)**2 + cos (d2r*lat[i])*cos(d2r*lat[j])*sin(d2r*(lng[i]-lng[j])/2)**2;
param d {i in L, j in L} := 2*6371* atan2(sqrt(alpha[i,j]),sqrt(1-alpha[i,j])); 
param a{i in L,j in L} := (if(R >= d[i,j]) then 1 else 0);

var x{i in L}, binary;

minimize custo: sum{i in L} x[i] * C[i];

s.t. inst{j in L}: sum{i in L} a[i,j] * x[i] >= 1;
end;