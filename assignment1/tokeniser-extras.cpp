// Author : Akide Liu
// Date : 30/8/21
// Student ID : A1743748
// Description :
#include "iobuffer.h"
#include "tokeniser-extras.h"
#include <algorithm>
#include <sstream>
#include <string>

// to shorten the code
using namespace std;
using namespace CS_IO_Buffers;

namespace Assignment_Tokeniser
{
    // check if a char matches another char or is a member of a character group
    // eg char_isa('3',cg_digit) returns true
    bool char_isa(int ch, int cg)
    {
        // single character groups use their character as their value
        // do not add them to the switch statement
        if (ch == cg)
            return true;

        switch (cg)
        {
        case cg_wspace:

            // characters that start rule wspace
            switch (ch)
            {
            case '\t':
            case '\n':
            case '\r':
            case ' ':
                return true;
            default:
                return false;
            }
            // complete a switch statement for each of the following
        case cg_identifier:
        // characters that start rule identifier
            switch (ch)
            {
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '$':
                return true;
            default:
                return false;
            }
        case cg_letter:
            // characters that start rule letter
            switch (ch)
            {
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
            case '-':
            case '$':
            case '.':
                return true;
            default:
                return false;
            }
        case cg_number: // characters that start rule number
        case cg_integer:

            // characters that start rule integer
            switch (ch)
            {
            case '0' ... '9':
                return true;
            default:
                return false;
            }
        case cg_scientific:

            // characters that start rule scientific
            switch (ch)
            {
            case '0' ... '9':
            // case '.':
                // case '+':
                // case '-':
                return true;
            default:
                return false;
            }
        case cg_digit:

            // characters that start rule digit
            switch (ch)
            {
            case '0' ... '9':
                return true;
            default:
                return false;
            }
        case cg_digit19:

            // characters that start rule digit19
            switch (ch)
            {
            case '1' ... '9':
                return true;
            default:
                return false;
            }
        case cg_exponent:

            // characters that start rule exponent
            switch (ch)
            {
            case 'e':
            case 'E':
                return true;
            default:
                return false;
            }
        case cg_eee:

            // characters that start rule eee
            switch (ch)
            {
            case 'e':
            case 'E':
                return true;
            default:
                return false;
            }
        case cg_sign:

            // characters that start rule eee
            switch (ch)
            {
            case '+':
            case '-':
                return true;
            default:
                return false;
            }
        case cg_instring:

            // characters that start rule instring
            switch (ch)
            {
            case ' ':
            case '!':
            case 35 ... 126:
            case 0x0C00 ... 0x0C7F:
                return true;
            default:
                return false;
            }
        case cg_telegu:

            // characters that start rule telegu
            switch (ch)
            {
            case 0x0C00 ... 0x0C7F:
                return true;
            default:
                return false;
            }
        case cg_symbol:

            // characters that start rule symbol
            switch (ch)
            {
            case '@':
            case '~':
            case '=':
            case '-':
            case '+':
            case '/':
            case '*':
            case '<':
            case '>':
            case '}':
            case '{':
            case '(':
            case ')':
            case '[':
            case ']':
            case '.':
                return true;
            default:
                return false;
            }
        case cg_comment_char:
            // characters that start rule comment_char
            switch (ch)
            {
            case 0x0530 ... 0x058F:
            case 0xFB13 ... 0xFB17:
            case '\t':
            case '\r':
            case 32 ... 126:
                return true;
            default:
                return false;
            }
        case cg_armenian:
            // characters that start rule armenian
            switch (ch)
            {
            case 0x0530 ... 0x058F:
            case 0xFB13 ... 0xFB17:
                return true;
            default:
                return false;
            }
        default:
            return false;
        }
    }

    // work out the kind of a parsed token, this may call keyword_or_identifier()
    // the spelling is a valid token or "" if at end of input
    TokenKind classify_spelling(string spelling)
    {
        if (spelling == "")
            return tk_eoi;

        switch (spelling[0])
        {
        case ' ':

            return tk_space;

        case '\n':

            return tk_newline;

        case '\t':

            return tk_tab;

        case '\r':

            return tk_carriage_return;

        case '@':

            if (spelling.length() == 1)
                return tk_at;

        case '~':

            if (spelling.length() == 1)
                return tk_not;

            if (spelling[1] == '=')
                return tk_ne;

        case '=':

            if (spelling.length() == 1)
                return tk_assign;

            if (spelling[1] == '=')
                return tk_eq;

        case '-':

            if (spelling.length() == 1)
                return tk_sub;

        case '+':

            if (spelling.length() == 1)
                return tk_add;

        case '/':

            if (spelling.length() == 1)
                return tk_div;

            if (spelling.find("**") != string::npos)
                return tk_div;

            if (spelling[1] == '/' && spelling[spelling.length() - 1] == '\n')
                return tk_eol_comment;

        case '*':

            if (spelling.length() >= 1)
                return tk_mult;

        case '<':

            if (spelling.length() == 2)
                return tk_la_shift;

            if (spelling.length() == 3)
                return tk_ll_shift;

        case '>':

            if (spelling.length() == 2)
                return tk_ra_shift;

            if (spelling.length() == 3)
                return tk_rl_shift;

        case '}':

            if (spelling.length() == 1)
                return tk_rcb;

        case '{':

            if (spelling.length() == 1)
                return tk_lcb;

        case '(':
            if (spelling.length() == 1)
                return tk_lrb;

        case ')':
            if (spelling.length() == 1)
                return tk_rrb;

        case '[':

            if (spelling.length() == 1)
                return tk_lsb;

        case ']':

            if (spelling.length() == 1)
                return tk_rsb;

        case '#':
            if (spelling[spelling.length() - 1] == '\n')
            return tk_hash_comment;

        case '"':

            if (spelling[spelling.length() - 1] == '"')
                return tk_string;

        case '.':

            if (spelling.length() == 1)
                return tk_dot;

        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '$':

            return keyword_or_identifier(spelling);

        case '0' ... '9':

            if (spelling.find('.') != string::npos)
                return tk_scientific;

            return tk_integer;

        default:
            return tk_oops;
        }
    }

    // format based depends on the computed shift count and input
    string tk_scientific_basic_controller(string spelling, int base)
    {
        if ((spelling.find("+") != string::npos || spelling.find("-") != string::npos) && spelling.length() == 1)
        {
            spelling = "";
        }

        if (spelling == "")
            return (base >= 0) ? "+" + std::to_string(base) : std::to_string(base);

        const int result = stoi(spelling) + base;

        return (result >= 0) ? "+" + std::to_string(result) : std::to_string(result);
    }
    // check the left is greater than 0
    pair<string, int> tk_scientific_left_shift(string input)
    {
        int count = input.find(".") - 1;
        string temp;

        temp.push_back(input[0]);
        temp.push_back('.');

        for (size_t i = 1; i < input.size(); i++)
        {
            if (input[i] != '.')
                temp.push_back(input[i]);
        }

        return make_pair(temp, count);
    }

    // check the left is less than 0
    pair<string, int> tk_scientific_right_shift(string input)
    {
        int count = 0;
        for (int i = input.length() - 1; i >= 0; i--)
        {
            if (input[i] == '0' || input[i] == '.')
                break;

            count++;
        }

        count = input.length() - (count + 1);
        count = 0 - count;
        string temp;

        // such as 0.0003
        if (count == -1)
        {
            temp = input[input.length() - 1];

            return make_pair(temp, count);
        }

        // such as 0.00364
        bool checker = false;
        for (size_t i = 1; i < input.size(); i++)
        {
            if (input[i] != '.' && input[i] != '0')
                checker = true;

            if (checker)
                temp.push_back(input[i]);
        }

        temp.insert(1, ".");
        return make_pair(temp, count);
    }

    // the checker
    int positive_negative_zero_checker(string left)
    {
        if (stod(left) == 0)
            return 0;
        else if (left[0] == '0')
            return -1;
        else
            return 1;
    }

    //  erase zeros and ".e" wrong format
    string erase_dot_zeros_before_e(string left)
    {
        if (left[left.length() - 1] == '.')
            return left.substr(0, left.length() - 1);

        int cut_count = 0;

        for (int i = left.length() - 1; i >= 0; i--)
        {
            if (left[i] == '0')
                ++cut_count;
            else
                break;
        }

        return left.substr(0, left.length() - cut_count);
        ;
    }

    // work out the correct spelling to use in the Token object being created by new_token()
    // the spelling is a valid token and kind is its kind
    string correct_spelling(TokenKind kind, string spelling)
    {
        if (spelling == "")
            return "";

        if (kind == tk_string)
        {
            return spelling.substr(1, spelling.length() - 2);
        }

        //label (LOOP)，return LOOP
        if (spelling[0] == '(' && spelling[spelling.length() - 1] == ')')
        {
            return spelling.substr(1, spelling.length() - 2);
        }
        if (kind == tk_hash_comment)
        {
            string remove_n = spelling.substr(0, spelling.find('\n'));

            const int index = remove_n.find(' ');

            return remove_n.substr(index, remove_n.length() - index);
        }
        if (kind == tk_eol_comment)
        {
            string remove_n = spelling.substr(0, spelling.find('\n'));

            const int index = remove_n.find(' ');

            return remove_n.substr(index, remove_n.length() - index);
        }

        //fully rewrite the logic for scientific handler
        // 1. check is there 'e' or 'E' exist
        // 2. if exist, split the spelling to left and right (left is digits before E, right is power after E)
        // 3. if not exist split the spelling to left and right (left is spelling, right is empty)
        // --- the checker is implemented by positive_negative_zero_checker()
        // 4. check the left is equal to 0, return 0
        // 5. check the left is greater than 0, call tk_scientific_left_shift()
        // 5. check the left is less than 0, call tk_scientific_right_shift()
        // 6. use erase_dot_zeros_before_e() to erase zeros and ".e" wrong format
        // 7. user tk_scientific_basic_controller() to format based depends on the computed shift count and input
        // 8. concatenation
        // return erase_dot_zeros_before_e(resultLeft.first) + "e" +
        // tk_scientific_basic_controller(right,resultLeft.second);
        if (kind == tk_scientific)
        {
            // left -> the digits before .
            // right -> the digits after . (may include e )
            string left, right;
            pair<string, int> resultLeft;
            int find_e = spelling.find('e');

            if (find_e == string::npos)
                find_e = spelling.find('E');

            // e found
            if (find_e != string::npos)
            {
                left = spelling.substr(0, find_e);
                right = spelling.substr(find_e + 1, spelling.length() - find_e - 1);
            }
            else
            {
                left = spelling;
                right = "";
            }

            // compute left part
            if (positive_negative_zero_checker(left) == 0)
                return "0";

            else if (positive_negative_zero_checker(left) == -1)
            {
                resultLeft = tk_scientific_right_shift(left);
            }
            else if (positive_negative_zero_checker(left) == 1)
            {
                resultLeft = tk_scientific_left_shift(left);
            }

            // result format
            return erase_dot_zeros_before_e(resultLeft.first) + "e" + tk_scientific_basic_controller(right, resultLeft.second);
        }
        return spelling;
    }

}
