#include <iostream>
#include <string> // is this really required? (Seems like it)

#define ULL				unsigned long long // Works only with C++11; Remove one "long" to downgrade
#define debug			if(DO_DEBUG)cout
#define DFUQ			2362363213 // Random constant. Used for "Bad input".
#define MAX_POW2	0x8000000000000000 // 2^63

const bool DO_DEBUG = 0; // Enables/Disables debug output. Should be disabled.

using namespace std;


ULL evaluate_term(string term);
ULL evaluate_product(string term);
ULL strtoULL(string s);
unsigned char charToBase10(char c);
ULL tenToThe(unsigned char power);
char allowed_chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', 'x', '\0'};
//------------------------------------------
ULL nmul(ULL a, ULL b);
ULL nmul_2_n(ULL n, ULL m);
ULL product(ULL *p, unsigned int size);
ULL nproduct(ULL *p, unsigned int size);
ULL nsum(ULL *p, unsigned int size);
unsigned char po2(ULL n, ULL *p);
ULL bp2(ULL n);
ULL pseudo_log2(ULL n);
bool is_fp(ULL n);
void print_ULL_array(ULL l[], unsigned int size);
void clear_list(ULL *p, unsigned int size);
unsigned int in_array(ULL n, ULL *p, unsigned int size);
bool elem(char c, char *p, unsigned int size);

int main()
{	
	cout	<<	"NIM multiplication and addition\n"
				<<	"-------------------------------\n"
				<<	"Numbers are in base 10, use \"+\" for NIM-Addition (XOR)\n"
				<<	"and \"x\" for NIM-Multiplication. Parathesises aren't supported (yet)\n"
				<<	"Example: \"8x8+2x4x16\"\n"
				<<	"Type \"exit\" to exit\n"
				<< endl;
	
	string currTerm;
	ULL result;
	while(1)
	{
		cout << ">>";
		cin >> currTerm;
		if(currTerm == "exit")
		{
			return 0;
		}
		result = evaluate_term(currTerm);
		if(result == DFUQ)
		{
			cout << "Invalid term!" << endl;
			continue;
		}
		cout << "= " << result << endl;
	}
	
	return 0;
}

//----------------------------------------------------------------------------------------------
//  TERM HANDELING
//----------------------------------------------------------------------------------------------

ULL evaluate_term(string term)
{
	for(unsigned int i = 0; i < term.length(); i++)
	{
		if(!elem(term[i], allowed_chars, 13))
		{
			debug<<"Invalid input term"<<endl;
			return DFUQ;
		}
	}
	
	string currentTerm;
	ULL result = 0;
	for(unsigned int i = 0; i < term.length(); i++)
	{
		if(term[i] == '+')
		{
			result ^= evaluate_product(currentTerm);
			currentTerm = "";
		}
		else
		{
			currentTerm += term[i];
		}
	}
	result ^= evaluate_product(currentTerm);
	return result;
}

ULL evaluate_product(string term)
{
	ULL result = 1;
	string currentNumber;
	for(unsigned int i = 0; i < term.length(); i++)
	{
		if(term[i] == 'x')
		{
			result = nmul(result, strtoULL(currentNumber));
			currentNumber = "";
		}
		else
		{
			currentNumber += term[i];
		}
	}
	result = nmul(result, strtoULL(currentNumber));
	return result;
}

ULL strtoULL(string s)
{
	ULL result = 0;
	for(unsigned char c = 0 ; c < s.length(); c++)
	{
		result += charToBase10(s[s.length()-1-c]) * tenToThe(c);
	}
	return result;
}

unsigned char charToBase10(char c)
{
	return c - 48;
}

ULL tenToThe(unsigned char power)
{
	ULL result = 1;
	unsigned char c = 0;
	while(c < power)
	{
		result *= 10;
		c++;
	}
	return result;
}

//----------------------------------------------------------------------------------------------
//  NIM MULTIPLICATION
//----------------------------------------------------------------------------------------------


ULL nmul(ULL a, ULL b)
{
	debug<<"\n+NMUL  ["<<a<<", "<<b<<"]"<<endl;
	//	0 (x) n
	if( !a || !b )
	{
		debug<<"+-ZERO"<<endl;
		return 0;
	}
	
	// 1 (x) b
	if( a == 1 ) {debug<<"+-B"<<endl;return b;}
	if( b == 1 ) {debug<<"+-A"<<endl;return a;}
	
	//	FP (x) FP
	if( is_fp(a) && is_fp(b))
	{
		debug<<"++FP"<<endl;
		// a == b
		if(a == b)
		{
			debug<<" +-SQUARE"<<endl;
			return 3 * a / 2;
		}// WHY C++, WHY DO YOU ALLOW BRACKETS TO BE MISSING FOR A ONE-LINER! TWO FUCKING CHARACTERS! Compiling works fine, Runs fine, returns wrong result. Infuriating!
		
		return a * b;
	}
	
	// 2^n (x) b || a (x) 2^n
	
	if(bp2(a) == a && bp2(b) == b)
	{
		debug<<"++AB2n"<<endl;
		return nmul_2_n(a,b);
	}
	
	// a (x) b
	debug<<"+-A(X)B"<<endl;
	ULL po2A[64],po2B[64];
	clear_list(po2A,64);
	clear_list(po2B,64);
	ULL result = 0;
	unsigned int sizeA,sizeB;
	sizeA = po2(a,po2A);
	sizeB = po2(b,po2B);
	for(unsigned int i = 0; i < sizeA; i++)
	{
		for(unsigned int j = 0; j < sizeB; j++)
		{
			result ^= nmul(po2A[i],po2B[j]);
		}
	}
	return result;
}

ULL nmul_2_n(ULL n, ULL m)
{
	debug<<"  NMUL_2_N  ["<<n<<", "<<m<<"]"<<endl;
	ULL logN = pseudo_log2(n);
	ULL logM = pseudo_log2(m);
	ULL po2LogN[6],po2LogM[6];
	clear_list(po2LogN,6);
	clear_list(po2LogM,6);
	po2(logN,po2LogN);
	po2(logM,po2LogM);
	for(unsigned int i = 0; i < 6; i++)
	{
		po2LogN[i] = 1<<po2LogN[i];
		po2LogM[i] = 1<<po2LogM[i];
	}
	ULL fps_products[5];
	bool diff_fpow = 1;
	for(unsigned char c = 0; c < 5; c++)
	{
		unsigned int count = in_array(1<<(1<<c),po2LogN,6);
		count += in_array(1<<(1<<c),po2LogM,6);
		if(count > 1) diff_fpow = 0;
		fps_products[c] = 1;
		for(unsigned int i = 0; i < count; i++)
		{
			fps_products[c] = nmul(fps_products[c], 1<<(1<<c));
		}
	}
	if(diff_fpow)return product(fps_products,5);
	else return nproduct(fps_products,5);
}

ULL nproduct(ULL *p, unsigned int size)
{
	ULL result = 1;
	for(unsigned int i = 0; i < size; i++)
	{
		result = nmul(result,p[i]);
	}
	return result;
}

ULL nsum(ULL *p, unsigned int size)
{
	ULL result = 0;
	for(unsigned int i = 0; i < size; i++)
	{
		result ^= p[i];
	}
	return result;
}

ULL product(ULL *p, unsigned int size)
{
	ULL result = 1;
	for(unsigned int i = 0; i < size; i++)
	{
		result *= p[i];
	}
	return result;
}

unsigned char po2(ULL n, ULL *p) // Provided array should at least be of size ceiling(log2(max_n))
{
	debug<<"  PO2  ["<<n<<"]"<<endl;
	unsigned char i = 0;
	ULL c = 1;
	do
	{
//		debug << c << endl;
		if(n & c)
		{
			p[i] = c;
			i++;
		}
		c*=2;
	}while(c);
	return i;
}

ULL bp2(ULL n)
{
	debug<<"  BP2  ["<<n<<"]"<<endl;
	ULL c = MAX_POW2; // 2^63 -- TODO: probably not the best way, throws warning; Don't care
	if(n & c) return c;
	do
	{
		c/=2;
		if(n & c) return c;
	}while(c != 1);
	return 0;
}

ULL pseudo_log2(ULL n)
{
	debug<<"  PSEUDO_LOG  ["<<n<<"]"<<endl;
	// Not really log2, but will only be used if a = 2^n anyway. Allows me to get away without any dependecies (other than output)
	unsigned char i = 0;
	ULL c = 1;
	do
	{
//		debug << c << endl;
		if(n & c)
		{
			return i;
		}
		i++;
		c*=2;
	}while(c);
	return DFUQ;
}

bool is_fp(ULL n)
{
	return (n == 1 || n == 2 || n == 4 || n == 16 || n == 256 || n == 65536 || n == 4294967296);
}

void print_ULL_array(ULL l[], unsigned int size)
{
	for(unsigned int i = 0; i < size; i++)
	{
		cout << l[i] << ", ";
	}
	cout << endl;
}

void clear_list(ULL *p, unsigned int size)
{
	for(unsigned int i = 0; i < size; i++)
	{
		p[i]=0;
	}
}

unsigned int in_array(ULL n, ULL *p, unsigned int size)
{
	unsigned int r = 0;
	for(unsigned int i = 0; i < size; i++)
	{
		if(p[i] == n) r++;
	}
	return r;
}

bool elem(char c, char *p, unsigned int size)
{
	for(unsigned int i = 0; i < size; i++)
	{
		if(p[i] == c) return true;
	}
	return false;
}
