#include<iostream>
#include<fstream>
#include<typeinfo>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"procedure.hh"
#include"program.hh"

int Ast::labelCounter = 0;
Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_Ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Data_Type Ast::get_data_type()
{
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

bool Ast::is_value_zero()
{
	stringstream msg;
	msg << "No is_value_zero() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_data_type(Data_Type dt)
{
	stringstream msg;
	msg << "No set_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	//ADD CODE HERE
	lhs = temp_lhs;
	rhs = temp_rhs;
	lineno = line;
}

Assignment_Ast::~Assignment_Ast()
{
	//ADD CODE HERE
	delete(lhs);
	delete(rhs);
}

bool Assignment_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	// use typeid(), get_data_type()
	//ADD CODE HERE
	// std::cout<<lhs->get_data_type()<<"\n"<<rhs->get_data_type()<<std::endl;
	if(lhs->get_data_type()!=rhs->get_data_type()){ //TODO: if lhs is ass_Ast or arith_Ast or num then show error
		// if(typeid(*rhs)==typeid(Number_Ast<int>) || typeid(*rhs)==typeid(Number_Ast<float>)){	//TODO: so that the same error is show
		// 	if(rhs->is_value_zero()){
		// 		return true;
		// 	}	
		// }	
		CHECK_INPUT(CONTROL_SHOULD_NOT_REACH,"Assignment statement data type not compatible", lineno);
		return false;
	}
	return true;
}

void Assignment_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"\n"<<setw(15)<<"Asgn:\n"<<setw(17)<<"LHS (";
	lhs->print(file_buffer);

	file_buffer<<")\n"<<setw(17)<<"RHS (";	
	rhs->print(file_buffer);

	file_buffer<<")";

}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{
	//ADD CODE HERE
	variable_symbol_entry = &var_entry;
	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");

	node_data_type = var_entry.get_data_type();
	lineno = line;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	// refer to functions for Symbol_Table_Entry 
	//ADD CODE HERE
	//TODO:	CHECK_INVARIANT(node_data_type == variable_symbol_entry->get_data_type(),"Data types dont match");
	return node_data_type;
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	//ADD CODE HERE
	return *variable_symbol_entry;
}

void Name_Ast::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	node_data_type = dt;
}

void Name_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"Name : "<<variable_symbol_entry->get_variable_name();
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	// use Ast_arity from Ast.hh
	//ADD CODE HERE
	constant = number;
	node_data_type = constant_data_type;
	ast_num_child = zero_arity;	//TODO
	lineno = line;

}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	//ADD CODE HERE
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	node_data_type = dt;
}

template <class DATA_TYPE>
bool Number_Ast<DATA_TYPE>::is_value_zero()
{
	//ADD CODE HERE
	if(constant==0)
		return true;
	else 
		return false;	
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"Num : "<<constant;
}

///////////////////////////////////////////////////////////////////////////////

Data_Type Arithmetic_Expr_Ast::get_data_type()
{
	//ADD CODE HERE
	return node_data_type;
}

void Arithmetic_Expr_Ast::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	node_data_type = dt;
}

bool Arithmetic_Expr_Ast::check_ast()
{
	// use get_data_type(), typeid()
	//ADD CODE HERE	
	if(lhs->get_data_type()!=rhs->get_data_type()){ //TODO:
		// if(typeid(*rhs)==typeid(Number_Ast<int>) || typeid(*rhs)==typeid(Number_Ast<double>)){	//TODO:
		// 	if(rhs->is_value_zero()){
		// 		return true;
		// 	}	
		// }	
		// if(typeid(*lhs)==typeid(Number_Ast<int>) || typeid(*lhs)==typeid(Number_Ast<double>)){	//TODO:
		// 	if(lhs->is_value_zero()){
		// 		return true;
		// 	}	
		// }	
		CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Arithmetic statement data type not compatible", lineno);
		return false;
	}
	return true;	
}

Arithmetic_Expr_Ast::~Arithmetic_Expr_Ast()
{
	//ADD CODE HERE
	delete(rhs);
	delete(lhs);
}

/////////////////////////////////////////////////////////////////////

Plus_Ast::Plus_Ast(Ast * l, Ast * r, int line)
{
	// set arity and data type
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	ast_num_child = binary_arity;
	// std::cout<<l->get_data_type()<<"\n"<<r->get_data_type()<<std::endl;
	// if(typeid(*rhs)==typeid(Number_Ast<int>) || typeid(*rhs)==typeid(Number_Ast<double>)){	
	// 	if(rhs->is_value_zero()){
	// 		node_data_type = l->get_data_type(); 		
	// 	}
	// 	else{
	// 		node_data_type = l->get_data_type();	//todo	
	// 	}		
	// }
	// else if(typeid(*lhs)==typeid(Number_Ast<int>) || typeid(*lhs)==typeid(Number_Ast<double>)){	
	// 	if(lhs->is_value_zero()){
	// 		node_data_type = r->get_data_type(); 
	// 	}	
	// 	else{
	// 		node_data_type = l->get_data_type();	//todo	
	// 	}		
	// }
	// else 
	if(lhs->get_data_type()!=rhs->get_data_type())
		node_data_type = void_data_type;	//todo	
	else
		node_data_type = lhs->get_data_type();
	lineno = line;
}

void Plus_Ast::print(ostream & file_buffer)
{
	//add code here
	file_buffer<<"\n"<<setw(24)<<"Arith: PLUS\n"<<setw(20)<<"LHS (";
	lhs->print(file_buffer);
	file_buffer<<")\n"<<setw(20)<<"RHS (";
	rhs->print(file_buffer);
	file_buffer<<")";
}


/////////////////////////////////////////////////////////////////

Minus_Ast::Minus_Ast(Ast * l, Ast * r, int line)
{
	//add code here
	lhs = l;
	rhs = r;
	ast_num_child = binary_arity;
	// if(typeid(*rhs)==typeid(Number_Ast<int>) || typeid(*rhs)==typeid(Number_Ast<double>)){	
	// 	if(rhs->is_value_zero()){
	// 		node_data_type = l->get_data_type(); 		
	// 	}
	// 	else{
	// 		node_data_type = l->get_data_type();	//todo	
	// 	}		
	// }
	// else if(typeid(*lhs)==typeid(Number_Ast<int>) || typeid(*lhs)==typeid(Number_Ast<double>)){	
	// 	if(lhs->is_value_zero()){
	// 		node_data_type = r->get_data_type(); 
	// 	}	
	// 	else{
	// 		node_data_type = l->get_data_type();	//todo	
	// 	}		
	// }
	// else 
	if(lhs->get_data_type()!=rhs->get_data_type())
		node_data_type = void_data_type;	//todo	
	else
		node_data_type = lhs->get_data_type();
	lineno = line;
}

void Minus_Ast::print(ostream & file_buffer)
{
	//add code here
	file_buffer<<"\n"<<setw(25)<<"Arith: MINUS\n"<<setw(20)<<"LHS (";
	lhs->print(file_buffer);
	file_buffer<<")\n"<<setw(20)<<"RHS (";
	rhs->print(file_buffer);
	file_buffer<<")";
}

//////////////////////////////////////////////////////////////////

Mult_Ast::Mult_Ast(Ast * l, Ast * r, int line)
{
	//add code here
	lhs = l;
	rhs = r;
	ast_num_child = binary_arity;
		// std::cout<<typeid(*lhs).name()<<"\n"<<typeid(Number_Ast<int>).name()<<std::endl;
	// if(typeid(*rhs).name()==typeid(Number_Ast<int>).name() || typeid(*rhs).name()==typeid(Number_Ast<double>).name()){	
	// 	if(rhs->is_value_zero()){
	// 		node_data_type = l->get_data_type(); 		
	// 	}
	// 	else{
	// 		node_data_type = l->get_data_type();	//todo	
	// 	}
	// }
	// else if(typeid(*lhs).name()==typeid(Number_Ast<int>).name() || typeid(*lhs).name()==typeid(Number_Ast<double>).name()){	
	// 	if(lhs->is_value_zero()){
	// 		node_data_type = r->get_data_type(); 
	// 	}	
	// 	else{
	// 		node_data_type = l->get_data_type();	//todo	
	// 	}
	// }
	// else 
	if(lhs->get_data_type()!=rhs->get_data_type())
		node_data_type = void_data_type;	//todo	
	else
		node_data_type = lhs->get_data_type();
	
	lineno = line;
}

void Mult_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"\n"<<setw(24)<<"Arith: MULT\n"<<setw(20)<<"LHS (";
	lhs->print(file_buffer);
	file_buffer<<")\n"<<setw(20)<<"RHS (";
	rhs->print(file_buffer);
	file_buffer<<")";
}

////////////////////////////////////////////////////////////////////

Divide_Ast::Divide_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	ast_num_child = binary_arity;
	// if(typeid(*rhs).name()==typeid(Number_Ast<int>).name() || typeid(*rhs).name()==typeid(Number_Ast<double>).name()){	
	// 	if(rhs->is_value_zero()){
	// 		node_data_type = l->get_data_type(); 		
	// 	}
	// 	else{
	// 		node_data_type = l->get_data_type();	//todo	
	// 	}		
	// }
	// else if (typeid(*lhs).name()==typeid(Number_Ast<int>).name() || typeid(*lhs).name()==typeid(Number_Ast<double>).name()){	
	// 	if(lhs->is_value_zero()){
	// 		node_data_type = r->get_data_type(); 
	// 	}
	// 	else{
	// 		node_data_type = l->get_data_type();	//todo	
	// 	}			
	// }
	// else 
	if(lhs->get_data_type()!=rhs->get_data_type())
		node_data_type = void_data_type;	//todo	
	else
		node_data_type = lhs->get_data_type();
	lineno = line;
}

void Divide_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"\n"<<setw(23)<<"Arith: DIV\n"<<setw(20)<<"LHS (";
	lhs->print(file_buffer);
	file_buffer<<")\n"<<setw(20)<<"RHS (";
	rhs->print(file_buffer);
	file_buffer<<")";
}

//////////////////////////////////////////////////////////////////////

UMinus_Ast::UMinus_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	ast_num_child = unary_arity;
	node_data_type = l->get_data_type();	//todo	
	lineno = line;
}

void UMinus_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"\n"<<setw(26)<<"Arith: UMINUS\n"<<setw(20)<<"LHS (";
	lhs->print(file_buffer);
	file_buffer<<")";
}

Relational_Expr_Ast::Relational_Expr_Ast(Ast *l,Relational_Op rop,Ast * r,int line)
{
	lhs_condition = l;
	rhs_condition = r;
	rel_op = rop;
	lineno = line;

	if(lhs_condition->get_data_type()!=rhs_condition->get_data_type())
		node_data_type = void_data_type;	//todo	
	else
		node_data_type = lhs_condition->get_data_type();
	ast_num_child = binary_arity;
	
}

Data_Type Relational_Expr_Ast::get_data_type()
{
	return node_data_type;
}

void Relational_Expr_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

bool Relational_Expr_Ast::check_ast()
{
	if(lhs_condition->get_data_type()!=rhs_condition->get_data_type()){ //TODO:
		CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Relational statement data type not compatible", lineno);
		return false;
	}
	return true;	
}

void Relational_Expr_Ast::print(ostream & file_buffer)
{
	//GOWTHAM
	file_buffer<<"\n"<<setw(23)<<"Condition: ";
	switch(rel_op){
		case less_equalto :
			file_buffer<<"LE";
			break;
		case less_than :
			file_buffer<<"LT";
			break;		
 		case greater_than :
			file_buffer<<"GT";
			break;		
		case greater_equalto : 
			file_buffer<<"GE";
			break; 		
 		case equalto : 
			file_buffer<<"EQ";
			break;		
		case not_equalto :
			file_buffer<<"NE";
			break;
	}
	file_buffer<<"\n"<<setw(20)<<"LHS (";
	lhs_condition->print(file_buffer);
	file_buffer<<")\n"<<setw(20)<<"RHS (";
	rhs_condition->print(file_buffer);
	file_buffer<<")";	
}

Boolean_Expr_Ast::Boolean_Expr_Ast(Ast *lhs,Boolean_Op bop,Ast * rhs, int line)
{
	lhs_op = lhs;
	rhs_op = rhs;
	bool_op = bop;
	lineno = line;

	// if(lhs_op->get_data_type()!=rhs_op->get_data_type())
	// 	node_data_type = void_data_type;	//todo	
	// else
		node_data_type = rhs_op->get_data_type();
	ast_num_child = binary_arity;

}

Data_Type Boolean_Expr_Ast::get_data_type()
{
	return node_data_type;
}

void Boolean_Expr_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

bool Boolean_Expr_Ast::check_ast()
{
	//GOWTHAM
}

void Boolean_Expr_Ast::print(ostream & file_buffer)
{
	file_buffer<<"\n"<<setw(23)<<"Condition: ";
	switch(bool_op){
		case boolean_not :
			file_buffer<<"NOT";
			break;
		case boolean_and :
			file_buffer<<"AND";
			break;		
 		case boolean_or :
			file_buffer<<"OR";
			break;		
	}
	file_buffer<<"\n"<<setw(20)<<"LHS (";
	lhs_op->print(file_buffer);
	file_buffer<<")\n"<<setw(20)<<"RHS (";
	rhs_op->print(file_buffer);
	file_buffer<<")";}

Selection_Statement_Ast::Selection_Statement_Ast(Ast *c,Ast * t,Ast * e, int line)
{
	// std::cout<<"in selection statement"<<endl;
	cond = c;
	then_part = t;
	else_part = e;
	lineno = line;
	node_data_type = void_data_type;
}

Data_Type Selection_Statement_Ast::get_data_type()
{
	return node_data_type;
}

void Selection_Statement_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

bool Selection_Statement_Ast::check_ast()
{
	//GOWTHAM
}

void Selection_Statement_Ast::print(ostream & file_buffer)
{
	//GOWTHAM
	file_buffer<<"\n         IF : \n         CONDITION (";
	cond->print(file_buffer);
	file_buffer<<")\n         THEN (";
	then_part->print(file_buffer);
	file_buffer<<")\n         ELSE (";
	else_part->print(file_buffer);
	file_buffer<<")";
}


Iteration_Statement_Ast::Iteration_Statement_Ast(Ast* i,Ast *c,Ast* u,Ast * b, int line,bool b1,bool b2,bool b3)
{
	init = i;
	cond = c;
	update = u;
	body = b;
	lineno = line;
	is_for = b1;
	is_do_while = b2;
	is_while = b3;
	node_data_type = void_data_type;	//todo	

}

// Iteration_Statement_Ast::Iteration_Statement_Ast(Ast *c,Ast * b, int line,bool b1,bool b2,bool b3)
// {
// 	cond = c;
// 	body = b;
// 	lineno = line;
// 	is_for = b1;
// 	is_do_while = b2;
// 	is_while = b3;	node_data_type = void_data_type;	//todo	
// 	// init = c;
// 	// update = c;
// }

Data_Type Iteration_Statement_Ast::get_data_type()
{
	return node_data_type;
}

void Iteration_Statement_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

bool Iteration_Statement_Ast::check_ast()
{
	//GOWTHAM
}

void Iteration_Statement_Ast::print(ostream & file_buffer)
{
	//GOWTHAM
	if(is_while){
		file_buffer<<"\n         WHILE : \n         CONDITION (";
		cond->print(file_buffer);
		file_buffer<<")\n         BODY (";
		body->print(file_buffer);
		file_buffer<<")";		
	}
	else if(is_do_while){
		file_buffer<<"\n         DO (";
		body->print(file_buffer);
		file_buffer<<")\n         WHILE CONDITION (";
		cond->print(file_buffer);
		file_buffer<<")";

	}
	else if(is_for){
		file_buffer<<"\n         FOR : ";
		if(init!=NULL){
			file_buffer<<"\n         INITIALISATION (";
			init->print(file_buffer);
		}
		if(cond!=NULL){
			file_buffer<<")\n"<<AST_SPACE<<"CONDITION (";
			cond->print(file_buffer);
		}
		if(update!=NULL){
			file_buffer<<")\n"<<AST_SPACE<<"UPDATE STATEMENT (";
			update->print(file_buffer);
		}
		if(body!=NULL){
			file_buffer<<")\n         BODY (";
			body->print(file_buffer);
		}

		file_buffer<<")";

	}
}

Conditional_Operator_Ast::Conditional_Operator_Ast(Ast * c, Ast* l,Ast * r, int line)
{
	lhs = l;
	rhs = r;
	cond = c;
	lineno = line;
	ast_num_child = ternary_arity;
	// if(lhs->get_data_type()!=rhs->get_data_type())
	// 	node_data_type = void_data_type;	//todo	
	// else
		node_data_type = lhs->get_data_type();

}

void Conditional_Operator_Ast::print(ostream & file_buffer)
{
	//GOWTHAM
	file_buffer<<"\n"<<"            Arith: Conditional\n               COND (";
	cond->print(file_buffer);
	file_buffer<<")\n"<<setw(20)<<"LHS (";
	lhs->print(file_buffer);
	file_buffer<<")\n"<<setw(20)<<"RHS (";
	rhs->print(file_buffer);
	file_buffer<<")";	
}


template class Number_Ast<double>;
template class Number_Ast<int>;


Sequence_Ast::Sequence_Ast(int line)
{
	lineno = line;
	node_data_type = void_data_type;
}

void Sequence_Ast::ast_push_back(Ast *ast)
{
	statement_list.push_back(ast);
}

void Sequence_Ast::print(ostream &file_buffer)
{
	//GOWTHAM
	file_buffer<<"\n      Sequence Ast:\n";
	for(std::list<Ast *>::iterator it = statement_list.begin();it!=statement_list.end();it++){
		(*it)->print(file_buffer);	
	}
}

Sequence_Ast::~Sequence_Ast()
{
}