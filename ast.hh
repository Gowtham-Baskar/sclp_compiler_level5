#ifndef AST_HH
#define AST_HH

#include<string>
#include<iostream>
#include<iomanip>
#include<typeinfo>
#include<list>
#include<set>


#define AST_SPACE "         "
#define AST_NODE_SPACE "            "
#define AST_SUB_NODE_SPACE "               "
#define SA_SPACE "      "

using namespace std;

class Ast;

class Ast
{
protected:
	typedef enum
	{
		zero_arity = 0,
		unary_arity = 1,
		binary_arity = 2,
		ternary_arity = 3
	}Ast_Arity;

	Data_Type node_data_type;
	Ast_Arity ast_num_child;
	static int labelCounter;
	int lineno;
	string get_new_label(){

		return "label"+to_string(labelCounter++);
	}

public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual void set_data_type(Data_Type dt);

	virtual bool is_value_zero();

	virtual bool check_ast();
	virtual Symbol_Table_Entry & get_symbol_entry();

	virtual void print(ostream & file_buffer) = 0;
	virtual Code_For_Ast & compile() = 0;
	virtual Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
	virtual void print_assembly();
	virtual void print_icode();

};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line);
	~Assignment_Ast();

	bool check_ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Name_Ast:public Ast
{
	Symbol_Table_Entry * variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line);
	~Name_Ast();

	Data_Type get_data_type();
	Symbol_Table_Entry & get_symbol_entry();
	void set_data_type(Data_Type dt);

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
};

template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type, int line);
	~Number_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);
	bool is_value_zero();

	void print(ostream & file_buffer);
	Code_For_Ast & compile();
};

class Relational_Expr_Ast:public Ast
{
	Ast * lhs_condition;
	Ast * rhs_condition;
	Relational_Op rel_op;

public:
	Relational_Expr_Ast(Ast * lhs, Relational_Op rop, Ast * rhs, int line);
	~Relational_Expr_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);

	bool check_ast();

	void print(ostream & file_buffer);
	Code_For_Ast & compile();
};

class Boolean_Expr_Ast:public Ast
{
	Ast * lhs_op;
	Ast * rhs_op;
	Boolean_Op bool_op;

public:
	Boolean_Expr_Ast(Ast * lhs, Boolean_Op bop, Ast * rhs, int line);
	~Boolean_Expr_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);

	bool check_ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Selection_Statement_Ast: public Ast {
protected:
	Ast* cond;
	Ast* then_part;
	Ast* else_part;
public:
	Selection_Statement_Ast(Ast * cond,Ast* then_part, Ast* else_part, int line);
	~Selection_Statement_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);

	bool check_ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Iteration_Statement_Ast: public Ast {
protected:
	Ast* cond;
	Ast* body;
	bool is_do_form;
public:
	Iteration_Statement_Ast(Ast * cond, Ast* body, int line, bool do_form);
	~Iteration_Statement_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);

	bool check_ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};


class Arithmetic_Expr_Ast:public Ast
{
protected:
	Ast * lhs;
	Ast * rhs;

public:
	Arithmetic_Expr_Ast() {}
	~Arithmetic_Expr_Ast();

	Data_Type get_data_type();
	void set_data_type(Data_Type dt);
	bool check_ast();

	virtual void print(ostream & file_buffer) = 0;
	virtual Code_For_Ast & compile() = 0;
};

class Plus_Ast:public Arithmetic_Expr_Ast
{
public:
	Plus_Ast(Ast * l, Ast * r, int line);
	~Plus_Ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Minus_Ast:public Arithmetic_Expr_Ast
{
public:
	Minus_Ast(Ast * l, Ast * r, int line);
	~Minus_Ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Divide_Ast:public Arithmetic_Expr_Ast
{
public:
	Divide_Ast(Ast * l, Ast * r, int line);
	~Divide_Ast();

	void print(ostream & file_buffer);
	Code_For_Ast & compile();
};

class Mult_Ast:public Arithmetic_Expr_Ast
{
public:
	Mult_Ast(Ast * l, Ast * r, int line);
	~Mult_Ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class Conditional_Operator_Ast: public Arithmetic_Expr_Ast
{
protected:
	Ast* cond;
public:
	Conditional_Operator_Ast(Ast* cond, Ast* l, Ast* r, int line);
	~Conditional_Operator_Ast();

	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class UMinus_Ast: public Arithmetic_Expr_Ast
{
public:
	UMinus_Ast(Ast * l, Ast * r, int line);
	~UMinus_Ast();
	
	void print(ostream & file_buffer);

	Code_For_Ast & compile();
};

class BasicBlock{
public:
	list<Icode_Stmt *> icode_list;
	list<BasicBlock *> succ_blocks;
	set<long int> gen;
	set<long int> kill;
	set<long int> in;
	set<long int> out;
	int block_num;

	void insert_stmt(Icode_Stmt* it);
	void print_block();
	void print_succ();
	void print_gen_kill();
	void print_in_out();
	void update_succ(BasicBlock * b);
	void create_gen_kill();
	bool check_opd_type(Ics_Opd * i);
	long int get_id(Ics_Opd *i);
	void remove_dead_stmt();
};

class CFG{
	std::vector<BasicBlock*> blocks;
	public:
		void insertBasicBlock(BasicBlock *b){
			blocks.push_back(b);
		}
		void printBasicBlocks(){
			for(int i=0;i<blocks.size();i++){
				blocks[i]->print_block();
			}
		}
		int get_number_blocks(){
			return blocks.size();
		}
		void update_succ(int s, int e){
			blocks[s]->update_succ(blocks[e]);
		}
		void print_succ(){
			for(int i=0;i<blocks.size();i++){
				cout<<"successors for block "<<i<<" ";
				blocks[i]->print_succ();
			}
		}
		void create_gen_kill(){
			for(int i=0;i<blocks.size();i++){
				blocks[i]->create_gen_kill();
			}
		}
		void print_gen_kill(){
			for(int i=0;i<blocks.size();i++){
				blocks[i]->print_gen_kill();
			}
		}
		void print_in_out(){
			for(int i=0;i<blocks.size();i++){
				blocks[i]->print_in_out();
			}
		}
		vector<BasicBlock*>	get_blocks(){
			return blocks;
		}
		// runs DFS
		void create_in_out_driver();
		// creates in and out sets
		void create_in_out(BasicBlock *b, set<BasicBlock*>&explored, bool &converged);
		// util for set union
		bool union_set(set<long int>&s1,set<long int> &s2);
		// util for set difference
		set<long> difference_set(set<long int>&s1,set<long int> &s2);

		void remove_dead_stmt(){
			for(int i=0;i<blocks.size();i++){
				blocks[i]->remove_dead_stmt();
			}	
		}
};

class Sequence_Ast: public Ast{
	list<Ast *> statement_list;
	list<Icode_Stmt *> sa_icode_list;
	CFG cfg;
public:
	Sequence_Ast(int line);
	~Sequence_Ast();
  void ast_push_back(Ast * ast);
	void print(ostream & file_buffer);
	Code_For_Ast & compile();
	void print_assembly(ostream & file_buffer);
	void print_icode(ostream & file_buffer);
	void optimize();
};



#endif
