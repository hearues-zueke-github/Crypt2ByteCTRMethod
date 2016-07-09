#include <iostream>

using namespace std;

int num(int a) {
  cout<<"a = "<<a<<endl;
  return a;
}

bool less(int a, int b) { return a < b; }

int main(int argc, char* argv[]) {
  int a = 5;
  for (int i = 0; i < a; i++) {
    cout<<"i = "<<i<<endl;
  }

  int c = 1;
  int b[] = {1,2,3};
  cout<<"b[1] = "<<b[1]<<endl;
  cout<<"1[b] = "<<1[b]<<endl;
  cout<<"*(b+1) = "<<*(b+1)<<endl;
  cout << "Hello, World!" << endl;

  return 0;
}
