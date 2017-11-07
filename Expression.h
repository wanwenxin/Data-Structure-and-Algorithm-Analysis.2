#pragma once
#include "stdafx.h"
#include <stack>
#include <iostream>
#include <string>
using namespace std;

class Expression
{
private:
	string s;                 //记录表达式
	stack<char>operators;     //操作符栈
	stack<double>operands;     //操作数栈
	int cur;                  //当前位置
	int length;               //表达式长度
	char ch;                  //读取字符
	bool end;                 //判断是否结束
	int i;

	//辅助函数
	void show();                        //提示输入，并接受输入
	void operate();                     //计算
	int judge(char ch);                 //判断读入字符属性（数字、计算符、无关符）
	double read_operand();              //读取完整数字
	int operate(char ch, char top);      //运行并检错
	bool compare(char ch, char top);    //比较计算符优先级
	void check(int error);

public:
	void begin();
};

void Expression::show()
{
	cout << "Please input a formula likes (1+2)/3=:" << endl;
	cin >> s;
	if (s == "q" || s == "Q")
	{
		cout << "OVER" << endl;
		system("pause");
		exit(0);
	}
	length = s.length();             //初始化
	cur = 0;
	end = 0;
}

void Expression::begin()                  //开始
{
	show();
	operate();
}

int Expression::judge(char ch)           //判断字符属性
{
	if (ch >= '0'&& ch <= '9')
		return 0;
	else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '(' || ch == ')')
		return 1;
	else return 2;
}

double Expression::read_operand()        //读取完整数
{
	double integer = 0, decimal = 0;
	double opend = 0;
	while (ch >= '0'&& ch <= '9' && !end)
	{
		integer = integer * 10 + ch - '0';
		++cur;
		if (cur == length) end = 1;
		else ch = s[cur];
	}
	if (ch == '.')
	{
		++cur;                                      //小数点后跟符号时错误处理（未写）
		ch = s[cur];
		double rank = 10;
		while (ch >= '0'&& ch <= '9' && !end)
		{
			decimal = decimal + (ch - '0') / rank;
			rank *= 10;
			++cur;
			if (cur == length) end = 1;
			else ch = s[cur];
		}
	}
	double result = integer + decimal;
	return result;
}

void Expression::operate()                     //主干
{
	operands.push(0);
	operators.push('=');
	operators.push('+');//设置栈顶
	while (!operators.empty())     //操作符栈不为空
	{
		if (!end)                 //未到头
		{
			ch = s[cur];
			if (judge(ch) == 2)                               //当前字符是无关符
			{
				cout << ch << " is unknow operator" << endl;
				return;

			}
			else if (ch == '=' && cur != length - 1)
			{
				cout << "Equal sign appears in the middle" << endl;
				return;
			}
			else if (judge(ch) == 1)                          //当前字符是计算符
			{
				char top = operators.top();
				operators.pop();
				int error = operate(ch, top);                //进行运算
				++cur;
				check(error);
			}
			else if (judge(ch) == 0)                              //当前字符是数字
			{
				double operand = read_operand();
				operands.push(operand);
				if (end)
				{
					cout << "Lack of equal sign" << endl;
					return;
				}
			}
		}
		else if (end)
		{
			return;
		}
	}
}

void Expression::check(int error)
{
	switch (error)
	{
	case 1:cout << "Divisor is 0" << endl;end = 1;break;              //error 1：除数是0,结束
	case 2:cout << "2 operators are together" << endl;break;          //error 2：2个运算符在一起
	case 3:if (operands.empty())                                      //error 3  的判断处理
		cout << "Formula is wrong" << endl;
		   else if (!operands.empty())
		   {
			   double result = operands.top();
			   operands.pop();
			   if (!operands.empty())
			   {
				   cout << "Formula is wrong";
				   return;
			   }
			   else
			   {
				   cout << "result is " << result << endl;          //实际上的结束
				   end = 1;
			   }
		   }break;
	default:break;
	}
}

bool Expression::compare(char ch, char top)             //比较运算符优先级,ch与top比，返真则top优先级高，top退栈参与计算
{
	if (ch == '=' || ch == ')')return true;             //优先级：(   + -   * /    )   =
	else if (ch == '(')return false;
	else if (ch == '+' || ch == '-')
	{
		if (top == '(' || top == '=')return false;
		else return true;                               //top为+ - * /   ps:栈内不会有)
	}
	else if (ch == '*' || ch == '/')
	{
		if (top == '*' || top == '/')return true;
		else return false;
	}

}

int Expression::operate(char ch, char top)            //关键步骤
{
	if (ch == '=')           //读入等号
	{
		while (top != '=')
		{
			double a, b;
			if (!operands.empty())
			{
				b = operands.top();
				operands.pop();           //后入栈的数
			}
			else return 2;                //error 2：2个运算符在一起
			if (!operands.empty())
			{
				a = operands.top();
				operands.pop();           //先入栈的数
			}
			else return 2;                //error 2：2个运算符在一起
			if (top == '+')a = a + b;
			else if (top == '-')a = a - b;
			else if (top == '*')a = a * b;
			else if (top == '/')
			{
				if (b == 0)return 1;      //error 1：除数是0
				else a = a / b;
			}
			operands.push(a);
			top = operators.top();        //操作符栈顶元素出栈
			operators.pop();
		}
		return 3;                         //error 3: 等号在中间出现（待定错误）

	}
	else
	{
		if (!compare(ch, top))           //当前字符ch优先级高，ch入栈
		{
			operators.push(top);
			operators.push(ch);
			if (ch == '(')
			{
				operands.push(0);
				operators.push('+');
			}
		}
		else                           //当前字符暂不处理，top出栈参与计算，当前字符可能为四则加）,=
		{
			double a, b;
			if (!operands.empty())
			{
				b = operands.top();
				operands.pop();          //后入栈的数
			}
			else return 2;                //error 2：2个运算符在一起
			if (!operands.empty())
			{
				a = operands.top();
				operands.pop();          //先入栈的数
			}
			else return 2;                //error 2：2个运算符在一起

			if (top == '+')a = a + b;
			else if (top == '-')a = a - b;
			else if (top == '*')a = a * b;
			else if (top == '/')
			{
				if (b == 0)return 1;      //error 1：除数是0
				else a = a / b;
			}
			operands.push(a);


			if (ch == ')')              //开始处理当前字符
			{
				top = operators.top();
				operators.pop();
				while (top != '(')
				{
					double a, b;
					if (!operands.empty())
					{
						b = operands.top();
						operands.pop();          //后入栈的数
					}
					else return 2;                //error 2：2个运算符在一起
					if (!operands.empty())
					{
						a = operands.top();
						operands.pop();          //先入栈的数
					}
					else return 2;                //error 2：2个运算符在一起
					if (top == '+')a = a + b;
					else if (top == '-')a = a - b;
					else if (top == '*')a = a * b;
					else if (top == '/')
					{
						if (b == 0)return 1;      //error 1：除数是0
						else a = a / b;
					}
					operands.push(a);
					top = operators.top();          //操作符栈顶元素出栈
					operators.pop();
				}
				if (operators.empty())return 2;
			}
			else operators.push(ch);           //四则入栈
		}
	}
}
