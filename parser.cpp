#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class rule
{
  public:
    rule(int _id, char nT, string exp)
    {
        id = _id;
        nonTerminal = nT;
        expression = exp;
    }

    int id;
    char nonTerminal;
    string expression;
};

class cell
{
  public:
    vector<rule> rules;
    string text; //dummy string for testing.
};

vector<rule> rules;
void cyk(string);
int getRuleByTerminal(char);
int concatAndSearch(char, char);
void printCell(cell **, int, int);

bool debug = false;

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        string a(argv[1]);
        if (a.compare("-g") == 0)
        {
            debug = true;
        }
    }

    ifstream input("grammar.txt");
    string output, terminals, nonterminals;

    std::getline(input, terminals);
    std::getline(input, nonterminals);

    while (std::getline(input, output))
    {
        stringstream ss(output);

        int id;
        char nonTerminal;
        string exp, temp;
        ss >> id;
        ss >> nonTerminal;
        ss >> temp;
        ss >> exp;

        rule r(id, nonTerminal, exp);

        rules.push_back(r);
    }

    input.close();

    ifstream words("words.txt");

    while (std::getline(words, output))
    {
        cyk(output);
    }

    words.close();
    //cyk("aca");

    return 0;
}

void cyk(string input)
{
    if (debug)
        cout << input << endl;
    int len = input.size();
    cell **table = new cell *[len];

    for (int i = 0; i < len; i++)
        table[i] = new cell[len];

    for (int i = len - 1; i >= 0; i--)
    {
        for (int j = i; j >= 0; j--)
        {
            table[i][j].text = "*";
        }
    }

    for (int i = 0; i < len; i++)
    {
        cell *row = table[i];

        char in = input[i];
        int ruleIndex = getRuleByTerminal(in);
        if (ruleIndex != -1)
        {
            row[i].rules.push_back(rules[ruleIndex]);
        }
        else
            cerr << "Invalid input." << endl;
    }

    int n = len;

    for (int size = 1; size < n; size++)
    {
        for (int i = 0; i < n - size; i++)
        {
            int j = i + size;
            cell *current = &table[i][j];

            if (debug)
                cout << "For i : " << i << " j : " << j << endl;

            for (int k = 0; k < size; k++)
            {
                if (debug)
                {
                    cout << "(" << i << "," << i + k << ") ";
                    cout << "(" << i + k + 1 << "," << j << ") ";
                }

                cell c1 = table[i][i + k];
                cell c2 = table[i + k + 1][j];
                int c1size = c1.rules.size();
                int c2size = c2.rules.size();
                for (int x = 0; x < c1size; x++)
                {
                    char nonTerminal1 = c1.rules[x].nonTerminal;
                    for (int y = 0; y < c2size; y++)
                    {
                        char nonTerminal2 = c2.rules[y].nonTerminal;
                        int rulesIndex = concatAndSearch(nonTerminal1, nonTerminal2);
                        if (rulesIndex != -1)
                        {
                            current->rules.push_back(rules[rulesIndex]);
                        }
                    }
                }
            }

            if (debug)
                cout << endl;
        }
    }

    cell fnl = table[0][n - 1];
    int fsize = fnl.rules.size();
    bool isFound = false;
    if (fsize > 0)
    {
        for (int i = 0; i < fsize; i++)
        {
            if (fnl.rules[i].nonTerminal == rules[0].nonTerminal)
            {
                isFound = true;
            }
        }
    }

    ofstream results("results.txt", fstream::app);
    if (isFound)
        results << "YES" << endl;
    else
        results << "NO" << endl;
    results.close();

    for (int i = 0; i < len; i++)
        delete[] table[i];

    delete[] table;

    /*
    if (debug)
    {
        for (int i = 0; i < len; i++)
        {
            for (int j = 0; j < len; j++)
            {
                cout << table[i][j].rules.size() << " ";
            }

            cout << endl;
        }
    }
*/
}

//printcell for debug
void printCell(cell **table, int i, int j)
{
    cout << "(" << i << "," << j << ") " << table[i][j].rules.size() << endl;
    int size = table[i][j].rules.size();
    for (int k = 0; k < size; k++)
    {
        rule r = table[i][j].rules[k];
        cout << r.nonTerminal << " " << r.expression << endl;
    }
}

//Concatenate two char and compare it with all rules
//Return -1 = fail, otherwise a rule index.
int concatAndSearch(char ch1, char ch2)
{
    char *concated = new char[3];
    concated[0] = ch1;
    concated[1] = ch2;
    concated[2] = '\0';

    int size = rules.size();

    for (int i = 0; i < size; i++)
    {
        if (rules[i].expression.compare(concated) == 0)
        {
            return i;
        }
    }

    return -1;
}

//get a rule by a terminal
int getRuleByTerminal(char terminal)
{
    int ruleCount = rules.size();
    for (int i = 0; i < ruleCount; i++)
    {
        string exp = rules[i].expression;
        if (exp.size() == 1 && exp[0] == terminal)
            return i;
    }

    return -1;
}