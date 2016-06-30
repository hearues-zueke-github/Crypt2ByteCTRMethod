# Crypt2ByteCTRMethod

## Intro
First of all this en/decryption method is inspired by the symmetrical encryption methods like DES, AES etc.
But the algorithm for the encryption is a little other.

### S-Box or the mapping function
For understanding, how the encryption is working, I will make a simple example.
Let say we have a given plain text in 2 Bits:
01 10 01 11 10 01 11 00

The idea of the encryption is to take always two of the given plain text code (in this example the code has the
length of 2 Bits). We will see later, what this means.

To can encryption we need a function where the input will give something else in the output (this is called a s-box,
but with this algorithm it will be a normal function). So let say we have an input x and a function f(x). When we apply
some input to the function f, it will give an output y. It means, that y = f(x). This function must be something special. 
The following property should be true: f^-1(f(x)) = x. The f^-1 is the inverse function of f. This means that when you
get an output y with applying f and x, then you should get x with applying f^-1 and y.

To get a feeling with this, let have a look at the following table:

| x    | f(x) |
| ---    | --- |   Because our plaintext has 2 bits per character, we will need a function which will take 4 bits.
| 0000 | 1100 |
| 0001 | 0000 |
| 0010 | 0101 |
| 0011 | 1111 |
| 0100 | 1011 |
| 0111 | 1000 |
| 0110 | 1101 |
| 0101 | 1010 |
| 1010 | 0100 |
| 1011 | 1001 |
| 1000 | 0111 |
| 1011 | 0011 |
| 1110 | 0010 |
| 1101 | 1110 |
| 1110 | 0110 |
| 1111 | 0001 |
