#include<iostream>
#include<cmath>
#include<cstdlib>
#define max 100

using namespace std;
long int n;   // n is the length of the input vectors (i.e. the ID)
long int q;   // input vectors are choosen from Z_q
long int N;   // N is the number of IDs (i.e. the leaves of the Tree)
long int k;   // k is the number of binary digits for the reprsentation of each elts of Z_q
long int l;   // l is the level of the Tree
long int nk;

// We need lattice based hash function, which is a matrix.
// Also we considers the n length vectors as matrix.
// So we define the matrix data structure
struct matrix{
 int r,c;
 int M[max][max];
};

matrix A0,A1;
matrix witness[max];  // We need a array of matrix to get the witness for the signing ID
matrix vmat[max];     // During verification we need this array of matrix


// Creating a "input" function to input the ID vectors and any other necessary matrix
matrix input(int row, int col)
{
  matrix A;
  if (row > max || col > max) 
  {
    cout<< "Error: Row or column size exceeds max (" << max << ").\n";
    exit(1);
  }
  A.r=row;
  A.c=col;
  for(int i=0;i<row;i++)
  {
    for(int j=0;j<col;j++)
    {
      cin>>A.M[i][j];
    }
  }         
  return A;
}


// Creating a "print" function to print matrix
void print(matrix A)
{ 
  int row=A.r, col=A.c;
  cout<<"The matrix is  ";
  for(int i=0;i<row;i++)
  {
    cout<<"\n";
    for(int j=0;j<col;j++)
    cout<<A.M[i][j]<<"\t";
  }
}


// In the construction of hash function, h_A=[A0|A1] we need to concatenate two matrices
// So we creat the "append" function
matrix append(matrix A,matrix B)
{
  matrix C;
  C.r=A.r;
  C.c=A.c+B.c;

  for(int i=0;i<A.r;i++)
  {
    cout<<"\n";
    for(int j=0;j<A.c;j++)
    {
      C.M[i][j]= A.M[i][j];
    } 
    for(int j=A.c; j<(A.c+B.c); j++)
    {
      C.M[i][j]= B.M[i][j];
    }
  }
  return C;
}


// Hash function is a matrix of dim (n x nk), created using random numbers from Z_q
// So we creat "randominput" function to generate a random (n x nk) matrix with elements from Z_q
matrix randominput()   
{
  matrix A;
  A.r=n;
  A.c=nk;

  for(int i=0;i<n;i++)
  {
    for(int j=0;j<nk;j++)
    {
      A.M[i][j]=rand()%q;
    }
  }  
  return A;
} 


// Before generating witness for an ID, first need to check whether it belongs to the ring or not
// This "equal" function checks whether the input ID is equal with some ID in the ring or not
// If input ID lies in the ring the function outputs "true" and proceed for generation of witness
bool equal( matrix A, matrix B) 
{
  if (A.r != B.r) { return false; }
  if (A.c != B.c) { return false; }

  for (int i = 0; i < A.r; i++) 
  {
    for (int j = 0; j < A.c; j++) 
    {
      if (A.M[i][j] != B.M[i][j]) 
      {
        return false; 
      }
    }
  }
  return true;
}


// Hash function takes two binary strings as input and outputa a binary string
// IDs (i.e. n-dim vectors) are converted into binary string using this "make_binary" function
matrix make_binary(matrix A)
{
  matrix BinA;
  BinA.r=nk;
  BinA.c=1;
  for(int i=1;i<=n;i++) 
  {
    int p=A.M[i-1][0];
    for(int j=i*k-1;j>=(i-1)*k;j--)
    {
      BinA.M[j][0]=p%2;
      p=floor(p/2);
    }
  }     
  return BinA; 
}


// Hash function requires matrix multiplication
// So we create this "multiply" function
matrix multiply(matrix m1, matrix m2)
{
  matrix Mult;
  for(int i=0;i<m1.r;i++)
  { 
    for(int j=0;j<m2.c;j++)
    {
      Mult.M[i][j]=0;
    } 
  }
  Mult.r=m1.r;
  Mult.c=m2.c;
  


  if(m1.c==m2.r)
  {
    for(int i=0;i<m1.r;i++)
    {
      for(int j=0;j<m2.c;j++)
      {
        Mult.M[i][j]=0;
        for(int k=0;k<m1.c;k++)
        {
          Mult.M[i][j]=(Mult.M[i][j] +((m1.M[i][k])*(m2.M[k][j])))%q;
        }        
      }
    } 
  }
  else
  {
    cout<<"\n matrix multiplication not possible  "<<m2.r<<"\t"<<m2.c<<endl;
    cout<<m1.r<<"\t"<<m1.c<<endl;
  }

  return Mult;         
}


// In the hash function we are required to add two matices
matrix add(matrix m1,matrix m2)
{
  matrix Add;
  if((m1.r!=m2.r)||(m1.c!=m2.c))
  {
    cout<<"\n matrix addition not possible \n";
  }
  else
  {   
    for(int i=0;i<m1.r;i++)
    {
      for(int j=0;j<m1.c;j++)
      {
        Add.M[i][j]=(m1.M[i][j]+m2.M[i][j])%q;
      }
    }
  }

  return Add;
}


// We have N number of IDs and need to accumulate them using hash function, h_A=[A0|A1]
// So we creat this "hash_root" function and with fixed A0 & A1 at each level we use h_A
// At first we take the all IDs in an array of matrices
// And use the array T[temp] to store hashed values at each level of the tree
matrix hash_root( matrix d[], matrix A0, matrix A1)
{
  matrix z1,z2,z3,u0;
  long int temp=N;
  matrix T[temp];
  
  for(int i=l;i>1;i--)   // i is level here
  {
    for(int j=0;j<temp;j++)  //j is the leaf-nodes here in each level
    {
      T[j]=d[j];
    }

    for(int j=0;j<temp;j=j+2)
    {
      z1=multiply(A0,T[j]);  
      z2=multiply(A1,T[j+1]);
      z3=add(z1,z2); 
      int j1=j/2;
      d[j1]=make_binary(z3);     
    }

    temp=temp/2;
  }

  z1=multiply(A0,d[0]);
  z2=multiply(A1,d[1]);
  z3=add(z1,z2);         // Root of the tree in n-dim vector form
  u0=make_binary(z3);    // Binary form of the root
  u0.r=nk;
  u0.c=1;
  cout<<"accumulated value in "<<n<<" length vec is\n";
  print(z3);  
  cout<<endl;

  return u0; 
}



int main()
{
  cout<<"choose security parameter n \n";
  cin>> n;
  cout<<"\nchoose integer q for modulo opeation\n";
  cin>> q;
  cout<<"\nenter number of tree leaves N \n"; // Since it's a binary tree, enter value of N as 2^l
  cin>> N;
  
  l=ceil(log(N)/log(2));
  k=ceil(log(q)/log(2));
  nk=n*k;
  cout<<"l="<<l<<", k="<<k<<", nk="<<nk<<"\n"; 

  //crating random matrices A0, A1 for the hash function
  matrix A0=randominput();  
  matrix A1=randominput();

  print(A0);
  cout<<endl;
  print(A1);
  cout<<endl;

  matrix e[N],d[N],f[N];

  cout<<"Enter "<<N<<" number of IDs which are elts of (Z_q)^n"<<endl;
  for(int i=0; i<N;i++)
  {
    cout<<"Enter "<<i<<"th ID : "<<endl;
    e[i]=input(n,1);
    cout<<endl;
  }

  for(int i=0; i<N;i++)
  {
    d[i]=make_binary(e[i]);
  }

  // As we will be working on the array d[], we needed an array f[] to store initial IDs 
  for(int i=0; i<N; i++)
  {
    f[i]=d[i];          
  }

  matrix u;
  u=hash_root(d,A0,A1);
  cout<<"Acumullated value u in binary form is ";
  print(u);
  

  // Generation of witness for a particular ID
  int g=0;
  cout<<"\n if you want to get witness then press 1\n";  
  cin>>g;             

  if(g==1)
  {
    cout<<"\n enter any ID D, a "<<n<<" length vector to get witness \n";
    matrix D, E; 
    D=input(n,1);
    E=make_binary(D);

    cout<<"\n print E (binary form of given Id) "<<endl;
    print(E);

    int index = -1;
    for(int i=0; i<N; i++)
    {
      if(equal(f[i],E))
      {
        index = i;    
        cout<<"\n Index of given ID is "<<index<<endl;
        break;
      }
    }

    if(index == -1)
    {
      cout<<"\n Id is not present in the list"<<endl;
    }

    if(index!= -1)
    {
      int int0, int1=index;
      matrix z1,z2,z3;
      matrix T[N];
      long int temp=N;
      int s=1;

      //there will be l number of u_[j] elts in the witness array
      for(int i=1; i<l; i++)   
      {
        if(int1%2==0)
        {
          int0= int1 + 1; 
        }
        else
        {
          int0= int1 - 1; 
        }
        witness[s] =  f[int0];   // witness[s] will be (nk x 1) column vector
        s++;
        for(int j=0;j<temp;j++) 
        {
          T[j]=f[j];
        }
        
        for(int j=0;j<temp;j=j+2)
        {
          z1=multiply(A0,T[j]);
          z2=multiply(A1,T[j+1]);
          z3=add(z1,z2);
          f[j/2]=make_binary(z3);
        }
        int1=floor(int1/2);
        temp=temp/2;
      }

      if(int1%2==0)
      {int0=int1+1;}  
      else
      {int0=int1-1;}
      witness[l]=f[int0];

      // witness[0] is binary representation of the index of the given ID
      // So it's a (l x 1) length vector
      witness[0].r = l;
      witness[0].c = 1;     

      for(int i=l-1; i>=0; i--)
      {
        witness[0].M[i][0] = index%2;
        index = floor(index/2);
      }

      for(int i=0; i<l+1; i++)
      {
        cout<<"\n witness["<<i<<"] is "<<endl;
        print(witness[i]);
      }
    }


    // This is the verification part.
    // Here we check whether for an given witness there is any valid ID in the ring or not.
    // In actual signature algorithm, there is a signature in the form of proof constructed
    // using message, secret key, signing ID, witness and accumulated value. The verify algorithm
    // verifies this proof and confirms the validity of signature.
    // But here we only check the validity of given witness.
    cout<<"\n to check whether your data id belongs to data set or not press 1\n";
    int h=0;
    cin>>h;

    // Inputing the witness for the verification
    matrix w[100]; 
    if(h==1)
    {
      w[0].r=l;
      w[0].c=1;
      for(int j=1;j<=l;j++)
      {
        w[j].r=nk;
        w[j].c=1;
      }
      cout<<"enter your witness\n";
      cout<<"enter first "<<l<<" bits\n";
      for(int i=0;i<l;i++) 
      {
        cin>>w[0].M[i][0]; 
      }
      for(int i=1;i<=l;i++) 
      {
        cout<<"enter "<<i<<"th "<<nk<<" bits of witness\n";
        for(int j=0;j<nk;j++)
        {
          cin>>w[i].M[j][0];     
        }
      }
    }

    vmat[l]=E;

    for(int i=l-1;i>=0;i--)
    {
      if(witness[0].M[i][0]==0)
      {
        vmat[i]=make_binary(add(multiply(A0,vmat[i+1]), multiply(A1,w[l-i]))); 
      }
      else
      {
        vmat[i]=make_binary(add(multiply(A0,w[l-i]), multiply(A1,vmat[i+1])));   
      }
    }

    cout<<"\n Print vmat[0]  "<<endl;
    print(vmat[0]);
    
    if(equal(vmat[0],u))
    {
      cout<<"\n Your ID is in data set \n";
    }
    else
    {
      cout<<"\n Your ID is not in data set \n";
    }
  }

  return 0;
}