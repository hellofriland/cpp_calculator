#include <iostream>
#include <math.h>
#include <ostream>
#include <string>
#include <vector>

using namespace std;

struct Token {
public:
    string Type;
    string Value;

    Token(string type, string value) {
        Type = type;
        Value = value;
    }
};

struct CilLineElem {
public:
    string Ln;
    string Op;
    string Rn;
};


class Common {
public:
    static vector<string> split(string text, char ch) {
        vector<string> vec;
        string sub_text;

        for (int i = 0; i < text.length(); i++) {
            while (text[i] == ch) {
                sub_text = text.substr(0, i);
                vec.push_back(sub_text);
                text = text.substr(i + 1);
                i = 0;
            }
        }
        vec.push_back(text);

        return vec;
    }

    static bool isLetter(char ch) {
        if (ch >= 'a' && ch <= 'z') {
            return true;
        } else if (ch >= 'A' && ch <= 'Z') {
            return true;
        }

        return false;
    }

    static bool isDigitOrLetter(char ch) {
        if (isdigit(ch) || isLetter(ch)) {
            return true;
        }

        return false;
    }

    static bool isOperator(string ch) {
        vector<string> myOperator = {"+", "-", "*", "/", "^", "="};
        for (int i = 0; i < myOperator.size(); i++) {
            if (ch == myOperator[i]) {
                return true;
            }
        }

        return false;
    }

    static bool IsOperator(char ch) {
        vector<char> myOperator = {'+', '-', '*', '/', '^', '='};
        for (int i = 0; i < myOperator.size(); i++) {
            if (ch == myOperator[i]) {
                return true;
            }
        }

        return false;
    }

    static bool isSeparator(char ch) {
        if (ch == '(' || ch == ')') {
            return true;
        }
        return false;
    }

    static bool isSeparator(string ch) {
        if (ch == "(" || ch == ")") {
            return true;
        }
        return false;
    }
};


class LexicalParser {
public:
    static vector<string> generateLexicalList(string text) {
		cout << "\n\n-----------------> Generate Lexical List" << endl;
        vector<string> lexicalList;
        string sub_text = "";
        int sub_index = 0;

        for (int i = 0; i < text.length();) {
            sub_text = text.substr(0, 1);

            if (Common::isOperator(sub_text)) {
                text = text.substr(1);
                lexicalList.push_back("Op," + sub_text);
            } else if (Common::isSeparator(sub_text)) {
                text = text.substr(1);
                lexicalList.push_back("Sep," + sub_text);
            } else {
                if (isdigit(sub_text[0]) == false) {
                    cout << "UnKnowSymbol" << endl;
                }

                while (i < text.length() && isdigit(text[i])) {
                    sub_index++;
                    i++;
                }

                sub_text = text.substr(0, sub_index);
                text = text.substr(sub_index);
                lexicalList.push_back("Num," + sub_text);
            }

            i = 0;
            sub_index = 0;
        } // for (int i=0; i < text.length(); i++)
        cout << "\n------> Completed: Generate Lexical List" << endl;
		printLexicalList(lexicalList);
        lexicalList.push_back("Eoi,Eoi");
        return lexicalList;
    } // GenerateLexicalList(string text)

    static void printLexicalList(vector<string> lexicalList) {
        cout << "\n---------> Print Lexical List <---------" << endl;
        for (int i = 0; i < lexicalList.size(); i++) {
            cout << lexicalList[i] << endl;
        }
    }
}; // class LexicalParser


class CIL {
private:
    vector<string> cilList;
    vector<Token> lexicalList;
    int lexicalListPos = 0;

    vector<string> idPool = {"id1", "id2", "id3", "id4", "id5", "id6", "id7", "id8", "id9"};
    int idPoolPos = 0;

    Token currentToken = {"Null", "Null"};

    string GetId() {
        if (idPoolPos >= idPool.size()) {
            cout << "IdPool is exhausted" << endl;
        }

        string id = idPool[idPoolPos];
        idPoolPos++;
        return id;
    }

    void ReleaseId(string id) {
        if (idPoolPos > 0) {
            idPoolPos--;
        }
    }

    void convertToToken(vector<string> lexicalText) {
        cout << "\n----------------------> Convert To Token" << endl;
        for (int i = 0; i < lexicalText.size(); i++) {
            vector<string> str = Common::split(lexicalText[i], ',');
            lexicalList.push_back(Token(str[0], str[1]));
        }
        cout << "\n-----------> Completed: Convert To Token" << endl;
        printAllToken();
    }

    void printAllToken() {
        cout << "\n----------> Print Token List <----------" << endl;
        for (int i = 0; i < lexicalList.size(); i++) {
            cout << "Type: " << lexicalList[i].Type << "\t";
            cout << "Value: " << lexicalList[i].Value << endl;
        }
    }


    void nextToken() {
        currentToken = lexicalList[lexicalListPos];
        lexicalListPos++;
        cout << "--> Next Token: ";
        cout << "Type: " << currentToken.Type << "\t";
        cout << "Value: " << currentToken.Value << endl;
    }

    void myStatements() {
        string id = GetId();
        myExpression(id);

        while ((currentToken.Type == "Eoi") == false) {
            myExpression(id);
            ReleaseId(id);
        }
    }

    void myExpression(string tempId) {
        string id;
        myTerm(tempId);

        if (currentToken.Type == "Op") {
            while (currentToken.Value == "+") {
                nextToken();
                id = GetId();
                myTerm(id);
                cilList.push_back(tempId + "+=" + id);
                ReleaseId(id);
            }

            while (currentToken.Value == "-") {
                nextToken();
                id = GetId();
                myTerm(id);
                cilList.push_back(tempId + "-=" + id);
                ReleaseId(id);
            }
        }

    }

    void myTerm(string tempId) {
        string id;
        myFactor(tempId);

        if (currentToken.Type == "Op") {
            while (currentToken.Value == "*") {
                nextToken();
                id = GetId();
                myFactor(id);
                cilList.push_back(tempId + "*=" + id);
                ReleaseId(id);
            }

            while (currentToken.Value == "/") {
                nextToken();
                id = GetId();
                myFactor(id);
                cilList.push_back(tempId + "/=" + id);
                ReleaseId(id);
            }

            while (currentToken.Value == "^") {
                nextToken();
                id = GetId();
                myFactor(id);
                cilList.push_back(tempId + "^" + id);
                ReleaseId(id);
            }
        }
    }

    void myFactor(string tempId) {
        if (currentToken.Type == "Num") {
            cilList.push_back(tempId + "=" + currentToken.Value);
            nextToken();
        } else if (currentToken.Type == "Sep") {
            if (currentToken.Value == "(") {
                nextToken();
                myExpression(tempId);

                if (currentToken.Value == ")") {
                    nextToken();
                }

            }
        } // else if (currentToken.Type == "Separator")
    } // MyFactor(string tempId)

public:

    vector<string> generateCIL(vector<string> lexicalText) {
        convertToToken(lexicalText);
        cout << "\n--------------------------> Generate CIL" << endl;
        nextToken();
        myStatements();
        cout << "\n---------------->Completed: Generate CIL" << endl;

		printAllCIL();

        return cilList;
    }

    void printAllCIL() {
        cout << "\n-----------> Print CIL List <-----------" << endl;
        for (int i = 0; i < cilList.size(); i++) {
            cout << cilList[i] << endl;
        }
        cout << endl;
    }





/*CILRun*/

private:
    struct IdValue {
    public:
        string Id;
        double Value;
    };

    vector<IdValue> idValueList;

    int searchIdPos(string id) {
        for (int i = 0; i < idValueList.size(); i++) {
            if (idValueList[i].Id == id) {
                return i;
            }
        }

        return -1;
    }

    bool existId(string id) {
        for (int i = 0; i < idValueList.size(); i++) {
            if (idValueList[i].Id == id) {
                return true;
            }
        }

        return false;
    }

    void setValue(string id, double value) {
        cout << "SetValue:\t";
        cout << "Id - " << idValueList[searchIdPos(id)].Id << "  ";
        cout << "Value - " << idValueList[searchIdPos(id)].Value;
        idValueList[searchIdPos(id)].Value = value;
        cout << " --> " << idValueList[searchIdPos(id)].Value << endl;
    }

    double getIdValue(string id) {
        cout << "GetIdValue:\t";
        cout << "Id - " << idValueList[searchIdPos(id)].Id << "  ";
        cout << "Value - " << idValueList[searchIdPos(id)].Value << endl;
        return idValueList[searchIdPos(id)].Value;
    }

    void addData(string id, double value) {
        IdValue idv = {id, value};
        idValueList.push_back(idv);
        cout << "AddData:\t";
        cout << "Id - " << idValueList[searchIdPos(id)].Id << "  ";
        cout << "Value - " << idValueList[searchIdPos(id)].Value << endl;
    }

    void removeData(string id) {
        cout << "RemoveData:\t";
        cout << "Id - " << idValueList[searchIdPos(id)].Id << endl;
        idValueList.erase(idValueList.begin() + searchIdPos(id));
    }


    CilLineElem splitLine(string line) {
        CilLineElem lineElem;
        int sub_line_index = 0;

        for (int i = 0; i < line.length(); i++) {
            while (i < line.length() && Common::isDigitOrLetter(line[i])) {
                i++;
                sub_line_index++;
            }

            lineElem.Ln = line.substr(0, sub_line_index);
            line = line.substr(sub_line_index);
            i = 0;
            sub_line_index = 0;


            while (i < line.length() && Common::IsOperator(line[i])) {
                i++;
                sub_line_index++;
            }

            lineElem.Op = line.substr(0, sub_line_index);
            line = line.substr(sub_line_index);
            i = 0;
            sub_line_index = 0;

            while (i < line.length() && Common::isDigitOrLetter(line[i])) {
                i++;
                sub_line_index++;
            }

            lineElem.Rn = line.substr(0, sub_line_index);
            line = line.substr(sub_line_index);
            i = 0;
            sub_line_index = 0;
        }

        return lineElem;
    }

public:
    void PrintIdValueList() {
        for (int i = 0; i < idValueList.size(); i++) {
            cout << "Id: " << idValueList[i].Id << "\t";
            cout << "Value: " << idValueList[i].Value << endl;
        }
    }

    void RunALine(string line) {
        CilLineElem lineElem = splitLine(line);

        int sw;
        if (lineElem.Op == "=")
            sw = 0;
        else if (lineElem.Op == "+=")
            sw = 1;
        else if (lineElem.Op == "-=")
            sw = 2;
        else if (lineElem.Op == "*=")
            sw = 3;
		if (lineElem.Op == "/=")
            sw = 4;
		if (lineElem.Op == "^")
			sw = 5;

        switch (sw) {
            case 0:
                if (existId(lineElem.Ln))
                    setValue(lineElem.Ln, stod(lineElem.Rn));
                else
                    addData(lineElem.Ln, stod(lineElem.Rn));
                break;
            case 1:
                setValue(lineElem.Ln, getIdValue(lineElem.Ln) + getIdValue(lineElem.Rn));
                removeData(lineElem.Rn);
                break;
            case 2:
                setValue(lineElem.Ln, getIdValue(lineElem.Ln) - getIdValue(lineElem.Rn));
                removeData(lineElem.Rn);
                break;
            case 3:
                setValue(lineElem.Ln, getIdValue(lineElem.Ln) * getIdValue(lineElem.Rn));
                removeData(lineElem.Rn);
                break;
            case 4:
                setValue(lineElem.Ln, getIdValue(lineElem.Ln) / getIdValue(lineElem.Rn));
                removeData(lineElem.Rn);
                break;
			case 5:
				setValue(lineElem.Ln, pow(getIdValue(lineElem.Ln),getIdValue(lineElem.Rn)));
				removeData(lineElem.Rn);
				break;
        }

    }

    double RunCIL() {
		cout << "\n---------------> Run CIL <--------------"<< endl;
        for (int i = 0; i < cilList.size(); i++) {
            RunALine(cilList[i]);
        }

        return getIdValue("id1");
    }
}; // class CIL





int main() {
    string str;
    str = "1+2+4*10+(9*3+2)-14";

    cout << "Enter Formula: ";
    // cin >> str;
    vector<string> lexList = LexicalParser::generateLexicalList(str);

    CIL cil;
    cil.generateCIL(lexList);

    double result = cil.RunCIL();

    cout << "\nResult: " << result << endl;

    return 0;
}
