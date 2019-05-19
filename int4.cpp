#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include <ctype.h>
#include <cstdlib>
#include <iterator>
#include <stack>
#include <algorithm>
using namespace std;

enum type_of_lex {
		LEX_NULL, // 0
		LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_BREAK, LEX_DO, LEX_ELSE, LEX_FOR,// 7
		LEX_GOTO, LEX_IF, LEX_INT, LEX_FALSE, LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ,// 15
		LEX_STRING, LEX_STRUCT, LEX_THEN, LEX_TRUE, LEX_WHILE, LEX_WRITE,// 21
		LEX_FIN,// 22
		LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ,// 29
		LEX_LSS, LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, // 37
		LEX_GEQ, LEX_LFB, LEX_RFB,// 40
		LEX_NUM,// 41
		LEX_ID,//42
		LEX_STR,//43
		POLIZ_LABEL,//44
		POLIZ_ADDRESS,//45
		POLIZ_GO,//47
		POLIZ_FGO,//48
		LEX_LABEL,
		POLIZ_GOTO
};

//=======================================  class LEX  ==========================================
class Lex
{
	type_of_lex t_lex;
	int 		v_lex;
public:
	Lex(type_of_lex t = LEX_NULL, int v = 0){ t_lex = t; v_lex = v;}
	Lex(const Lex & ob)
	{
		t_lex = ob.t_lex;
		v_lex = ob.v_lex;
	}
	Lex& operator=(const Lex &l)
	{
		if(this == &l)
			return *this;
		t_lex = l.t_lex;
		v_lex = l.v_lex;
		return *this;
	}
	type_of_lex get_type() const{ return t_lex;}
	int get_value() const{ return v_lex;}
	friend ostream& operator <<( ostream &s, Lex l);
};

//=======================================  class Ident  ========================================
class Ident
{
	string name;
	bool declare;
	type_of_lex type;
	bool assign;
	int value;
public:
	Ident(){ declare = false; assign = false;}
	Ident(const string & n){
		name = n;
		declare = false;
		assign = false;
	}
	bool operator==(const string& s)const{ return name == s;}
	string get_name() const{ return name;}
	bool get_declare() const{ return declare;}
	void put_declare(){ declare = true;}
	type_of_lex get_type() const{ return type;}
	void put_type(type_of_lex t){ type = t;}
	bool get_assign() const{ return assign;}
	void put_assign(){ assign = true;}
	int get_value() const{ return value;}
	void put_value(int v){ value = v;}
};

//=======================================  Table Ident  ========================================
vector <Ident> TID;
int put(const string & buf)
{
	vector <Ident>:: iterator k;

	if((k = find(TID.begin(), TID.end(), buf)) != TID.end())
		return k - TID.begin();
	TID.push_back(Ident(buf));
	return TID.size() - 1;
}

vector<string> TSTR;

//======================================  class LABEL  =========================================
class Label
{
	string name;
	bool declared;
	int label;
public:
	Label(){ declared = false; label = -1;}
	Label(const string &s)
	{
		name = s;
		declared = false;
		label = -1;
	}
	bool operator==(const string &s) const{ return name == s;}
	bool get_declared() const{ return declared;}
	void set_declared() { declared = true;}
	void set_label(int l) { label = l;}
	int get_label() const{ return label;}
};

vector <Label> TLB;
int put_label(const string & buf)
{
	vector <Label>:: iterator k;

	if((k = find(TLB.begin(), TLB.end(), buf)) != TLB.end())
		return k - TLB.begin();
	TLB.push_back(Label(buf));
	return TLB.size() - 1;
}

//=======================================  class Scanner  ======================================
class Scanner
{
	FILE *fp;
	int c;
	int look(const string & buf, const char **list)
	{
		int i = 1;
		while(list[i])
		{
			if(buf == list[i])
				return i;
			i++;
		}
		return 0;
	}
	static type_of_lex words[];
	static type_of_lex dlms[];
	void gc(){ c = fgetc(fp);}
public:
	static const char *TW[], *TD[];
	Scanner(const char *program){ fp = fopen(program, "r");}
	Lex get_lex();
};

//=========================================  TD и TW ==========================================
const char *
Scanner::TW[] = 
{
	NULL, "and", "begin", "bool", "break", "do", "else", "for", "goto", "if", "int",
	"false", "not", "or", "program", "read", "string", "struct", "then", "true",
	"while", "write", NULL
};

const char *
Scanner::TD[] = 
{
	NULL, ";" , ",", ":", "=", "(", ")", "==", "<", ">", "+", "-", "*", "/", "<=", "!=", ">=",
	"{", "}", NULL
};

type_of_lex Scanner::words[] =
{
	LEX_NULL, // 0
	LEX_AND, LEX_BEGIN, LEX_BOOL, LEX_BREAK, LEX_DO, LEX_ELSE, LEX_FOR,// 7
	LEX_GOTO, LEX_IF, LEX_INT, LEX_FALSE, LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ,// 15
	LEX_STRING, LEX_STRUCT, LEX_THEN, LEX_TRUE, LEX_WHILE, LEX_WRITE,// 21
	LEX_NULL
};

type_of_lex Scanner::dlms[] = 
{
	LEX_NULL,// 
	LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ,// 7
	LEX_LSS, LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, // 15
	LEX_GEQ, LEX_LFB, LEX_RFB,// 18
	LEX_NULL
};
//-----------------------------------------  operator << --------------------------------------
ostream& operator <<( ostream &s, Lex l)
{
	string t;
	if (l.t_lex < LEX_FIN)
		t = Scanner::TW[l.t_lex];
	else if (l.t_lex > LEX_FIN && l.t_lex <= LEX_RFB)
      t = Scanner::TD[l.t_lex-LEX_FIN];
    else if (l.t_lex == LEX_NUM)
        t = "NUMB";
    else if (l.t_lex == LEX_STR) {
		s << '(' << "STR" << ',' << TSTR[l.v_lex] << ");";
		return s;
	}	
    else if (l.t_lex == LEX_ID)
          t = TID[l.v_lex].get_name(); 
    else if (l.t_lex == POLIZ_LABEL)
          t = "POLIZ_Label";
    else if(l.t_lex == POLIZ_ADDRESS)
          t = "Addr";
    else if (l.t_lex == POLIZ_GO)
          t = "!";
    else if (l.t_lex == POLIZ_FGO) 
          t = "!F";
   	
    s << '(' << t << ',' << l.v_lex << ");";
    return s;
}

//=========================================  GET_LEX() =========================================
Lex Scanner::get_lex()
{
	enum state{H, IDENT, NUMB, COM, CC1, ALE, NEQ, STR};// CC2 убрали
	state CS = H;
	string buf;
	int d,j;
	do
	{
		gc();
		switch(CS){
			case H:
				if( c == ' ' || c == '\n' || c == '\t' || c == '\r')
					;
				else if(isalpha(c))
				{
					buf.push_back(c);
					CS = IDENT;
				}
				else if(isdigit(c))
				{
					d = c - '0';
					CS = NUMB;
				}
				else if(c == '/') 
					CS = CC1;
				else if(c == '>' || c == '<' || c == '=')
				{
					buf.push_back(c);
					CS = ALE;
				}
				else if(c == EOF)
					return Lex(LEX_FIN);
				else if(c == '!')
				{
					buf.push_back(c);
					CS = NEQ;
				}
				else if(c == '"')
					CS = STR;
				else//  DELIM
				{
					buf.push_back(c);
					if((j = look(buf,TD)))
						return Lex(dlms[j], j);// Lex((type_of_lex)(j+(int)LEX_FIN), j);//
					else
						throw c;
				}
				break;//end H
			case IDENT:
				if(isalpha(c) || isdigit(c))
					buf.push_back(c);
				else
				{
					ungetc(c, fp);
					if((j = look(buf,TW)))
						return Lex(words[j], j);// Lex((type_of_lex)j,j);//
					else
					{
						j = put(buf);
						return Lex(LEX_ID, j);
					}
				}
				break;// end IDENT
			case NUMB:
				if(isdigit(c))
				{
					d = d * 10 + c - '0';
				}	
				else
				{
					ungetc(c, fp);
					return Lex(LEX_NUM,d);
				}
				break;
			case CC1:
				if(c == '*')
					CS = COM;
				else
				{
					ungetc(c,fp);
					buf.push_back('/');
					if((j = look(buf,TD)))
						return Lex(dlms[j], j);// Lex((type_of_lex)(j+(int)LEX_FIN), j);//
					else
						throw c;
				}
				break;
			case COM:
				if(c == '*')
				{
					gc();
					if(c == '/')
						CS = H;
					else
						break;
				}
				else if(c == EOF)
					throw c;
				break;
			case ALE:
				if(c == '=')
				{
					buf.push_back(c);
					j = look(buf,TD);
					return Lex(dlms[j],j);// Lex((type_of_lex)(j+(int)LEX_FIN),j);//
				}
				else
				{
					ungetc(c,fp);
					j = look(buf,TD);
					return Lex(dlms[j],j);// Lex((type_of_lex)(j+(int)LEX_FIN),j);//
				}
				break;
			case NEQ:
				if(c == '=')
				{
					buf.push_back(c);
					j = look(buf, TD);
					return Lex(LEX_NEQ, j);
				}
				else
					throw '!';
				break;
			case STR:
				if(c != '"')
					if(c != EOF)
						buf.push_back(c);
					else
						throw c;
				else
				{
					j = TSTR.size();
					TSTR.push_back(buf);
					return Lex(LEX_STR,j);
				}
				break;
		}// end switch
	}while(true);
}

//=========================================  class Stack =======================================
template <class T, int max_size>
class Stack
{
	T s[max_size];
	int top;
public:
	Stack(){ top = 0;}
	void reset(){ top = 0;}
	void push(T i);
	T pop();
	bool is_empty(){ return top == 0;}
	bool is_full(){ return top == max_size;}
};
//------------------------------------- Stack:: push() -----------------------------------------
template <class T, int max_size>
void Stack<T,max_size>::push(T i)
{
	if(!is_full())
	{
		s[top] = i;
		++top;
	}
	else
		throw "Stack_is_full";
}
//-------------------------------------  Stack:: pop() -----------------------------------------
template <class T, int max_size>
T Stack<T,max_size>::pop()
{
	if(!is_empty())
	{
		--top;
		return s[top];
	}
	else
		throw "Stack_is_empty";
}

//========================================  POLIZ  =============================================
class Poliz
{
	Lex *p;
	int size;
	int free;
public:
	Poliz(int max_size)
	{
		p = new Lex[size = max_size];
		free = 0;
	}
	~Poliz(){ delete []p;}
	void put_lex(Lex l) { p[free] = l; ++free;}
	void  put_lex(Lex l, int place) { p[place] = l;}
	void pop_lex() { --free;}
	void blank() { ++free;}
	int get_free(){ return free;}
	Lex& operator[](int index);
	void print();
};

Lex& Poliz::operator[](int index)
{
	if(index > size)
		throw "POLIZ: out of array";
	else
		if(index > free)
			throw "POLIZ: indefinite element of array";
		else
			return p[index];
}

void Poliz::print()
{
	for(int i = 0; i < free; ++i)
		cout << p[i];
}

//=========================================  PARSER  ===========================================
class Parser
{
	Lex curr_lex;
	type_of_lex c_type;
	int c_val;
	Scanner scan;
	Stack<Lex,1000> st_lex;
	Stack<type_of_lex,1000> st_type_of_lex;
	Lex t_e;
	Stack<int,50> st_label;

	void P(); void D(); void H(); void V(); void IH(); void C();
	void NH(); void R(); void R1(); void E();
	void E1(); void E2(); void E3(); void E4(); void E5(); void E6();
	
	void dec(type_of_lex type);
	void check_id();
	void check_op();
	void check_not();
	void eq_type();
	void eq_bool();
	void check_id_in_read();
	void gl() // ПОЛУЧЕНИЕ ОЧЕРЕДНОЙ ЛЕКСЕМЫ
	{
		curr_lex = scan.get_lex();
		c_type = curr_lex.get_type();
		c_val = curr_lex.get_value();
	}
public:
	Poliz prog;
	Parser(const char* program): scan(program), prog(1000){}
	void analyze();
};

//---------------------------------   check_id_in_read()  --------------------------------------  
void Parser::check_id_in_read()
{
	if( !TID[c_val].get_declare())
		throw "identificator is not declared in READ";
}

//--------------------------------------  eq_bool()  -------------------------------------------
void Parser::eq_bool()
{
	if(st_type_of_lex.pop() != LEX_BOOL)
		throw "expression is not boolean in condition";
}

//--------------------------------------  eq_type()  -------------------------------------------
void Parser::eq_type()
{
	if(st_type_of_lex.pop() != st_type_of_lex.pop())
		throw "wrong types are in = (assign)";
}

//--------------------------------------  check_not()  -----------------------------------------
void Parser::check_not()
{
	if(st_type_of_lex.pop() != LEX_BOOL)
		throw "wrong type is in NOT";
	else
		st_type_of_lex.push(LEX_BOOL);
	prog.put_lex(Lex(LEX_NOT));
}
//--------------------------------------  check_op()  ------------------------------------------
void Parser::check_op()
{
	type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;
	
	t2 = st_type_of_lex.pop();
	op = st_type_of_lex.pop();
	t1 = st_type_of_lex.pop();

	if(op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH)
	{
		r = LEX_INT;
	}
	if(op == LEX_AND || op == LEX_OR)
		t = LEX_BOOL;
	if(t1 == LEX_BOOL && t2 == LEX_BOOL && (op == LEX_EQ || op == LEX_NEQ))
	{
		t = LEX_BOOL;
		r = LEX_BOOL;
	}
	if(t1 == LEX_STRING && t2 == t1 && (op == LEX_EQ || op == LEX_NEQ || 
			op == LEX_LSS || op == LEX_GTR))
	{
		t = LEX_STRING;
		r = LEX_BOOL;
	}
	else
	{
	if(t2 == LEX_STRING && t1 == LEX_STRING && op == LEX_PLUS)
	{
		t = LEX_STRING;
		r = LEX_STRING;
	}
	else
	if(t1 == LEX_STRING && t2 == LEX_STRING )
		throw "wrong operation in using strings";
	}
	if(t1 == t2 && t1 == t)
	{
		st_type_of_lex.push(r);
	}
	else
		throw "wrong types are in operation";
	prog.put_lex(Lex(op));
}

//------------------------------------  check_id()  --------------------------------------------
void Parser::check_id()
{
	if(TID[c_val].get_declare())
		st_type_of_lex.push(TID[c_val].get_type());
	else
		throw "not declared";
}
//------------------------------------  dec(type_of_lex)  --------------------------------------
void Parser::dec(type_of_lex type)
{
	int i;
	while( !st_lex.is_empty())
	{
		i = st_lex.pop().get_value();
		if(TID[i].get_declare())
			throw "twice";
		else
		{
			TID[i].put_declare();
			TID[i].put_type(type);
		}
	}
}

//========================================  ANALYZE() =========================================
void Parser::analyze()
{
	prog.put_lex(Lex(LEX_NUM,0));
	gl();
	P();
	if(c_type != LEX_FIN)
		throw curr_lex;
	prog.print();
	cout << endl << "YES!!!\n";
}
//--------------------------------------- P program{D R}  --------------------------------------
void Parser::P()
{
	if(c_type == LEX_PROGRAM)
	{
		gl();
		if(c_type == LEX_LFB)
		{
			gl();
			D();// описания
			R();//операторы
			if(c_type == LEX_RFB)
			{
				gl();
			}
			else
				throw curr_lex;
		}
		else 
			throw curr_lex;
	}
	else
		throw curr_lex;
}
//----------------------------------------  D ОПИСАНИЯ  ---------------------------------------
void Parser::D()
{
	while(c_type == LEX_INT || c_type == LEX_STRING || c_type == LEX_BOOL)
	{
		int flag_lex;
		switch(c_type){
			case LEX_INT: flag_lex = 0; break;
			case LEX_STRING: flag_lex = 1; break;
			case LEX_BOOL: flag_lex = 2; break;
			default: break;
		}
		st_lex.reset();
		gl();
		V();
		H();
		if(c_type == LEX_SEMICOLON)
		{
			switch(flag_lex){
				case 0: dec(LEX_INT); break;
				case 1: dec(LEX_STRING); break;
				case 2: dec(LEX_BOOL); break;
			}
			gl();
		}
		else
			throw curr_lex;
	}
}

void Parser::H()
{
	if(c_type == LEX_COMMA)
	{
		gl();
		V();
		H();
	}
}

void Parser::V()
{
	if(c_type == LEX_ID)
	{
		st_lex.push(curr_lex);
		int temp_val = c_val;
		gl();
		if(c_type == LEX_ASSIGN)
		{
			prog.put_lex(Lex(POLIZ_ADDRESS,temp_val));
			gl();
			C();
			prog.put_lex(Lex(LEX_SEMICOLON));
		}
	}
}

void Parser::C()
{
	if (c_type == LEX_PLUS || c_type == LEX_MINUS)
	{
		int fl;
		if(c_type == LEX_PLUS)
			fl = 1;
		else
			fl = -1;
		gl();
		if(c_type == LEX_NUM)
		{
			int value = fl * curr_lex.get_value();

			prog.put_lex(Lex( LEX_NUM, value) );
			prog.put_lex(LEX_ASSIGN);
			gl();
		}
		else
			throw curr_lex;
	}
	else 
	if(c_type == LEX_NUM)
	{
		prog.put_lex(curr_lex);
		prog.put_lex(LEX_ASSIGN);
		gl();
	}
	else
	if (c_type == LEX_STR)
	{
		prog.put_lex(curr_lex);
		prog.put_lex(LEX_ASSIGN);
		gl();
	}
	else 
	if (c_type == LEX_TRUE)
	{
		prog.put_lex(Lex(LEX_TRUE,1));
		prog.put_lex(LEX_ASSIGN);
		gl();
	}
	else
	if(c_type == LEX_FALSE)
	{
		prog.put_lex(Lex(LEX_FALSE,0));
		prog.put_lex(LEX_ASSIGN);
		gl();
	}
	else
		throw curr_lex;
}

//======================================== ОПЕРАТОРЫ ========================================
void Parser::R()
{
	if(c_type == LEX_IF || c_type == LEX_WHILE || c_type == LEX_READ || 
		c_type == LEX_WRITE || c_type == LEX_LFB || c_type == LEX_FOR || 
		c_type == LEX_BREAK || c_type == LEX_GOTO || c_type == LEX_ID)
	{
		R1();
		R();
	}
}

void Parser::R1()
{
	int pl0, pl1, pl2, pl3, pl4, pl5;

	if (c_type == LEX_IF)
	{
		gl();
		if(c_type == LEX_LPAREN)
		{
			gl();
			E();
			eq_bool();
			pl2 = prog.get_free();
			prog.blank();
			prog.put_lex(Lex(POLIZ_FGO));
			if(c_type == LEX_RPAREN)
			{
				gl();
				R1();
				pl3 = prog.get_free();
				prog.blank();
				prog.put_lex(Lex(POLIZ_GO));
				prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()),pl2);
				if(c_type == LEX_ELSE)
				{
					gl();
					R1();
					prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()),pl3);
				}
				else
					throw curr_lex;
			}
			else 
				throw curr_lex;
		}
		else 
			throw curr_lex;
	}
	else
	if(c_type == LEX_WHILE)
	{
		st_label.push(-1);
		pl0 = prog.get_free();
		gl();
		if(c_type == LEX_LPAREN)
		{
			gl();
			E();
			eq_bool();
			pl1 = prog.get_free();
			prog.blank();
			prog.put_lex(Lex(POLIZ_FGO));
			if(c_type == LEX_RPAREN)
			{
				gl();
				R1();
				prog.put_lex(Lex(POLIZ_LABEL,pl0));
				prog.put_lex(Lex(POLIZ_GO));
				prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()),pl1);

				
				if(!st_label.is_empty())
				{
					pl5 = st_label.pop();
					if(pl5 != -1)
					{
						prog.put_lex(Lex( POLIZ_LABEL, prog.get_free()), pl5);
						st_label.pop();
					}
				}
			}
			else
				throw curr_lex;
		}
		else
			throw curr_lex;
	}
	else
	if(c_type == LEX_READ)
	{
		gl();
		if(c_type == LEX_LPAREN)
		{
			gl();
			if(c_type == LEX_ID)
			{
				check_id_in_read();
				prog.put_lex(Lex(POLIZ_ADDRESS,c_val));
				gl();
			}
			else
				throw curr_lex;
			if(c_type == LEX_RPAREN)
			{
				prog.put_lex(Lex(LEX_READ));
				gl();
			}
			else
				throw curr_lex;
			if(c_type == LEX_SEMICOLON)
				gl();
			else
				throw curr_lex;
		}
		else
			throw curr_lex;
	}
	else
	if(c_type == LEX_WRITE)
	{
		int cnt_output_elements = 1;
		gl();
		if(c_type == LEX_LPAREN)
		{
			gl();
			E();
			while(c_type == LEX_COMMA)
			{
				++cnt_output_elements;
				gl();
				E();
			}
			if(c_type == LEX_RPAREN)
			{
				prog.put_lex(Lex(LEX_WRITE,cnt_output_elements));
				gl();
			}
			else
				throw curr_lex;
			if(c_type == LEX_SEMICOLON)
				gl();
			else
				throw curr_lex;
		}
		else
			throw curr_lex;
	}
	else 
	if(c_type == LEX_LFB)
	{
		gl();
		R();
		if(c_type == LEX_RFB)
			gl();
		else
			throw curr_lex;
	}
	else 
	if(c_type == LEX_FOR)
	{
		st_label.push(-1);
		gl();
		if(c_type != LEX_LPAREN)
			throw curr_lex;
		gl();
		if(c_type != LEX_SEMICOLON)
		{
			E();
			if(c_type != LEX_SEMICOLON)
				throw curr_lex;
			gl();
			//prog.put_lex(Lex(LEX_SEMICOLON));
		}
		else
			gl();

		if(c_type != LEX_SEMICOLON)
		{
			pl3 = prog.get_free();
			E();
			eq_bool();

			if(c_type != LEX_SEMICOLON)
				throw curr_lex;

			pl1 = prog.get_free();
			prog.blank();
			prog.put_lex(Lex(POLIZ_FGO));
			pl2 = prog.get_free();
			prog.blank();
			prog.put_lex(Lex(POLIZ_GO));
			pl4 = prog.get_free();

			gl();
		}
		else
			gl();
		if(c_type != LEX_RPAREN)
		{
			E();
			if(c_type != LEX_RPAREN)
				throw curr_lex;

			prog.put_lex(Lex( POLIZ_LABEL, pl3));
			prog.put_lex(Lex(POLIZ_GO));
			prog.put_lex(Lex( POLIZ_LABEL, prog.get_free()), pl2);

			gl();
			prog.put_lex(Lex(LEX_SEMICOLON));
		}
		else
		{
			prog.put_lex(Lex( POLIZ_LABEL, pl3));
			prog.put_lex(Lex(POLIZ_GO));
			prog.put_lex(Lex( POLIZ_LABEL, prog.get_free()), pl2);
			gl();
		}
		//prog.put_lex(Lex(LEX_SEMICOLON));
		R1();
		prog.put_lex(Lex(POLIZ_LABEL, pl4));
		prog.put_lex(Lex(POLIZ_GO));
		prog.put_lex(Lex( POLIZ_LABEL, prog.get_free()), pl1);

		if(!st_label.is_empty())
				{
					pl5 = st_label.pop();
					if(pl5 != -1)
					{
						prog.put_lex(Lex( POLIZ_LABEL, prog.get_free()), pl5);
						st_label.pop();
					}
				}
	}
	else 
	if(c_type == LEX_BREAK)
	{
		if(st_label.is_empty())
			throw "break is not in cycle";

		int check_add_label = st_label.pop();
		st_label.push(check_add_label);
		if(check_add_label == -1)
		{
			pl5 = prog.get_free();
			st_label.push(pl5);
			prog.blank();
			prog.put_lex(Lex(POLIZ_GO));
		}
		gl();
		if(c_type == LEX_SEMICOLON)
			gl();
		else
			throw curr_lex;
	}
	else 
	if(c_type == LEX_GOTO)
	{
		gl();
		if(c_type == LEX_ID)
		{	
			string label_name = TID[curr_lex.get_value()].get_name();

			prog.put_lex(Lex( POLIZ_LABEL, put_label(label_name)) );
			prog.put_lex(Lex(POLIZ_GOTO));

			gl();
			if(c_type == LEX_SEMICOLON)
				gl();
			else
				throw curr_lex;
		}
		else
			throw curr_lex;
	}
	else 
	if(c_type == LEX_ID)
	{
		//check_id();
		Lex temp = curr_lex;
		gl();
		
		if(c_type == LEX_COLON) // GOTO
		{
			int i = put_label( TID[temp.get_value()].get_name() );
			if(TLB[i].get_declared())
			{
				throw "GOTO: label has been declared twice";
			}
			TLB[i].set_declared();
			TLB[i].set_label(prog.get_free());
			
			gl();
			R1();
		}
		else 
		if(c_type == LEX_ASSIGN)
		{
			curr_lex = temp;
			c_type = curr_lex.get_type();
			c_val = curr_lex.get_value();

			check_id();
			prog.put_lex(Lex(POLIZ_ADDRESS,c_val));
			gl();
			E();
			eq_type();
			prog.put_lex(Lex(LEX_ASSIGN));
			if(c_type == LEX_SEMICOLON)
			{
				prog.put_lex(Lex(LEX_SEMICOLON));
				gl();
			}
			else
				throw curr_lex;
		}
		else
			throw curr_lex;
	}
	else
		throw curr_lex;
}

//======================================  ВЫРАЖЕНИЕ  ==========================================
void Parser::E()
{
	E1();
	while(c_type == LEX_ASSIGN)
	{
		curr_lex = t_e;
		c_type = curr_lex.get_type();
		c_val = curr_lex.get_value();
		check_id();

		prog.pop_lex();
		prog.put_lex(Lex(POLIZ_ADDRESS,c_val));

		gl();
		E();

		eq_type();
		prog.put_lex(Lex(LEX_ASSIGN));
	}
}

void Parser::E1()
{
	E2();
	while(c_type == LEX_OR)
	{
		st_type_of_lex.push(c_type);
		gl();
		E2();
		check_op();
	}
}

void Parser::E2()
{
	E3();
	while(c_type == LEX_AND)
	{
		st_type_of_lex.push(c_type);
		gl();
		E3();
		check_op();
	}
}

void Parser::E3()
{
	E4();
	if(c_type == LEX_LSS || c_type == LEX_LEQ || c_type == LEX_EQ ||
		c_type == LEX_NEQ || c_type == LEX_GTR || c_type == LEX_GEQ)
	{
		st_type_of_lex.push(c_type);
		gl();
		E4();
		check_op();
	}
}

void Parser::E4()
{
	E5();
	while(c_type == LEX_PLUS || c_type == LEX_MINUS)
	{
		st_type_of_lex.push(c_type);
		gl();
		E5();
		check_op();
	}
}

void Parser::E5()
{
	E6();
	while(c_type == LEX_TIMES || c_type == LEX_SLASH)
	{
		st_type_of_lex.push(c_type);
		gl();
		E6();
		check_op();
	}
}

void Parser::E6()
{
	if(c_type == LEX_ID)
	{
		check_id();
		prog.put_lex(curr_lex);
		t_e = curr_lex;
		gl();
	}
	else
	if(c_type == LEX_TRUE)
	{
		st_type_of_lex.push(LEX_BOOL);
		prog.put_lex(Lex(LEX_TRUE,1));
		gl();
	}
	else
	if(c_type == LEX_FALSE)
	{
		st_type_of_lex.push(LEX_BOOL);
		prog.put_lex(Lex(LEX_FALSE,0));
		gl();
	}
	else
	if(c_type == LEX_NOT)
	{
		gl();
		E6();
		check_not();
	}
	else
	if(c_type == LEX_LPAREN)
	{
		gl();
		E();
		if(c_type == LEX_RPAREN)
			gl();
		else
			throw curr_lex;
	}
	else
	if(c_type == LEX_NUM)
	{
		st_type_of_lex.push(LEX_INT);
		prog.put_lex(curr_lex);
		gl();
	}
	else
	if(c_type == LEX_STR)
	{
		st_type_of_lex.push(LEX_STRING);
		prog.put_lex(curr_lex);
		gl();
	}
	else
	if(c_type == LEX_MINUS)
	{
		st_type_of_lex.push(LEX_INT);
		prog.put_lex(Lex(LEX_NUM, 0));
		st_type_of_lex.push(c_type);
		gl();
		E6();
		check_op();
	}
	else
		throw curr_lex;
}

//========================================  EXECUTER  ==========================================
class Executer
{
	Lex pc_el;
public:
	void execute(Poliz& prog);
};

void Executer::execute(Poliz& prog)
{
	Stack <Lex,1000> args;
	int i, j, index = 0, size = prog.get_free();
	Lex temp, temp2;
	type_of_lex temp_type;

	while(index < size)
	{
		pc_el = prog[index];

		switch(pc_el.get_type())
		{
			case LEX_TRUE:
			case LEX_FALSE:
			case LEX_NUM:
			case LEX_STR:
			case POLIZ_ADDRESS:
			case POLIZ_LABEL:
				args.push(pc_el);
				break;
			case LEX_ID:
				i = pc_el.get_value();
				if(TID[i].get_declare())
				{
					args.push(Lex(TID[i].get_type(), TID[i].get_value()) );
					break;
				}
				else
					throw "POLIZ: indefinite identifier";
			case LEX_NOT:
				args.push(Lex( LEX_NUM, !args.pop().get_value()) );
				break;
			case LEX_OR:
				i = args.pop().get_value();
				args.push(Lex( LEX_NUM, args.pop().get_value() || i) );
				break;
			case LEX_AND:
				i = args.pop().get_value();
				args.push(Lex( LEX_NUM, args.pop().get_value() && i) );
				break;
			case POLIZ_GO:
				index = args.pop().get_value() - 1;
				break;
			case POLIZ_FGO:
				i = args.pop().get_value();
				if(!args.pop().get_value())
					index = i - 1;
				break;
			case LEX_WRITE:
			{
				int count = pc_el.get_value();
				Stack <Lex , 30> out;
				for(int q = 0; q < count; ++q)
					out.push(args.pop());
				
				for(int q = 0; q < count; ++q)
				{
					temp = out.pop();
					temp_type = temp.get_type();
					if(temp_type == LEX_STRING || temp_type == LEX_STR)
						cout << TSTR[temp.get_value()] << endl;
					else
					if(temp_type == LEX_BOOL)
						temp.get_value() == 1 ? cout << "true" << endl :
												cout << "false" << endl;
					else
						cout << temp.get_value() << endl;
				}
				break;
			}
			case LEX_READ:
				{
					int k;
					i = args.pop().get_value();
					if(TID[i].get_type() == LEX_INT)
					{
						cout << "input int value for ";
						cout << TID[i].get_name() << endl;
						cin >> k;
					}
					else
					if(TID[i].get_type() == LEX_BOOL)
					{
						char j[20];
					rep:
						cout << "input boolean value (true or false) for ";
						cout << TID[i].get_name() << endl;
						cin >> j;
						if(!strcmp(j, "true"))
							k = 1;
						else if(!strcmp(j,"false"))
							k = 0;
						else
						{
							cout << "Error in input: true/false\n";
							goto rep;
						}
					}
					else
					if(TID[i].get_type() == LEX_STRING)
					{

						string str;
						cout << "input string for ";
						cout << TID[i].get_name() << endl;
						cin >> str;
						k = TSTR.size();
						TSTR.push_back(str);
					}
				TID[i].put_value(k);
				TID[i].put_assign();
				break;}
			case LEX_PLUS:
				temp2 = args.pop();
				temp_type = temp2.get_type();
				temp = args.pop();
				if(temp_type == LEX_STR || temp_type == LEX_STRING)
				{
					string s1 = TSTR[temp.get_value()], s2 = TSTR[temp2.get_value()];
					s1 = s1 +s2;
					TSTR[temp.get_value()] = s1;
					args.push(Lex( LEX_STR, temp.get_value()) );
				}
				else
					args.push(Lex( LEX_NUM, temp.get_value() + temp2.get_value()) );
				break;
			case LEX_TIMES:
				args.push(Lex( LEX_NUM, args.pop().get_value() * args.pop().get_value()) );
				break;
			case LEX_MINUS:
				i = args.pop().get_value();
				args.push(Lex( LEX_NUM, args.pop().get_value() - i) );
				break;
			case LEX_SLASH:
				i = args.pop().get_value();
				if(i)
					args.push(Lex( LEX_NUM, args.pop().get_value() / i) );
				else
					throw "POLIZ: divide by zero ";
				break;
			case LEX_EQ:
				temp2 = args.pop();
				temp_type = temp2.get_type();
				temp = args.pop();
				if(temp_type == LEX_STR || temp_type == LEX_STRING)
				{
					string s1 = TSTR[temp.get_value()], s2 = TSTR[temp2.get_value()];
					args.push(Lex( LEX_BOOL, s1 == s2) );
				}
				else
					args.push(Lex( LEX_BOOL, temp.get_value() == temp2.get_value()) );
				break;
			case LEX_LSS:
				temp2 = args.pop();
				temp_type = temp2.get_type();
				temp = args.pop();
				if(temp_type == LEX_STR || temp_type == LEX_STRING)
				{
					string s1 = TSTR[temp.get_value()], s2 = TSTR[temp2.get_value()];
					args.push(Lex( LEX_BOOL, s1 < s2) );
				}
				else
				{
					i = temp2.get_value();
					args.push(Lex( LEX_BOOL, temp.get_value() < i) );
				}
				break;
			case LEX_GTR:
				temp2 = args.pop();
				temp_type = temp2.get_type();
				temp = args.pop();
				if(temp_type == LEX_STR || temp_type == LEX_STRING)
				{
					string s1 = TSTR[temp.get_value()], s2 = TSTR[temp2.get_value()];
					args.push(Lex( LEX_BOOL, s1 > s2) );
				}
				else
				{
					i = temp2.get_value();
					args.push(Lex( LEX_BOOL, temp.get_value() > i) );
				}
				break;
			case LEX_LEQ:
				i = args.pop().get_value();
				args.push(Lex( LEX_BOOL, args.pop().get_value() <= i) );
				break;
			case LEX_GEQ:
				i = args.pop().get_value();
				args.push(Lex( LEX_BOOL, args.pop().get_value() >= i) );
				break;
			case LEX_NEQ:
				temp2 = args.pop();
				temp_type = temp2.get_type();
				temp = args.pop();
				if(temp_type == LEX_STR || temp_type == LEX_STRING)
				{
					string s1 = TSTR[temp.get_value()], s2 = TSTR[temp2.get_value()];
					args.push(Lex( LEX_BOOL, s1 != s2) );
				}
				else
				{
					i = temp2.get_value();
					args.push(Lex( LEX_BOOL, temp.get_value() != i) );
				}
				break;
			case LEX_ASSIGN:
				temp2 = args.pop();
				temp = args.pop();
				//cout << temp2 << temp2.get_value() << endl;
				//cout << temp << temp.get_value() << endl;
				i = temp2.get_value();
				j = temp.get_value();
				TID[j].put_value(i);
				TID[j].put_assign();
				args.push(temp2);
				break;
			case POLIZ_GOTO:
				i = args.pop().get_value();
				if(TLB[i] .get_declared())
					index = TLB[i].get_label() - 1;
				else
					throw "POLIZ: using in GOTO undeclared label";
				break;
			case LEX_SEMICOLON:
				args.pop();
				break;
			default:
				throw "POLIZ: unexpected element";
		}	// END OF SWITCH
		++index;
	}		//END OF WHILE
	cout << "Finish of executing!!!\n";
}

//=====================================  INTERPRETATOR  ========================================
class Interpretator
{
	Parser pars;
	Executer E;
public:
	Interpretator(char *program): pars(program){}
	void interpretation()
	{
		pars.analyze();
		E.execute(pars.prog);
	}
};


//=========================================  MAIN  =============================================
int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		cout << "NO input FILE!\n";
		return -1;
	}
	
	try
	{
		Interpretator I(argv[1]);
		I.interpretation();
		return 0;
	}
	catch(int c){
		if(c == EOF)
			cout << "Incorrect end of program_file " << argv[1] << endl;
		else
			cout << "Error on symbol: " << (char)c << endl;
		return 1;
	}
	catch(const Lex &l){
		cout << "unexpected lexeme " << l << endl;
		return 1;
	}
	catch(const char *s){
		cout << s << endl;
		return 1;
	}
	catch(...){
		cout << "WTF???\n";
		return 1;
	}
}
