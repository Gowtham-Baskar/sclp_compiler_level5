#include <iostream>
#include <fstream>
#include <typeinfo>

using namespace std;

#include "common-classes.hh"
#include "error-display.hh"
#include "user-options.hh"
#include "icode.hh"
#include "reg-alloc.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "procedure.hh"
#include "program.hh"

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_assembly()
{
	stringstream msg;
	msg << "No print_assembly() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_icode()
{
	stringstream msg;
	msg << "No print_icode() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Code_For_Ast & Assignment_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Assignment_Ast");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();
	CHECK_INVARIANT(load_register, "Load register cannot be null in Assignment_Ast");
	// if(load_register!=NULL) load_register->set_use_for_expr_result();

	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	CHECK_INVARIANT((load_register != NULL), "Load register cannot be null in Assignment_Ast");
	if(load_register!=NULL) load_register->reset_use_for_expr_result();

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt = new Code_For_Ast();
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

	return *assign_stmt;
}


/////////////////////////////////////////////////////////////////

Code_For_Ast & Name_Ast::compile()
{
	if(node_data_type == int_data_type){
		Mem_Addr_Opd * variable = new Mem_Addr_Opd(*variable_symbol_entry);
		Tgt_Op ic_operator;
		const Register_Use_Category reg_type = gp_data;
		ic_operator = load;
		Register_Descriptor* load_register = machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Move_IC_Stmt *stmt = new Move_IC_Stmt(ic_operator,variable,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		ic_list.push_back(stmt);	
		Code_For_Ast * load_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			load_stmt  = new Code_For_Ast(ic_list,load_register);
		return *load_stmt;
	}
	else if(node_data_type == double_data_type){
		Mem_Addr_Opd * variable = new Mem_Addr_Opd(*variable_symbol_entry);
		Tgt_Op ic_operator;
		const Register_Use_Category reg_type = float_reg;
		ic_operator = load_d;
		Register_Descriptor* load_register = machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Move_IC_Stmt *stmt = new Move_IC_Stmt(ic_operator,variable,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		ic_list.push_back(stmt);	
		Code_For_Ast * load_stmt =  new Code_For_Ast();
		if (ic_list.empty() == false)
			load_stmt  = new Code_For_Ast(ic_list,load_register);
		return *load_stmt;
;	}
	
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{	
	Mem_Addr_Opd * variable = new Mem_Addr_Opd(*variable_symbol_entry);
	Register_Addr_Opd * reg = new Register_Addr_Opd(store_register);
	Move_IC_Stmt *stmt;
	if(node_data_type ==int_data_type){
		stmt = new Move_IC_Stmt(store,reg,variable);
	}
	else if(node_data_type == double_data_type){
		stmt = new Move_IC_Stmt(store_d,reg,variable);
	} 

	if (command_options.is_do_lra_selected() == false)
		variable_symbol_entry->free_register(store_register);

	else
	{
		variable_symbol_entry->update_register(store_register);
		store_register->reset_use_for_expr_result();
	}


	list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
	ic_list.push_back(stmt);	
	Code_For_Ast * store_stmt = new Code_For_Ast();
	if (ic_list.empty() == false)
		store_stmt  = new Code_For_Ast(ic_list,NULL);
	return *store_stmt;
	
}


///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile()
{

	if(node_data_type == int_data_type){
		Const_Opd<int> * const_num = new Const_Opd<int>(constant);
		Tgt_Op ic_operator;
	    const Register_Use_Category reg_type = gp_data;
		ic_operator = imm_load;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Move_IC_Stmt *stmt = new Move_IC_Stmt(ic_operator,const_num,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		ic_list.push_back(stmt);	
		Code_For_Ast * load_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			load_stmt  = new Code_For_Ast(ic_list,load_register);
		return *load_stmt;
	}
	else if(node_data_type == double_data_type){
		Const_Opd<double> * const_num = new Const_Opd<double>(constant);
		Tgt_Op ic_operator;
	    const Register_Use_Category reg_type = float_reg;
		ic_operator = imm_load_d;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Move_IC_Stmt *stmt = new Move_IC_Stmt(ic_operator,const_num,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		ic_list.push_back(stmt);	
		Code_For_Ast * load_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			load_stmt  = new Code_For_Ast(ic_list,load_register);
		return *load_stmt;
	}
}

///////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Relational_Expr_Ast::compile()
{
	CHECK_INVARIANT((lhs_condition != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	CHECK_INVARIANT((rhs_condition != NULL), "Rhs cannot be null in Relational_Expr_Ast");

	Code_For_Ast & lhs_stmt = lhs_condition->compile();
	Register_Descriptor * lhs_load_register = lhs_stmt.get_reg();
	// if(lhs_load_register != NULL)	lhs_load_register->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs_condition->compile();
	Register_Descriptor * rhs_load_register = rhs_stmt.get_reg();
	// if(rhs_load_register != NULL)	rhs_load_register->set_use_for_expr_result();

	//GOWTHAM : check NULL

	Register_Addr_Opd * lhs_addr = new Register_Addr_Opd(lhs_load_register);
	Register_Addr_Opd * rhs_addr = new Register_Addr_Opd(rhs_load_register);

	Tgt_Op ic_operator;	
 	switch(rel_op){
		case less_equalto :
			ic_operator = sle;
			break;	
		case less_than :
			ic_operator = slt;
			break;	
		case greater_than :
			ic_operator = sgt;
			break;	
		case greater_equalto :
			ic_operator = sge;
			break;	
		case equalto:
			ic_operator = seq;
			break;	
		case not_equalto :
			ic_operator = sne;
			break;
	}	

	Register_Descriptor* load_register =  machine_desc_object.get_new_register<gp_data>();	
	// if(load_register!=NULL) load_register->set_use_for_expr_result();
	if(lhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();
	if(rhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();

	Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
	Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
	list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;

	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();
	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
	
	ic_list.push_back(stmt);	

	Code_For_Ast * rel_stmt =new Code_For_Ast();
	if (ic_list.empty() == false)
		rel_stmt = new Code_For_Ast(ic_list, load_register);

	return *rel_stmt;
	
}

//////////////////////////////////////////////////////////////////////

Code_For_Ast & Boolean_Expr_Ast::compile()
{

	if(bool_op!=boolean_not){
		Code_For_Ast & lhs_stmt = lhs_op->compile();
		Register_Descriptor * lhs_load_register = lhs_stmt.get_reg();
		// if(lhs_load_register!=NULL) lhs_load_register->set_use_for_expr_result();

		Code_For_Ast & rhs_stmt = rhs_op->compile();
		Register_Descriptor * rhs_load_register = rhs_stmt.get_reg();
		// if(rhs_load_register!=NULL) rhs_load_register->set_use_for_expr_result();


		//GOWTHAM : check NULL

		Register_Addr_Opd *lhs_addr = new Register_Addr_Opd(lhs_load_register);
		Register_Addr_Opd *rhs_addr = new Register_Addr_Opd(rhs_load_register);
		Tgt_Op ic_operator;	
	 	switch(bool_op){
			case boolean_or :
				ic_operator = or_t;
				break;	
			case boolean_and :
				ic_operator = and_t;
				break;	
		}	
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<gp_data>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;

		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		
		ic_list.push_back(stmt);	

		Code_For_Ast * bool_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			bool_stmt = new Code_For_Ast(ic_list, load_register);

		return *bool_stmt;

	}
	else{
		// Code_For_Ast & lhs_stmt = lhs_op->compile();
		// Register_Descriptor * lhs_load_register = lhs_stmt.get_reg();
		// if(lhs_load_register!=NULL) lhs_load_register->set_use_for_expr_result();

		Register_Descriptor* lhs_load_register =  machine_desc_object.get_new_register<gp_data>();	

		Code_For_Ast & rhs_stmt = rhs_op->compile();
		Register_Descriptor * rhs_load_register = rhs_stmt.get_reg();
		// if(rhs_load_register!=NULL) rhs_load_register->set_use_for_expr_result();


		//GOWTHAM : check NULL

		Register_Addr_Opd *lhs_addr = new Register_Addr_Opd(lhs_load_register);
		Register_Addr_Opd *rhs_addr = new Register_Addr_Opd(rhs_load_register);
		Const_Opd<int> * one_num = new Const_Opd<int>(1);

		Tgt_Op ic_operator;	
	 	switch(bool_op){
			case boolean_not :
				ic_operator = not_t;
				break;	
		}	
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<gp_data>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Move_IC_Stmt * load_one_stmt = new Move_IC_Stmt(imm_load,one_num,lhs_addr);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,rhs_addr,lhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;

		ic_list.push_back(load_one_stmt);
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		
		ic_list.push_back(stmt);	

		Code_For_Ast * bool_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			bool_stmt = new Code_For_Ast(ic_list, load_register);

		return *bool_stmt;
	}

	
}
///////////////////////////////////////////////////////////////////////

Code_For_Ast & Selection_Statement_Ast::compile()
{
	CHECK_INVARIANT((cond != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	CHECK_INVARIANT((then_part != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	CHECK_INVARIANT((else_part != NULL), "Lhs cannot be null in Relational_Expr_Ast");


	Code_For_Ast & cond_stmt_code = cond->compile();
	Register_Descriptor * cond_load_register = cond_stmt_code.get_reg();
	// if(cond_load_register!=NULL)	cond_load_register->set_use_for_expr_result();


	Code_For_Ast & then_stmt_code = then_part->compile();
	Register_Descriptor * then_load_register = then_stmt_code.get_reg();
	// if(then_load_register!=NULL)	then_load_register->set_use_for_expr_result();

	Code_For_Ast & else_stmt_code = else_part->compile();
	Register_Descriptor * else_load_register = else_stmt_code.get_reg();

	if(cond_load_register!=NULL)	cond_load_register->reset_use_for_expr_result();
	if(then_load_register!=NULL)	then_load_register->reset_use_for_expr_result();
	if(else_load_register!=NULL) 	else_load_register->reset_use_for_expr_result();



	Register_Addr_Opd *cond_addr = new Register_Addr_Opd(cond_load_register);
	Register_Addr_Opd *zero_addr = new Register_Addr_Opd(machine_desc_object.spim_register_table[zero]);

	string label1 = get_new_label();
	string label2 = get_new_label();
	Control_Flow_IC_Stmt * cond_stmt = new Control_Flow_IC_Stmt(beq,cond_addr,zero_addr,label1);
	Control_Flow_IC_Stmt * jump_stmt = new Control_Flow_IC_Stmt(j,NULL,NULL,label2);
	Label_IC_Stmt * label1_stmt = new Label_IC_Stmt(label,label1);
	Label_IC_Stmt * label2_stmt = new Label_IC_Stmt(label,label2);


	list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
	if (cond_stmt_code.get_icode_list().empty() == false)
		ic_list = cond_stmt_code.get_icode_list();
	ic_list.push_back(cond_stmt);	


	if (then_stmt_code.get_icode_list().empty() == false){
		// cout<<"B"<<endl;
		// cout<<then_stmt_code.get_icode_list().size()<<endl;
		ic_list.splice(ic_list.end(), then_stmt_code.get_icode_list());
		// cout<<"A"<<endl;
	}

	ic_list.push_back(jump_stmt);	
	ic_list.push_back(label1_stmt);	

	if (else_stmt_code.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), else_stmt_code.get_icode_list());
	ic_list.push_back(label2_stmt);	

	Code_For_Ast * selection_stmt = new Code_For_Ast();
	if (ic_list.empty() == false)
		selection_stmt = new Code_For_Ast(ic_list, NULL);		//GOWTHAM :


	return *selection_stmt;

}

///////////////////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Iteration_Statement_Ast::compile()
{

	// CHECK_INVARIANT((cond != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	// CHECK_INVARIANT((body != NULL), "Lhs cannot be null in Relational_Expr_Ast");

	if(is_while){
		Code_For_Ast & cond_stmt_code = cond->compile();
		Register_Descriptor * cond_load_register = cond_stmt_code.get_reg();
		// if (cond_load_register!=NULL) cond_load_register->set_use_for_expr_result();




		Register_Addr_Opd *cond_addr = new Register_Addr_Opd(cond_load_register);
		Register_Addr_Opd *zero_addr = new Register_Addr_Opd(machine_desc_object.spim_register_table[zero]);

		string label1 = get_new_label();
		string label2 = get_new_label();
		Control_Flow_IC_Stmt * cond_stmt = new Control_Flow_IC_Stmt(bne,cond_addr,zero_addr,label1);
		Control_Flow_IC_Stmt * jump_stmt = new Control_Flow_IC_Stmt(j,NULL,NULL,label2);
		Label_IC_Stmt * label1_stmt = new Label_IC_Stmt(label,label1);
		Label_IC_Stmt * label2_stmt = new Label_IC_Stmt(label,label2);

		Code_For_Ast & body_stmt_code = body->compile();
		Register_Descriptor * body_load_register = body_stmt_code.get_reg();


		if(cond_load_register!=NULL)	cond_load_register->reset_use_for_expr_result();
		if(body_load_register!=NULL) 	body_load_register->reset_use_for_expr_result();


		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		ic_list.push_back(jump_stmt);	
		ic_list.push_back(label1_stmt);	
		if (body_stmt_code.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), body_stmt_code.get_icode_list());
		ic_list.push_back(label2_stmt);	
		if (cond_stmt_code.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), cond_stmt_code.get_icode_list());
		ic_list.push_back(cond_stmt);	

		Code_For_Ast * iteration_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			iteration_stmt = new Code_For_Ast(ic_list, NULL);		//GOWTHAM :

		return *iteration_stmt;

	}
	else if(is_do_while){

		Code_For_Ast & cond_stmt_code = cond->compile();
		Register_Descriptor * cond_load_register = cond_stmt_code.get_reg();
		// if (cond_load_register!=NULL) cond_load_register->set_use_for_expr_result();


		Register_Addr_Opd *cond_addr = new Register_Addr_Opd(cond_load_register);
		Register_Addr_Opd *zero_addr = new Register_Addr_Opd(machine_desc_object.spim_register_table[zero]);

		string label1 = get_new_label();
		string label2 = get_new_label();
		Control_Flow_IC_Stmt * cond_stmt = new Control_Flow_IC_Stmt(bne,cond_addr,zero_addr,label1);
		Label_IC_Stmt * label1_stmt = new Label_IC_Stmt(label,label1);
		Label_IC_Stmt * label2_stmt = new Label_IC_Stmt(label,label2);


		Code_For_Ast & body_stmt_code = body->compile();
		Register_Descriptor * body_load_register = body_stmt_code.get_reg();

		if(cond_load_register!=NULL)	cond_load_register->reset_use_for_expr_result();
		if(body_load_register!=NULL) 	body_load_register->reset_use_for_expr_result();


		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		ic_list.push_back(label1_stmt);	
		if (body_stmt_code.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), body_stmt_code.get_icode_list());
		ic_list.push_back(label2_stmt);	
		if (cond_stmt_code.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), cond_stmt_code.get_icode_list());
		ic_list.push_back(cond_stmt);	

		Code_For_Ast * iteration_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			iteration_stmt = new Code_For_Ast(ic_list, NULL);		//GOWTHAM :

		return *iteration_stmt;
	
	}
	else if(is_for){

		Code_For_Ast & init_stmt_code = init->compile();
		Register_Descriptor * init_load_register = init_stmt_code.get_reg();

		Code_For_Ast & cond_stmt_code = cond->compile();
		Register_Descriptor * cond_load_register = cond_stmt_code.get_reg();


		Code_For_Ast & update_stmt_code = update->compile();
		Register_Descriptor * update_load_register = update_stmt_code.get_reg();
		// if (cond_load_register!=NULL) cond_load_register->set_use_for_expr_result();



		Register_Addr_Opd *cond_addr = new Register_Addr_Opd(cond_load_register);
		Register_Addr_Opd *zero_addr = new Register_Addr_Opd(machine_desc_object.spim_register_table[zero]);

		string label1 = get_new_label();
		string label2 = get_new_label();
		Control_Flow_IC_Stmt * cond_stmt = new Control_Flow_IC_Stmt(bne,cond_addr,zero_addr,label1);
		Control_Flow_IC_Stmt * jump_stmt = new Control_Flow_IC_Stmt(j,NULL,NULL,label2);
		Label_IC_Stmt * label1_stmt = new Label_IC_Stmt(label,label1);
		Label_IC_Stmt * label2_stmt = new Label_IC_Stmt(label,label2);

		Code_For_Ast & body_stmt_code = body->compile();
		Register_Descriptor * body_load_register = body_stmt_code.get_reg();


		if(init_load_register!=NULL)	init_load_register->reset_use_for_expr_result();
		if(cond_load_register!=NULL)	cond_load_register->reset_use_for_expr_result();
		if(update_load_register!=NULL)	update_load_register->reset_use_for_expr_result();
		if(body_load_register!=NULL) 	body_load_register->reset_use_for_expr_result();


		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (init_stmt_code.get_icode_list().empty() == false)
			ic_list = init_stmt_code.get_icode_list();
		ic_list.push_back(jump_stmt);	
		ic_list.push_back(label1_stmt);	
		if (body_stmt_code.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), body_stmt_code.get_icode_list());
		if (update_stmt_code.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), update_stmt_code.get_icode_list());
		ic_list.push_back(label2_stmt);	
		if (cond_stmt_code.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), cond_stmt_code.get_icode_list());
		ic_list.push_back(cond_stmt);	

		Code_For_Ast * iteration_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			iteration_stmt = new Code_For_Ast(ic_list, NULL);		//GOWTHAM :

		return *iteration_stmt;	}

}

///////////////////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Plus_Ast::compile()
{
	
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Relational_Expr_Ast");


	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_load_register = lhs_stmt.get_reg();
	// if(lhs_load_register!=NULL) 	lhs_load_register->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_load_register = rhs_stmt.get_reg();
	// if(rhs_load_register!=NULL) 	rhs_load_register->set_use_for_expr_result();

	//GOWTHAM : check NULL

	Register_Addr_Opd *lhs_addr = new Register_Addr_Opd(lhs_load_register);
	Register_Addr_Opd *rhs_addr = new Register_Addr_Opd(rhs_load_register);

	Tgt_Op ic_operator;	

	if(node_data_type == int_data_type){
	    const Register_Use_Category reg_type = gp_data;
		ic_operator = add;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		ic_list.push_back(stmt);	
		Code_For_Ast * plus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			plus_stmt = new Code_For_Ast(ic_list, load_register);
		return *plus_stmt;
	}
	else if(node_data_type == double_data_type){
		ic_operator = add_d;
		const Register_Use_Category reg_type = float_reg;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		ic_list.push_back(stmt);	
		Code_For_Ast * plus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			plus_stmt = new Code_For_Ast(ic_list, load_register);
		return *plus_stmt;
	}
	
}

/////////////////////////////////////////////////////////////////

Code_For_Ast & Minus_Ast::compile()
{	
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Relational_Expr_Ast");

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_load_register = lhs_stmt.get_reg();
	// if(lhs_load_register!=NULL) 	lhs_load_register->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_load_register = rhs_stmt.get_reg();
	// if(rhs_load_register!=NULL) 	rhs_load_register->set_use_for_expr_result();
	//GOWTHAM : check NULL

	Register_Addr_Opd *lhs_addr = new Register_Addr_Opd(lhs_load_register);
	Register_Addr_Opd *rhs_addr = new Register_Addr_Opd(rhs_load_register);

	Tgt_Op ic_operator;	

	if(node_data_type == int_data_type){
	    const Register_Use_Category reg_type = gp_data;
		ic_operator = sub;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		ic_list.push_back(stmt);	
		Code_For_Ast * plus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			plus_stmt = new Code_For_Ast(ic_list, load_register);
		return *plus_stmt;
	}
	else if(node_data_type == double_data_type){
		ic_operator = sub_d;
		const Register_Use_Category reg_type = float_reg;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		ic_list.push_back(stmt);	
		Code_For_Ast * plus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			plus_stmt = new Code_For_Ast(ic_list, load_register);
		return *plus_stmt;
	}

}

//////////////////////////////////////////////////////////////////

Code_For_Ast & Mult_Ast::compile()
{
	
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Relational_Expr_Ast");

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_load_register = lhs_stmt.get_reg();
	// if(lhs_load_register!=NULL) 	lhs_load_register->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_load_register = rhs_stmt.get_reg();
	// if(rhs_load_register!=NULL) 	rhs_load_register->set_use_for_expr_result();
	//GOWTHAM : check NULL

	Register_Addr_Opd *lhs_addr = new Register_Addr_Opd(lhs_load_register);
	Register_Addr_Opd *rhs_addr = new Register_Addr_Opd(rhs_load_register);

	Tgt_Op ic_operator;	

	if(node_data_type == int_data_type){
	    const Register_Use_Category reg_type = gp_data;
		ic_operator = mult;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		ic_list.push_back(stmt);	
		Code_For_Ast * plus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			plus_stmt = new Code_For_Ast(ic_list, load_register);
		return *plus_stmt;
	}
	else if(node_data_type == double_data_type){
		ic_operator = mult_d;
		const Register_Use_Category reg_type = float_reg;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		ic_list.push_back(stmt);	
		Code_For_Ast * plus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			plus_stmt = new Code_For_Ast(ic_list, load_register);
		return *plus_stmt;
	}

}

////////////////////////////////////////////////////////////////////

Code_For_Ast & Conditional_Operator_Ast::compile()
{
	CHECK_INVARIANT((cond != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	CHECK_INVARIANT((rhs != NULL), "Lhs cannot be null in Relational_Expr_Ast");

	Code_For_Ast & cond_stmt_code = cond->compile();
	Register_Descriptor * cond_load_register = cond_stmt_code.get_reg();
	// if(cond_load_register!=NULL) cond_load_register->set_use_for_expr_result();

	Code_For_Ast & lhs_stmt_code = lhs->compile();
	Register_Descriptor * lhs_load_register = lhs_stmt_code.get_reg();
	// if(lhs_load_register!=NULL) lhs_load_register->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt_code = rhs->compile();
	Register_Descriptor * rhs_load_register = rhs_stmt_code.get_reg();
	// if(rhs_load_register!=NULL) rhs_load_register->set_use_for_expr_result();


	Register_Descriptor * zero_register = machine_desc_object.spim_register_table[zero];
	Register_Descriptor * temp_register;

	if(node_data_type==int_data_type){
		temp_register = machine_desc_object.get_new_register<gp_data>();	
		// if(temp_register!=NULL)	temp_register->set_use_for_expr_result();
	}
	else if(node_data_type ==double_data_type){
		temp_register = machine_desc_object.get_new_register<float_reg>();	
		// if(temp_register!=NULL)	temp_register->set_use_for_expr_result();
	}

	if(cond_load_register!=NULL) cond_load_register->reset_use_for_expr_result();
	if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();
	if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();

	Register_Addr_Opd *cond_addr = new Register_Addr_Opd(cond_load_register);
	Register_Addr_Opd *zero_addr = new Register_Addr_Opd(zero_register);
	Register_Addr_Opd *lhs_addr = new Register_Addr_Opd(lhs_load_register);
	Register_Addr_Opd *rhs_addr = new Register_Addr_Opd(rhs_load_register);
	Register_Addr_Opd *temp = new Register_Addr_Opd(temp_register);

	string label1 = get_new_label();
	string label2 = get_new_label();
	Control_Flow_IC_Stmt * cond_stmt = new Control_Flow_IC_Stmt(beq,cond_addr,zero_addr,label1);
	Control_Flow_IC_Stmt * jump_stmt = new Control_Flow_IC_Stmt(j,NULL,NULL,label2);

	Compute_IC_Stmt * temp1_stmt = new Compute_IC_Stmt(or_t,lhs_addr,zero_addr,temp);
	Compute_IC_Stmt * temp2_stmt = new Compute_IC_Stmt(or_t,rhs_addr,zero_addr,temp);

	Label_IC_Stmt * label1_stmt = new Label_IC_Stmt(label,label1);
	Label_IC_Stmt * label2_stmt = new Label_IC_Stmt(label,label2);

	list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
	if (cond_stmt_code.get_icode_list().empty() == false)
		ic_list = cond_stmt_code.get_icode_list();
	ic_list.push_back(cond_stmt);	
	if (lhs_stmt_code.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), lhs_stmt_code.get_icode_list());
	ic_list.push_back(temp1_stmt);	
	ic_list.push_back(jump_stmt);	
	ic_list.push_back(label1_stmt);	
	if (rhs_stmt_code.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt_code.get_icode_list());
	ic_list.push_back(temp2_stmt);	
	ic_list.push_back(label2_stmt);	

	Code_For_Ast * conditional_stmt = new Code_For_Ast();
	if (ic_list.empty() == false)
		conditional_stmt = new Code_For_Ast(ic_list, temp_register);		//GOWTHAM :

	return *conditional_stmt;

}


////////////////////////////////////////////////////////////////////

Code_For_Ast & Divide_Ast::compile()
{
	
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Relational_Expr_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Relational_Expr_Ast");

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_load_register = lhs_stmt.get_reg();
	// if(lhs_load_register!=NULL) 	lhs_load_register->set_use_for_expr_result();

	Code_For_Ast & rhs_stmt = rhs->compile();
	Register_Descriptor * rhs_load_register = rhs_stmt.get_reg();
	// if(rhs_load_register!=NULL) 	rhs_load_register->set_use_for_expr_result();
	//GOWTHAM : check NULL

	Register_Addr_Opd *lhs_addr = new Register_Addr_Opd(lhs_load_register);
	Register_Addr_Opd *rhs_addr = new Register_Addr_Opd(rhs_load_register);

	Tgt_Op ic_operator;	

	if(node_data_type == int_data_type){
	    const Register_Use_Category reg_type = gp_data;
		ic_operator = divd;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		ic_list.push_back(stmt);	
		Code_For_Ast * plus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			plus_stmt = new Code_For_Ast(ic_list, load_register);
		return *plus_stmt;
	}
	else if(node_data_type == double_data_type){
		ic_operator = div_d;
		const Register_Use_Category reg_type = float_reg;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();
		if(rhs_load_register!=NULL) rhs_load_register->reset_use_for_expr_result();

		// cout<<load_register->get_name()<<endl;

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Compute_IC_Stmt * stmt = new Compute_IC_Stmt(ic_operator,lhs_addr,rhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		if (rhs_stmt.get_icode_list().empty() == false)
			ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());
		ic_list.push_back(stmt);	
		Code_For_Ast * plus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			plus_stmt = new Code_For_Ast(ic_list, load_register);
		return *plus_stmt;
	}

}


//////////////////////////////////////////////////////////////////////

Code_For_Ast & UMinus_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Relational_Expr_Ast");

	Code_For_Ast & lhs_stmt = lhs->compile();
	Register_Descriptor * lhs_load_register = lhs_stmt.get_reg();
	// if(lhs_load_register!=NULL) 	lhs_load_register->set_use_for_expr_result();

	//GOWTHAM : check NULL

	Register_Addr_Opd *lhs_addr = new Register_Addr_Opd(lhs_load_register);
	Tgt_Op ic_operator;	

	if(node_data_type == int_data_type){
		ic_operator = uminus;
		const Register_Use_Category reg_type = gp_data;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		// if(load_register!=NULL) load_register->set_use_for_expr_result();
		if(lhs_load_register!=NULL) lhs_load_register->reset_use_for_expr_result();

		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Move_IC_Stmt * stmt = new Move_IC_Stmt(ic_operator,lhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		ic_list.push_back(stmt);	
		Code_For_Ast * uminus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			uminus_stmt = new Code_For_Ast(ic_list, load_register);
		return *uminus_stmt;
	}
	else if(node_data_type == double_data_type){
		ic_operator = uminus_d;
		const Register_Use_Category reg_type = float_reg;
		Register_Descriptor* load_register =  machine_desc_object.get_new_register<reg_type>();	
		if(lhs_load_register!=NULL)	lhs_load_register->reset_use_for_expr_result();
		Register_Addr_Opd * reg = new Register_Addr_Opd(load_register);
		Move_IC_Stmt * stmt = new Move_IC_Stmt(ic_operator,lhs_addr,reg);
		list<Icode_Stmt *> & ic_list = * new list<Icode_Stmt *>;
		if (lhs_stmt.get_icode_list().empty() == false)
			ic_list = lhs_stmt.get_icode_list();
		ic_list.push_back(stmt);	
		Code_For_Ast * uminus_stmt = new Code_For_Ast();
		if (ic_list.empty() == false)
			uminus_stmt = new Code_For_Ast(ic_list, load_register);
		return *uminus_stmt;
	}
}

//////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Sequence_Ast::compile()
{

	for(std::list<Ast *>::iterator it = statement_list.begin();it!=statement_list.end();it++){
		Code_For_Ast & stmt = (*it)->compile();
	
		// cout<<"pllp"<<endl;

		Register_Descriptor * reg = stmt.get_reg();

		if(reg != NULL)	reg->reset_use_for_expr_result();

		if (stmt.get_icode_list().empty() == false){

			sa_icode_list.splice(sa_icode_list.end(), stmt.get_icode_list());

		}

	}

	Code_For_Ast * seq_stmt = new Code_For_Ast();
	if (sa_icode_list.empty() == false){
		seq_stmt = new Code_For_Ast(sa_icode_list, NULL);
	}
	return *seq_stmt;
}

void Sequence_Ast::print_assembly(ostream & file_buffer)
{
	for(list<Icode_Stmt *>::iterator it = sa_icode_list.begin() ; it != sa_icode_list.end() ; it++)
	{
		(*it)->print_assembly(file_buffer);
	}
}

void Sequence_Ast::print_icode(ostream & file_buffer)
{

	for(list<Icode_Stmt *>::iterator it = sa_icode_list.begin() ; it != sa_icode_list.end() ; it++)
	{
		if((*it)!=NULL)
			(*it)->print_icode(file_buffer);

	}
		
}

//////////////////////////////////////////////////////////////////////////////

template class Number_Ast<double>;
template class Number_Ast<int>;
