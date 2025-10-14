// Calculator love compiler (^ 3 ^) !
// [Made in VectorizeOrz]

/* eg. 
1+5*3 
-13/5+2*7+8
(1+3)*(1024-1024)
18/(-3+9)
123/0
*/

/* EBNF Grammar
<Expr> ::= <AddSubExpr>                                     Priority |  LOW
<AddSubExpr> ::= <MulDivExpr> {('+'|'-') <MulDivExpr>}               |
<MulDivExpr> ::= <PrimaryExpr> {('*'|'/') <PrimaryExpr>}             |
<PrimaryExpr> ::= NUM | '-'NUM | '('<Expr>')'                       \|/ HIGH
*/

/* eg. 
1+5*3
<Expr> ::= <AddSubExpr>
       ::= <MulDivExpr> + <MulDivExpr>
       ::= <PrimaryExpr> + <PrimaryExpr> * <PrimaryExpr>
       ::= NUM:1 + NUM:5 * NUM:3
*/

#include <iostream>
#define DEBUG 1

// --- Helper ---
std::string Src;    // source code
int NumberVal = 0;  // if get number token,store its value 
int Current = 0;    // point to char in parsing

void Debug(std::string Message) // print some message
{
    if(DEBUG)
        std::cout<<"[Debug] "<<Message<<std::endl;
}

void Error(std::string Message) // report error and exit
{
    std::cout<<"Error : "<<Message<<std::endl;
    exit(-1);
}

// --- Lexer ---
enum TokenType
{
    NUMBER,                 // 123 -6
    ADD,SUB,MUL,DIV,        // + - * /
    LEFTPAREN,RIGHTPAREN,   // ( )
    END
} Token; // used by evaluate

void GetToken()
{
    while(Current<Src.size() && isspace(Src[Current])) Current++; // skip the white
    if(Current>=Src.size()) 
    {
        Token = END; Debug("TOKEN:END"); return; // in end
    }

    switch(Src[Current])
    {
        case '+': Token = ADD; Current++; Debug("TOKEN:ADD"); return;
        case '-': Token = SUB; Current++; Debug("TOKEN:SUB"); return;
        case '*': Token = MUL; Current++; Debug("TOKEN:MUL"); return;
        case '/': Token = DIV; Current++; Debug("TOKEN:DIV"); return;
        case '(': Token = LEFTPAREN; Current++; Debug("TOKEN:LEFTPAREN"); return;
        case ')': Token = RIGHTPAREN;Current++; Debug("TOKEN:RIGHTPAREN");return;

        default:
            if(Src[Current]>='0' && Src[Current]<='9') // get number
            {
                NumberVal = 0;
                while(Current<Src.size() && Src[Current]>='0' && Src[Current]<='9')
                {
                    NumberVal = NumberVal*10 + Src[Current]-'0';
                    Current++;
                }
                Token = NUMBER; Debug("TOKEN:NUMBER"); return;
            }
            else Error("Unknow token"); // other is error
    }
}

// --- Parser & Interpreter --- (both in EvalXXX) 
int EvalExpr();
int EvalAddSubExpr();
int EvalMulDivExpr();
int EvalPrimaryExpr();

//<Expr> ::= <AddSubExpr>
int EvalExpr()
{
    Debug("EVAL:Expr");

    return EvalAddSubExpr();
}

//<AddSubExpr> ::= <MulDivExpr> {('+'|'-') <MulDivExpr>}
int EvalAddSubExpr()
{
    Debug("EVAL:AddSubExpr");

    int AddSubExprVal = EvalMulDivExpr();
    while(Token==ADD || Token==SUB) // if have + or - go on
    {
        TokenType OpToken = Token;
        GetToken();
        int TempVal = EvalMulDivExpr();
        // accumulate
        if(OpToken==ADD) AddSubExprVal+=TempVal;
        else if(OpToken==SUB) AddSubExprVal-=TempVal;
    }

    return AddSubExprVal;
}

//<MulDivExpr> ::= <PrimaryExpr> {('*'|'/') <PrimaryExpr>}
int EvalMulDivExpr()
{
    Debug("EVAL:MulDivExpr");

    int MulDivExprVal = EvalPrimaryExpr();
    while(Token==MUL || Token==DIV) // if have * or / go on
    {
        TokenType OpToken = Token;
        GetToken();
        int TempVal = EvalPrimaryExpr();
        // accumulate
        if(OpToken==MUL) MulDivExprVal*=TempVal;
        else if(OpToken==DIV) {
            if(TempVal==0) Error("Divide zero"); // xxx/0
            else MulDivExprVal/=TempVal;
        }
    }

    return MulDivExprVal;
}

//<PrimaryExpr> ::= NUM | '-'NUM | '('<Expr>')'
int EvalPrimaryExpr()
{
    Debug("EVAL:PrimaryExpr");

    int PrimaryExprVal = 0;
    
    switch(Token)
    {
        case NUMBER: PrimaryExprVal = NumberVal; break;
        case SUB:{
            GetToken();
            if(Token==NUMBER)
            {
                PrimaryExprVal = -NumberVal; // negative number
                break;
            }
            else Error("Negative not a number");
        }
        case LEFTPAREN:{
            GetToken();
            PrimaryExprVal = EvalExpr(); // recursion can give us its value :)
            if(Token!=RIGHTPAREN) Error("Right paren loss");
            break;
        }
        default: Error("Illegal primary expr");
    }

    GetToken();
    return PrimaryExprVal;
}

// --- Driver ---
int main()
{
    std::cout<<"Input your expr:"<<std::endl;
    std::getline(std::cin,Src);
    std::cout<<std::endl;
    
    GetToken();
    int ExprVal = EvalExpr();
    std::cout<<std::endl<<"Evaluate result :)"<<std::endl;
    std::cout<<ExprVal<<std::endl;

    return 0;
}
