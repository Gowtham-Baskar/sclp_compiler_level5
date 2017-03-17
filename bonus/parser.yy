%scanner scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	int integer_value;
	std::string * string_value;
	float float_value;
	// pair<Data_Type, string> * decl;
	Sequence_Ast * sequence_ast;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	std::vector<Symbol_Table_Entry*> * symbol_entry_list;
	Procedure * procedure;
	Data_Type vtype;
	std::vector<string*> * vlist;
	std::string * vname;
};

//ADD TOKENS HERE


%token <integer_value> INTEGER_NUMBER
%token <float_value> DOUBLE_NUMBER
%token <string_value> NAME
%token INTEGER FLOAT
%token ASSIGN VOID
%token IF FOR
%token WHILE DO 
//%token RETURN

%right '?' 
%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%left '+' '-'
%left '*' '/'
%right UMINUS NOT
%nonassoc '('
%nonassoc THEN
%nonassoc ELSE


%type <symbol_table> optional_variable_declaration_list
%type <symbol_table> variable_declaration_list
%type <symbol_entry_list> variable_declaration
// %type <decl> declaration
%type <vtype> variable_type
%type <vlist> variable_list
%type <vname> variable_name
%type <sequence_ast> statement_list
%type <ast> assignment_statement
%type <ast> assignment
// %type <ast> for_init_statement
// %type <ast> for_boolean_expression
// %type <ast> for_update_statement
%type <ast> variable
%type <ast> constant
%type <ast> expression_term
%type <ast> arith_expression
%type <ast> operand
%type <ast> statement
%type <ast> selection_statement
%type <ast> iterative_statement
%type <ast> boolean_expression
%type <ast> relational_expression

 
//ADD CODE HERE

%start program

%%

program:
	declaration_list procedure_definition
	{
	if (NOT_ONLY_PARSE)
	{

		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

		program_object.set_procedure(current_procedure, get_line_number());
		program_object.global_list_in_proc_check();
		
	}
	}
;

declaration_list:
	procedure_declaration
	{
	if (NOT_ONLY_PARSE)
	{

		Symbol_Table * global_table = new Symbol_Table();
		program_object.set_global_table(*global_table);
	}
	}
|
	variable_declaration_list
	procedure_declaration
	{
	if (NOT_ONLY_PARSE)
	{

		Symbol_Table * global_table = $1;

		CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

		program_object.set_global_table(*global_table);
	}
	}
|
	procedure_declaration
	variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{

		Symbol_Table * global_table = $2;

		CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

		program_object.set_global_table(*global_table);
	}
	}
;

procedure_declaration:
	VOID NAME '(' ')' ';'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "Procedure name cannot be null");
		CHECK_INVARIANT((*$2 == "main"), "Procedure name must be main in declaration");
	}
	}
;

procedure_definition:
	NAME '(' ')'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Procedure name cannot be null");
		CHECK_INVARIANT((*$1 == "main"), "Procedure name must be main");

		string proc_name = *$1;

		current_procedure = new Procedure(void_data_type, proc_name, get_line_number());

		CHECK_INPUT ((program_object.variable_in_symbol_list_check(proc_name) == false),
			"Procedure name cannot be same as global variable", get_line_number());
	}
	}

	'{' optional_variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{

		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

		Symbol_Table * local_table = $6;

		if (local_table == NULL)
			local_table = new Symbol_Table();

		current_procedure->set_local_list(*local_table);
	}
	}

	statement_list '}'
	{
	if (NOT_ONLY_PARSE)
	{

		Sequence_Ast* seq = $8;

		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");
		CHECK_INVARIANT((seq != NULL), "statement list cannot be null");

		current_procedure->set_sequence_ast(*seq);

	}
	}
;

optional_variable_declaration_list:
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = NULL;
	}
	}
|
	variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Declaration statement list cannot be null here");

		$$ = $1;
	}
	}
;

variable_declaration_list:
	variable_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		// std::cout<<"var decl list"<<endl;
		std::vector<Symbol_Table_Entry*> * var_decl_list = $1;
		Symbol_Table * decl_list = new Symbol_Table();		

		CHECK_INVARIANT((decl_list != NULL), "The declaration statement list cannot be null");

		for(int i=0;i<var_decl_list->size();i++){
			Symbol_Table_Entry * decl_stmt = (*var_decl_list)[i];

			CHECK_INVARIANT((decl_stmt != NULL), "Non-terminal declaration statement cannot be null");

			string decl_name = decl_stmt->get_variable_name();
			CHECK_INPUT ((program_object.variable_proc_name_check(decl_name) == false),
					"Variable name cannot be same as the procedure name", get_line_number());

			if(current_procedure != NULL)
			{
				CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
					"Variable name cannot be same as procedure name", get_line_number());
			}

			CHECK_INPUT((decl_list->variable_in_symbol_list_check(decl_name) == false), 
					"Variable is declared twice", get_line_number());

			decl_list->push_symbol(decl_stmt);
		}
		$$ = decl_list;
	}
	}
|
	variable_declaration_list variable_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list
		std::vector<Symbol_Table_Entry*> * var_decl_list = $2;
		Symbol_Table * decl_list = $1;
		CHECK_INVARIANT((decl_list != NULL), "The declaration statement list cannot be null");

		for(int i=0;i<var_decl_list->size();i++){
			Symbol_Table_Entry * decl_stmt = (*var_decl_list)[i];

			CHECK_INVARIANT((decl_stmt != NULL), "The declaration statement cannot be null");

			string decl_name = decl_stmt->get_variable_name();
			CHECK_INPUT((program_object.variable_proc_name_check(decl_name) == false),
				"Procedure name cannot be same as the variable name", get_line_number());
			if(current_procedure != NULL)
			{
				CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
					"Variable name cannot be same as procedure name", get_line_number());
			}

			CHECK_INPUT((decl_list->variable_in_symbol_list_check(decl_name) == false), 
					"Variable is declared twice", get_line_number());

			decl_list->push_symbol(decl_stmt);
		}

		$$ = decl_list;
	}
	}
;

variable_declaration:
	variable_type variable_list ';'
	{
	if (NOT_ONLY_PARSE)
	{
		// std::cout<<"var decl"<<endl;
		Data_Type var_type = $1;
		std::vector<string*> *var_list = $2;
		std::vector<Symbol_Table_Entry*> * var_decl_list =  new std::vector<Symbol_Table_Entry*>();
		for(int i=0;i<var_list->size();i++){
			Symbol_Table_Entry * decl = new Symbol_Table_Entry(*((*var_list)[i]),var_type,get_line_number());
			var_decl_list->push_back(decl);
		}
		$$ = var_decl_list;
	}
	}
;


variable_type:
	INTEGER
	{
	if(NOT_ONLY_PARSE)
	{
		Data_Type data_type = int_data_type;
		$$ = data_type;
	}
	}
|
	FLOAT
	{
	if(NOT_ONLY_PARSE)
	{
		Data_Type data_type = double_data_type;
		$$ = data_type;
	}
	}
;

variable_list:
	variable_name
	{
	if(NOT_ONLY_PARSE)
	{
		std::vector<string*> * var_list =  new std::vector<string*>();
		string* var_name = $1;
		var_list->push_back(var_name);
		$$ = var_list;
	}
	}
|
	variable_list ',' variable_name
	{
	if(NOT_ONLY_PARSE)
	{
		std::vector<string*> * var_list = $1;
		string *var_name = $3;
		var_list->push_back(var_name);
		$$ = var_list;
	}
	}
;

variable_name:
	NAME
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}	
;

// variable_declaration:
// 	declaration ';'
// 	{
// 	if (NOT_ONLY_PARSE)
// 	{
// 		pair<Data_Type, string> * decl = $1;

// 		CHECK_INVARIANT((decl != NULL), "Declaration cannot be null");

// 		Data_Type type = decl->first;
// 		string decl_name = decl->second;

// 		Symbol_Table_Entry * decl_entry = new Symbol_Table_Entry(decl_name, type, get_line_number());

// 		$$ = decl_entry;

// 	}
// 	}
// ;

// declaration:
// 	INTEGER NAME
// 	{
// 	if (NOT_ONLY_PARSE)
// 	{
// 		//ADD CODE HERE
// 		string name = *$2;
// 		Data_Type type = int_data_type;
// 		pair<Data_Type, string> * declar = new pair<Data_Type, string>(type, name);

// 		$$ = declar;
// 	}
// 	}
// |
// 	FLOAT NAME
// 	{
// 	if (NOT_ONLY_PARSE)
// 	{
// 		//ADD CODE HERE
// 		string name = *$2;
// 		Data_Type type = double_data_type;
// 		pair<Data_Type, string> * declar = new pair<Data_Type, string>(type, name);

// 		$$ = declar;
// 	}
// 	}
// ;


statement_list:
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE

		// std::cout<<"empty statement list"<<endl;
		Sequence_Ast * statement_list = new Sequence_Ast(get_line_number());
		$$ = statement_list;
	}
	}
|
	statement_list statement
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		// std::cout<<"statement list"<<endl;

		Sequence_Ast * statement_list = $1;
		Ast * statement = $2; 
		CHECK_INVARIANT((statement!= NULL), "The statement  cannot be null"); 	//TODO
		statement_list->ast_push_back(statement);
		$$ = statement_list;
		// std::cout<<"statement list finished"<<endl;		
	}
	}
;


statement :
	selection_statement
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}
|	
	assignment_statement
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}

|
	iterative_statement
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $1;
	}
	}

|
	'{' statement_list '}'
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $2;
	}
	}
;

selection_statement:
	IF  '(' boolean_expression ')' statement %prec THEN
	{
	if(NOT_ONLY_PARSE)		
	{
		Ast * cond = $3;
		Ast * then_part = $5;
		Ast * else_part = new Sequence_Ast(get_line_number()); 
		CHECK_INVARIANT((then_part!= NULL), "The then_part  cannot be null"); 	//TODO
		CHECK_INVARIANT((else_part!= NULL), "The else_part  cannot be null"); 	//TODO		
		Ast * selection_ast = new Selection_Statement_Ast(cond,then_part,else_part,get_line_number());
		$$ = selection_ast;		
	}
	}

|	
	IF  '(' boolean_expression ')' statement ELSE statement  
	{
	if(NOT_ONLY_PARSE)		
	{
		Ast * cond = $3;
		Ast * then_part = $5;
		Ast * else_part = $7;
		Ast * selection_ast = new Selection_Statement_Ast(cond,then_part,else_part,get_line_number());
		$$ = selection_ast;		
	}
	}
;



iterative_statement:
	WHILE '(' boolean_expression ')' statement
	{
	if(NOT_ONLY_PARSE)		
	{
		Ast * cond = $3;
		Ast * body = $5;
		Ast * init = NULL;
		Ast * update = NULL;

		Ast * iterative_ast = new Iteration_Statement_Ast(init,cond,update,body,get_line_number(),false,false,true);
		$$ = iterative_ast;		
	}
	}

|
	DO statement WHILE '(' boolean_expression ')' ';'
	{
	if(NOT_ONLY_PARSE)		
	{
		Ast * cond = $5;
		Ast * body = $2;
		Ast * init = NULL;
		Ast * update = NULL;

		Ast * iterative_ast = new Iteration_Statement_Ast(init,cond,update,body,get_line_number(),false,true,false);
		$$ = iterative_ast;		
	}
	}

|
	FOR '(' assignment ';' boolean_expression ';' assignment ')' statement
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * init = $3;
		Ast * cond = $5;
		Ast * update = $7;
		Ast * body = $9;
		Ast * for_ast = new Iteration_Statement_Ast(init,cond,update,body,get_line_number(),true,false,false);

		$$ = for_ast;

	}
	}
; 
	
// for_init_statement :
// 	{
// 	if(NOT_ONLY_PARSE)
// 	{
// 		$$ = NULL;
// 	}
// 	}
// |	
// 	assignment 
// 	{
// 	if(NOT_ONLY_PARSE)
// 	{
// 		$$=$1;
// 	}
// 	}
// ;

// for_boolean_expression:
// 	{
// 	if(NOT_ONLY_PARSE)
// 	{
// 		$$ = NULL;
// 	}
// 	}
// |	
// 	boolean_expression 
// 	{
// 	if(NOT_ONLY_PARSE)
// 	{
// 		$$=$1;
// 	}
// 	}
// ;


// for_update_statement:
// 	{
// 	if(NOT_ONLY_PARSE)
// 	{
// 		$$ = NULL;
// 	}
// 	}
// |	
// 	assignment 
// 	{
// 	if(NOT_ONLY_PARSE)
// 	{
// 		$$=$1;
// 	}
// 	}
// ;

// statement:
// 	matched_statement 
// 	{
// 	if(NOT_ONLY_PARSE)
// 	{
// 		$$ = $1;
// 	}
// 	}
// |
// 	unmatched_statement
// 	{
// 	if(NOT_ONLY_PARSE)
// 	{
// 		// std::cout<<"unmatched statement"<<endl;
// 		$$ = $1;
// 	}
// 	}
// ;	

// matched_statement:
// 	IF  '(' boolean_expression ')' THEN matched_statement ELSE matched_statement %prec THEN
// 	{
// 	if(NOT_ONLY_PARSE)		
// 	{
// 		Ast * cond = $3;
// 		Ast * then_part = $5;
// 		Ast * else_part = $7;
// 		CHECK_INVARIANT((then_part!= NULL), "The then_part  cannot be null"); 	//TODO
// 		CHECK_INVARIANT((else_part!= NULL), "The else_part  cannot be null"); 	//TODO		
// 		Ast * selection_ast = new Selection_Statement_Ast(cond,then_part,else_part,get_line_number());
// 		$$ = selection_ast;		
// 	}
// 	}

// |	
// 	other_statement
// 	{
// 	if(NOT_ONLY_PARSE)
// 	{
// 		$$ =$1;
// 	}
// 	}
// ;

// unmatched_statement:
// 	IF  '(' boolean_expression ')' THEN statement 	%prec THEN
// 	{
// 	if(NOT_ONLY_PARSE)		
// 	{
// 		// std::cout<<"in unmatched statement"<<endl;
// 		Ast * cond = $3;
// 		Ast * then_part = $5;
// 		Ast * else_part = new Sequence_Ast(get_line_number()); 
// 		CHECK_INVARIANT((then_part!= NULL), "The then_part  cannot be null"); 	//TODO
// 		CHECK_INVARIANT((else_part!= NULL), "The else_part  cannot be null"); 	//TODO
// 		Ast * selection_ast = new Selection_Statement_Ast(cond,then_part,else_part,get_line_number());
// 		$$ = selection_ast;		
// 	}
// 	}	

// |
// 	IF  '(' boolean_expression ')' THEN matched_statement ELSE unmatched_statement  %prec THEN
// 	{
// 	if(NOT_ONLY_PARSE)		
// 	{
// 		Ast * cond = $3;
// 		Ast * then_part = $5;
// 		Ast * else_part = $7;
// 		Ast * selection_ast = new Selection_Statement_Ast(cond,then_part,else_part,get_line_number());
// 		$$ = selection_ast;		
// 	}
// 	}
// ;


boolean_expression:
	boolean_expression OR boolean_expression 
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * boolean_expression = new Boolean_Expr_Ast(lhs,boolean_or,rhs,get_line_number());
		$$ = boolean_expression;
	}
	}
|
	boolean_expression AND boolean_expression
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * boolean_expression = new Boolean_Expr_Ast(lhs,boolean_and,rhs,get_line_number());
		$$ = boolean_expression;
	}
	}
|
	NOT boolean_expression 
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * rhs = $2;
		Ast * boolean_expression = new Boolean_Expr_Ast(NULL,boolean_not,rhs,get_line_number());
		$$ = boolean_expression;	
	}
	}
|
	'(' boolean_expression ')'
	{
	if(NOT_ONLY_PARSE)
	{
		$$ = $2;	
	}
	}	
|
	relational_expression
	{
	if(NOT_ONLY_PARSE)
	{
		// std::cout<<"relational_expression"<<endl;
		$$ = $1;
	}
	}
;


relational_expression :
	operand LT operand
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * relational_expression = new Relational_Expr_Ast(lhs,less_than,rhs,get_line_number());
		relational_expression->check_ast();
		$$ = relational_expression;
	}
	}
|
	operand LE operand
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * relational_expression = new Relational_Expr_Ast(lhs,less_equalto,rhs,get_line_number());
		relational_expression->check_ast();
		$$ = relational_expression;
	}
	}
|
	operand GT operand
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * relational_expression = new Relational_Expr_Ast(lhs,greater_than,rhs,get_line_number());
		relational_expression->check_ast();
		$$ = relational_expression;
	}
	}
|
	operand GE operand
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * relational_expression = new Relational_Expr_Ast(lhs,greater_equalto,rhs,get_line_number());
		relational_expression->check_ast();
		$$ = relational_expression;
	}
	}
|
	operand EQ operand
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * relational_expression = new Relational_Expr_Ast(lhs,equalto,rhs,get_line_number());
		relational_expression->check_ast();
		$$ = relational_expression;
	}
	}
|
	operand NE operand
	{
	if(NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * relational_expression = new Relational_Expr_Ast(lhs,not_equalto,rhs,get_line_number());
		relational_expression->check_ast();
		$$ = relational_expression;
	}
	}
;


assignment:
	variable ASSIGN arith_expression
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * assignment_ast = new Assignment_Ast(lhs,rhs,get_line_number());
		assignment_ast->check_ast();
		$$ = assignment_ast; //TODO
	}
	}
;
// Make sure to call check_ast in assignment_statement and arith_expression
// Refer to error_display.hh for displaying semantic errors if any
assignment_statement:
	variable ASSIGN arith_expression ';'
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * assignment_ast = new Assignment_Ast(lhs,rhs,get_line_number());
		assignment_ast->check_ast();
		$$ = assignment_ast; //TODO
	}
	}
;

arith_expression:
	operand '+' operand
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * ast = new Plus_Ast(lhs,rhs,get_line_number());
		ast->check_ast();		
		$$ = ast;
	}
	}
|
	operand '-' operand
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * ast = new Minus_Ast(lhs,rhs,get_line_number());
		ast->check_ast();		
		$$ = ast;
	
	}
	}
|
	operand '*' operand
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * ast = new Mult_Ast(lhs,rhs,get_line_number());
		ast->check_ast();		
		$$ = ast;
	
	}
	}
|
	operand '/' operand
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * lhs = $1;
		Ast * rhs = $3;
		Ast * ast = new Divide_Ast(lhs,rhs,get_line_number());
		ast->check_ast();		
		$$ = ast;
	
	}
	}
|
	'-' operand %prec UMINUS
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * lhs = $2;
		Ast * ast = new UMinus_Ast(lhs,NULL,get_line_number());
		$$ = ast;
	
	}
	}
|
	boolean_expression '?' operand ':' operand 
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * lhs = $3;
		Ast * rhs = $5;
		Ast * cond = $1;
		Ast * ast = new Conditional_Operator_Ast(cond,lhs,rhs,get_line_number());
		ast->check_ast();
		$$  = ast;
	}
	}	
|
	'(' operand ')'
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $2;
	}
	}
|
	expression_term
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $1;			
	}
	}
;
		//ADD RELEVANT CODE ALONG WITH GRAMMAR RULES HERE
                // SUPPORT binary +, -, *, / operations, unary -, and allow parenthesization
                // i.e. E -> (E)
                // Connect the rules with the remaining rules given below


operand:
	arith_expression
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		$$ = $1;
		
	}
	}
;

expression_term:
	variable
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		$$ = $1; 
	}
	}
|
	constant
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		$$ = $1;
	}
	}
;

variable:
	NAME
	{
	if (NOT_ONLY_PARSE)
	{

		Symbol_Table_Entry * var_table_entry;

		CHECK_INVARIANT(($1 != NULL), "Variable name cannot be null");

		if (current_procedure->variable_in_symbol_list_check(*$1))
			 var_table_entry = &(current_procedure->get_symbol_table_entry(*$1));

		else if (program_object.variable_in_symbol_list_check(*$1))
			var_table_entry = &(program_object.get_symbol_table_entry(*$1));

		else
			CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Variable has not been declared", get_line_number());

		$$ = new Name_Ast(*$1, *var_table_entry, get_line_number());

		delete $1;

	}
	}
;

constant:
	INTEGER_NUMBER
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		int number = $1; 
		Ast * num_ast  = new Number_Ast<int>(number,int_data_type,get_line_number());
		$$ = num_ast;
	}
	}
|
	DOUBLE_NUMBER
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		double number = $1; 
		Ast * num_ast  = new Number_Ast<double>(number,double_data_type,get_line_number());
		$$ = num_ast;
	}
	}
;
