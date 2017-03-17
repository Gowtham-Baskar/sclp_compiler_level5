%filenames="scanner"
%lex-source="scanner.cc"

digit [0-9]
alphabet [a-zA-Z_]
//number([-+]{digit}+)|({digit}+)
number {digit}+
identifier {alphabet}({alphabet}|{digit})*
fl (((({number})\.({number}))|(\.{number})|({number}\.))(([eE][+-]?{number})?))|({number}[eE][+-]?{number}) 


%%
//ADD YOUR CODE HERE

"<="	{
		store_token_name("LE");
		return Parser::LE;		
	}

">="	{
		store_token_name("GE");
		return Parser::GE;		
	}

"<"	{
		store_token_name("LT");
		return Parser::LT;		
	}

">"	{
		store_token_name("GT");
		return Parser::GT;		
	}

"=="	{
		store_token_name("EQ");
		return Parser::EQ;		
	}

"!="	{
		store_token_name("NE");
		return Parser::NE;		
	}


"="	{
		// printf("equal to fouqnd\n");
		store_token_name("ASSIGN_OP");
		return Parser::ASSIGN;
	}

"||"	{
		store_token_name("OR");
		return Parser::OR;		
	}

"&&"	{
		store_token_name("AND");
		return Parser::AND;		
	}

"!"	{
		store_token_name("NOT");
		return Parser::NOT;		
	}

"void"	{
		// printf("void found\n");
		store_token_name("VOID");
		return Parser::VOID;	
	}


"float"	{
		// printf("float found\n");
		store_token_name("FLOAT");
		return Parser::FLOAT;
	}


"int"	{
		// printf("int found\n");
		store_token_name("INTEGER");
		return Parser::INTEGER;
	}
	
"if"	{
		store_token_name("IF");
		return Parser::IF;
	}

"else"	{
		store_token_name("ELSE");
		return Parser::ELSE;
	}

"do"	{
		store_token_name("DO");
		return Parser::DO;
	}

"while"	{
		store_token_name("WHILE");
		return Parser::WHILE;
	}


	

// "return" {
// 		// printf("return found\n");
// 		store_token_name("RETURN");
// 		return Parser::RETURN;
// 	 }
			
{identifier} {
		// std::cout<<"identifier found "<<matched()<<endl;
		ParserBase::STYPE__ *val = getSval();      
		std::string * matched_str = new string(matched()); 	
		val -> string_value = matched_str;
		store_token_name("NAME");
		return Parser::NAME; 
             }

[-+*/] {
		// std::cout<<"other found "<<matched()<<endl;	
		store_token_name("ARITHOP");
		return matched()[0];
	 }

[\(\);{}\:\?\,\{\}] {
		// std::cout<<"other found "<<matched()<<endl;	
		store_token_name("META CHAR");
		return matched()[0];
	 }

{fl} 	{
		// std::cout<<"float value found "<<matched()<<endl;
		ParserBase::STYPE__ *val = getSval();      
		val -> float_value = atof(matched().c_str());
		store_token_name("FNUM");
		return Parser::DOUBLE_NUMBER; 

	}

{number} {	
		// std::cout<<"number found "<<matched()<<endl;
		ParserBase::STYPE__ *val = getSval();      
	        val -> integer_value = atoi(matched().c_str());
		store_token_name("NUM");
		return Parser::INTEGER_NUMBER; 
   	}


\n    		|
";;".*  	|
[ \t]*";;".*	|
[ \t]*"//".*	|
[ \t]		{
			if (command_options.is_show_tokens_selected())
				ignore_token();
		}

.		{ 
			string error_message;
			error_message =  "Illegal character `" + matched();
			error_message += "' on line " + lineNr();
			
			CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, error_message, lineNr());
		}

