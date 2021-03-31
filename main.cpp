/*
CSCE 306 Program 3 (Dr. St. Clair)
David Rudenya
*/


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
    // David Rudenya -- Deleted unused "valid" variable
    while(!infile.eof()){ 
        pos = line.find(":");
        tok = line.substr(0, pos); 
        lex = line.substr(pos+1, line.length());  
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

		// David Rudenya -- added a check below to see if tokitr  has hit
		// the "\0", which should be the very last line; if either of the
		// below conditions are false, then end was not found before
		// hitting either the end of the file or finding the empty line
		// that shouid have come after end; also added braces to ensure
		// that, if the below if-statement is false, it lines up
		// with the else clause printing "no end"
            	if (tokitr!=tokens.end() && *tokitr != "\0") // should be at end token
		{
                	if (*tokitr == "t_end"){
                		tokitr++; lexitr++;
				// David Rudenya -- reformated the below if-statement
				// to see if the empty line has been found (which
				// should come immediately after end and is the
				// last line in the file) instead of checking
				// if the tokitr has reached tokens.end(); this
				// prevents the program from reading the empty
				// line that comes after end in the input as
				// a statement after end
				if (tokitr != tokens.end() && *tokitr == "\0") // end was the last thing in file
				{
					cout << "Valid source code file" << endl;
					return true;

				} 
				else{
					cout << "end came too early" << endl;
				}
                	}
                	else{
                		cout << "invalid statement ending code" << endl;
                	}
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
	// David Rudenya -- moved if statement into while condition
        while (result == 0 && tokitr != tokens.end()){ 
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

    // David Rudenya -- removed the unused string variable "temp" and combined the
    // if statements into one
    if (tokitr != tokens.end() && (*tokitr == "t_integer" || *tokitr == "t_string"))
    {
	    tokitr++; lexitr++;
    } 
    else
	    return 1; 
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
						if (stmtlist())
						{
							if (elsepart())
							// the methods called by elsepart() will increment if necessary; otherwise, if
							// elsepart is empty in the input file, won't increment
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
    // David Rudenya -- added a check to ensure tokitr not null
    if (tokitr != tokens.end() && *tokitr == "t_else"){
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
		if (tokitr != tokens.end() && expr())
		{
			if (tokitr != tokens.end() && *tokitr == "s_semi")
			{
				tokitr++; lexitr++;
				return true;
			}
		}
	}
	
	return false;
}
bool SyntaxAnalyzer::inputstmt(){
    // David Rudenya -- added checks to ensure tokitr is not null below
    if (tokitr != tokens.end() && *tokitr == "s_lparen"){
        tokitr++; lexitr++;
        if (tokitr != tokens.end () && *tokitr == "t_id"){
            tokitr++; lexitr++;
            if (tokitr != tokens.end() && *tokitr == "s_rparen"){
                tokitr++; lexitr++;
                return true;
            }
        }
    }
    return false;
}

bool SyntaxAnalyzer::outputstmt()
	// pre: none
	// post: the source code has been checked for a valid output statement;
	//       returns true if valid, false if not
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
	//       returns true if valid, false if not
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
	    { // David Rudenya -- added open/close braces
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
        if (c != '\r' || input.peek() != '\n') 
        {
            output += c;
        }
        input.get(c);
    }

    return input;
}

int main(){
    ifstream infile("output.txt"); // remember to change this to correct file name!
    if (!infile){
    	cout << "error opening lexemes.txt file" << endl;
        exit(-1);
    }
    SyntaxAnalyzer sa(infile);
    sa.parse();
    return 1;
}
