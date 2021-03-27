#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

class SyntaxAnalyzer{
    private:
        vector<string> lexemes;
        vector<string> tokens;
        vector<string>::iterator lexitr;
        vector<string>::iterator tokitr;

        // other private methods
        bool vdec();
        int vars();
        bool stmtlist();
        int stmt();
        bool ifstmt();
        bool elsepart();
        bool whilestmt();
        bool assignstmt();
        bool inputstmt();
        bool outputstmt();
        bool expr();
        bool simpleexpr();
        bool term();
        bool logicop();
        bool arithop();
        bool relop();
        std::istream& getline_safe(std::istream& input, std::string& output);
    public:
        SyntaxAnalyzer(istream& infile);
        // pre: 1st parameter consists of an open file containing a source code's
        //	valid scanner output.  This data must be in the form
        //			token : lexeme
        // post: the vectors have been populated

        bool parse();
        // pre: none
        // post: The lexemes/tokens have been parsed.
        // If an error occurs, a message prints indicating the token/lexeme pair
        // that caused the error.  If no error, data is loaded in vectors
};
SyntaxAnalyzer::SyntaxAnalyzer(istream& infile){
    string line, tok, lex;
    int pos;
    getline_safe(infile, line);
    bool valid = true;
    while(!infile.eof() && (valid)){ // valid never updated in this loop?
        pos = line.find(":");
        tok = line.substr(0, pos-1); // changed from pos to pos-1 to account for the space MAKE SURE TO CHANGE THIS BACK
        lex = line.substr(pos+1, line.length()); // HERE TOO!
        cout << pos << " " << tok << " " << lex << endl;
        tokens.push_back(tok);
        lexemes.push_back(lex);
        getline_safe(infile, line);
    }
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}

bool SyntaxAnalyzer::parse(){
    if (vdec()){
        if (tokitr!=tokens.end() && *tokitr=="t_main"){ 
            tokitr++; lexitr++;
	    // David Rudenya -- removed redundant null iterator check
            if (stmtlist()){ 
            	if (tokitr!=tokens.end()) // should be at end token
                	if (*tokitr == "t_end"){
                		tokitr++; lexitr++;
                		if (tokitr==tokens.end()){  // end was last thing in file
                			cout << "Valid source code file" << endl;
                			return true;
                		}
                		else{
                			cout << "end came too early" << endl;
                		}
                	}
                	else{
                		cout << "invalid statement ending code" << endl;
				cout << *tokitr << endl;
                }
                else{
                	cout << "no end" << endl;
                }
            }
            else{
            	cout << "bad/no stmtlist" << endl;
            }
        }
        else{
        	cout << "no main" << endl;
        }
    }
    else{
    	cout << "bad var list" << endl;
    }
    return false;

}

bool SyntaxAnalyzer::vdec(){
    // David Rudenya - added a check to ensure that tokitr is not null
    if (tokitr == tokens.end())
	    return false;

    if (*tokitr != "t_var") // vdec can be null
        return true; 
    else{
        tokitr++; lexitr++;
        int result = 0;   // 0 - valid, 1 - done, 2 - error
        result = vars();
        if (result == 2)
            return false;
        while (result == 0){ // maybe include the if in the while condition?
            if (tokitr!=tokens.end()) // the entirety of this loop might be unnecessary; vars() looks for ALL variables
                result = vars(); // parse vars
        }

        if (result == 1)
            return true;
        else
            return false;
    }
}

int SyntaxAnalyzer::vars(){
    int result = 0;  // 0 - valid, 1 - done, 2 - error
    string temp;
    if (*tokitr == "t_integer"){
        temp = "t_integer";
        tokitr++; lexitr++;
    }
    else if (*tokitr == "t_string"){
        temp = "t_string";
        tokitr++; lexitr++;
    }
    else 
        return 1; // what if we run into non-id terms?
    bool semihit = false;
    while (tokitr != tokens.end() && result == 0 && !semihit){
        if (*tokitr == "t_id"){
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_comma"){
                tokitr++; lexitr++;
            }
            else if (tokitr != tokens.end() && *tokitr == "s_semi"){
                semihit = true;
                tokitr++; lexitr++;
		
		// David Rudenya -- updated result to reflect hitting a semicolon
		// and when there is another list of variables to check
		if (tokitr != tokens.end() && (*tokitr == "t_string" || *tokitr == "t_integer"))
		{
			semihit = false;
			tokitr++; lexitr++;
		}
		else 
			result = 1;
            }
            else
                result = 2;
        }
        else{
            result = 2;
        }
    }
    return result;
}

bool SyntaxAnalyzer::stmtlist(){
    int result = stmt();
    while (result == 1){
    	result = stmt();
    }

    if (result == 0)
        return false;
    else
        return true;
}
int SyntaxAnalyzer::stmt(){  // returns 1 or 2 if valid, 0 if invalid
	// David Rudenya -- added check to ensure tokitr is not null
	if (tokitr == tokens.end())
		return false;

	if (*tokitr == "t_if"){
        tokitr++; lexitr++;
        if (ifstmt()) return 1;
	else return 0;
	
    }
    else if (*tokitr == "t_while"){
        tokitr++; lexitr++;
        if (whilestmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_id"){  // assignment starts with identifier
        tokitr++; lexitr++;
        cout << "t_id" << endl;
        if (assignstmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_input"){
        tokitr++; lexitr++;
        if (inputstmt()) return 1;
        else return 0;
    }
    else if (*tokitr == "t_output"){
        tokitr++; lexitr++;
        cout << "t_output" << endl;
        if (outputstmt()) return 1;
	else return 0;
    }
    return 2;  //stmtlist can be null
}

bool SyntaxAnalyzer::ifstmt(){ 
	// pre: none
	// post: the source code has been analyzed for a valid if-statement
	// 	 returns true if valid; false if not
	// desc: David Rudenya's version of the method, not the in-class one	
	if (tokitr != tokens.end())
	{
		if (*tokitr == "s_lparen")
		{
			tokitr++; lexitr++;
			if (expr()) 
			{ 
				if (tokitr != tokens.end() && *tokitr == "s_rparen")
				{
					tokitr++; lexitr++;
					if (tokitr != tokens.end() && *tokitr == "t_then")
					{
						tokitr++; lexitr++;
						if (tokitr != tokens.end() && stmtlist())
						{
							if (tokitr != tokens.end() && elsepart())
							// the methods called by elsepart() will increment if necessary; otherwise, if
							// elsepart() is just empty, don't increment
							{
								if (tokitr != tokens.end() && *tokitr == "t_end")
								{
									tokitr++; lexitr++;
									if (tokitr != tokens.end() && *tokitr == "t_if")
									{
										tokitr++; lexitr++;
										return true;
									}
								}
							}
						}
					}
				}
			}

		}
	}	

	return false;
}

bool SyntaxAnalyzer::elsepart(){
    if (*tokitr == "t_else"){
        tokitr++; lexitr++;
        if (stmtlist())
            return true;
        else
            return false;
    }
    return true;   // elsepart can be null
}

bool SyntaxAnalyzer::whilestmt()
	// pre: none
	// post: source code analyzed for a valid while-statement
	// desc: written by David Rudenya; assumes invalid statement unless
	//       conditions to be a valid while-statement are met
{
	if (tokitr != tokens.end() && *tokitr == "s_lparen")
	{
		tokitr++; lexitr++;
		if (expr())
		{
			if (tokitr != tokens.end() && *tokitr == "s_rparen")
			{
				tokitr++; lexitr++;
				if (tokitr != tokens.end() && *tokitr == "t_loop")
				{
					tokitr++; lexitr++;
					if (stmtlist())
					{
						if (tokitr != tokens.end() && *tokitr == "t_end")
						{
							tokitr++; lexitr++;
							if (tokitr != tokens.end() && *tokitr == "t_loop")
							{
								tokitr++; lexitr++;
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool SyntaxAnalyzer::assignstmt()
	// pre: none
	// post: the source code has been checked for a valid assignment statement
	//       returns true if valid; false if not
	// desc: written by David Rudenya; assumes the statement is not valid
	//       unless it meets all requirements to be valid
{
	if (tokitr != tokens.end() && *tokitr == "s_assign")
	{
		tokitr++; lexitr++;
		if (tokitr != tokens.end() && *tokitr == "s_assign")
		{
			tokitr++; lexitr++;
			if (tokitr != tokens.end() && expr())
			{
				if (tokitr != tokens.end() && *tokitr == "s_semi")
				{
					tokitr++; lexitr++;
					return true;
				}
			}
		}
	}
	return false;
}
bool SyntaxAnalyzer::inputstmt(){
    if (*tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if (*tokitr == "t_id"){
            tokitr++; lexitr++;
            if (*tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
}

bool SyntaxAnalyzer::outputstmt()
	// pre: none
	// post: the source code has been checked for a valid output statement
	// desc: written by David Rudenya; assumes the output statement is
	//       invalid unless it passes all tests to be valid
{
	if (tokitr != tokens.end() && *tokitr == "s_lparen")
	{
		tokitr++; lexitr++;
		if (tokitr != tokens.end())
		{
			if (*tokitr == "t_str") 
			{
				tokitr++; lexitr++; 
			} 
			// expr() updates the tokitr/lexitr, but if it returns false, immediately exit the function
			else if (!expr()) 
				return false;


			// if the program reaches this point, either a valid string
			// or expression was found
			if (tokitr != tokens.end() && *tokitr == "s_rparen")
			{
				tokitr++; lexitr++;
				return true;
			}
		}
	}	
	
	return false;
}

bool SyntaxAnalyzer::expr(){
    if (simpleexpr()){
	if (logicop()){
		if (simpleexpr())
			return true;
		else
			return false;
	}
	else
		return true;
    }
    else{
	return false;

    }
}

bool SyntaxAnalyzer::simpleexpr()
	// pre: none
	// post: source code has been checked for a valid simple expression
	// desc: written by David Rudenya; assumes the expression is invalid
	//       unless it passes all tests to be considered valid
{
	if (term())
	{
		if (arithop() || relop())
		{	
			
			if (term())
			{
				return true;
			} else
				return false;
			 
		} else
		{
			return true;
		}
	}
	return false;
}

bool SyntaxAnalyzer::term(){
    // David Rudenya - added a check to ensure tokitr is not null
    if (tokitr != tokens.end() && (
        (*tokitr == "t_int")
	|| (*tokitr == "t_str")
	|| (*tokitr == "t_id"))){
    	tokitr++; lexitr++;
    	return true;
    }
    else if (tokitr != tokens.end() && *tokitr == "s_lparen"){
            tokitr++; lexitr++;
            if (expr())
	    { // David Rudenya -- added open/close brackets
                if (*tokitr == "s_rparen"){
                    tokitr++; lexitr++;
                    return true;
                }
	    }
        }
    return false;
}

bool SyntaxAnalyzer::logicop(){
    // David Rudenya -- added a check to ensure tokitr is not null
    if (tokitr != tokens.end() && ((*tokitr == "s_and") || (*tokitr == "s_or"))){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

bool SyntaxAnalyzer::arithop(){
    // David Rudenya -- added check to ensure tokitr is not null
    if (tokitr != tokens.end() &&  ((*tokitr == "s_mult") || (*tokitr == "s_plus") || (*tokitr == "s_minus")
        || (*tokitr == "s_div")	|| (*tokitr == "s_mod"))){
        tokitr++; lexitr++;
        return true;
    }
    else
        return false;
}

bool SyntaxAnalyzer::relop(){
    // David Rudenya -- added check to ensure tokitr != tokens.end()
    if (tokitr != tokens.end() &&  ((*tokitr == "s_lt") || (*tokitr == "s_gt") || (*tokitr == "s_ge")
        || (*tokitr == "s_eq") || (*tokitr == "s_ne") || (*tokitr == "s_le"))){
        tokitr++; lexitr++;
        return true;
    }
    else
    	return false;
}
std::istream& SyntaxAnalyzer::getline_safe(std::istream& input, std::string& output)
{
    char c;
    output.clear();

    input.get(c);
    while (input && c != '\n')
    {
        if (c != '\r' || input.peek() != '\n') // should probably be &&, not ||
        {
            output += c;
        }
        input.get(c);
    }

    return input;
}

int main(){
    ifstream infile("output.txt");
    if (!infile){
    	cout << "error opening lexemes.txt file" << endl;
        exit(-1);
    }
    SyntaxAnalyzer sa(infile);
    sa.parse();
    return 1;
}
