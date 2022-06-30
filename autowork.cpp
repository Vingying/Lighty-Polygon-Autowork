#include "sys/stat.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
// #include <filesystem>
#include <experimental/filesystem>
#ifdef __linux__
#include "sys/io.h"
#elif _WIN64 or _WIN32
#include "io.h"
#include "windows.h"
#endif
using namespace std;

const int MAX_FILE_SIZE = 1024 * 1024; /*数据最大的大小，单位字节*/
const int LEAST_DATA = 10;             /*gen-chart 生成的数据数，少于该数目会error，多于则提取前LEAST_DATA组数据*/

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

char s[1000];

bool FindFile(string filename, string errinfo = "")
{
    int isNotFound;
    cerr << "Finding " << filename << endl;
#ifdef __linux__
    sprintf(s, "find %s", filename.c_str());
    isNotFound = system(s);
    cerr << s << endl;
#elif _WIN64 or _WIN32
    _finddata_t file;
    long k = _findfirst(filename.c_str(), &file);
    isNotFound = (k == -1 ? 1 : 0);
#endif
    cerr << isNotFound << endl;
    if (isNotFound)
    {
        cout << RED << errinfo << RESET << endl;
    }
    return isNotFound == 0;
}

void validation()
{
    cout << "------------------------------------" << endl
         << RESET;
    cout << "|        Start Validation...       |" << endl
         << RESET;
    cout << "------------------------------------" << endl
         << RESET;

    if (!FindFile("val.cpp", "NOT FOUND Validator"))
        exit(0);
    int compile_err = system("g++ val.cpp -o val");
    if (compile_err != 0)
    {
        cout << RED << "Something wrong with val.cpp" << RESET << endl;
        exit(0);
    }
    int err = 0;
    for (int i = 1; i <= LEAST_DATA; ++i)
    {
#ifdef __linux__
        sprintf(s, "./val < %d.in", i);
#elif _WIN64 or _WIN32
        sprintf(s, "val < %d.in", i);
#endif
        cerr << s << endl;
        err = max(err, system(s));
    }
    if (err != 0)
    {
        cout << RED << "NOT PASS Validation" << endl
             << RESET;
        exit(0);
    }

    int type = 0, k = FindFile("std2.cpp", "NOT FOUND std2.cpp");
    if (k == 0)
        k = FindFile("std2.py", "NOT FOUND std2.py"), type = 1;
    if (k == 0)
        exit(0);

    if (type == 0)
    {
        compile_err = system("g++ std2.cpp -o std2 -O2 -lm");
        if (compile_err != 0)
        {
            cout << RED << "Something wrong with compiling std2.cpp" << RESET << endl;
            exit(0);
        }
    }
    for (int i = 1; i <= LEAST_DATA; ++i)
    {
        if (type == 0)
        {
#ifdef __linux__
            sprintf(s, "./std2 < %d.in > std2out", i);
#elif _WIN64 or _WIN32
            sprintf(s, "std2 < %d.in > std2out", i);
#endif
        }
        else
        {
#ifdef __linux__
            sprintf(s, "python3 std2.py < %d.in > std2out", i);
#elif _WIN64 or _WIN32
            sprintf(s, "python std2.py < %d.in > std2out", i);
#endif
        }
        cerr << s << endl;
        int err = system(s);
        if (err != 0)
        {
            cout << RED << "Something wrong with running std2" << RESET << endl;
            exit(0);
        }
#ifdef __linux__
        sprintf(s, "cmp %d.out std2out", i);
#elif _WIN64 or _WIN32
        sprintf(s, "fc %d.out std2out", i);
#endif
        cerr << s << endl;
        int cmperr = system(s);
        if (cmperr != 0)
        {
            sprintf(s, "std output is not consistent with std2 (test %d)", i);
            cout << RED << s << RESET << endl;
            exit(0);
        }
    }
#ifdef __linux__
    system("rm std2out");
#elif _WIN32 or _WIN64
    system("del std2out");
#endif

    cout << "------------------------------------" << endl;
    cout << "|" << GREEN << "        Validation Over.          " << RESET << "|" << endl;
    cout << "------------------------------------" << endl;
}

void generation()
{
    cout << "------------------------------------" << endl;
    cout << "|        Start Generation...       |" << endl;
    cout << "------------------------------------" << endl;
    int err = FindFile("gen-chart", "NOT FOUND gen-chart");
    if (err == 0)
        exit(0);
    int errgen = FindFile("gen.cpp", "NOT FOUND gen.cpp");
    if (errgen == 0)
        exit(0);
    int compile_err = system("g++ gen.cpp -o gen");
    if (compile_err != 0)
    {
        cout << RED << "Something wrong with compiling gen.cpp" << RESET << endl;
        exit(0);
    }
    fstream gen_chart;
    gen_chart.open("gen-chart");
    string gen_op;
    int cnt = 0;
    while (getline(gen_chart, gen_op))
    {
#ifdef __linux__
        if (gen_op[0] != '.')
            gen_op = "./" + gen_op;
#elif _WIN64 or _WIN32
        if (gen_op[0] == '.' && gen_op[1] == '/')
            gen_op = gen_op.substr(2);
#endif
        auto genop = gen_op.c_str();
        cerr << genop << endl;
        int err = system(genop);
        if (err != 0)
        {
            cout << RED << "ERROR gen-chart" << endl
                 << RESET;
            exit(0);
        }
        cnt++;
        if (cnt == LEAST_DATA)
            break;
    }
    gen_chart.close();
    if (cnt != LEAST_DATA)
    {
        cout << RED << "gen-chart NOT ENOUGH DATAS" << endl
             << RESET;
        exit(0);
    }
    int k = FindFile("std.cpp"), type = 0;
    if (k == 0)
        k = FindFile("std.py", "NOT FOUND std"), type = 1;
    if (k == 0)
        exit(0);
    if (type == 0)
    {
        int compile_err = system("g++ std.cpp -o std -O2 -lm");
        if (compile_err != 0)
        {
            cout << RED << "Something wrong with compiling std" << RESET << endl;
            exit(0);
        }
    }

    /*检验数据是否超过最大限制大小*/
    for (int i = 1; i <= LEAST_DATA; ++i)
    {
        sprintf(s, "%d.in", i);
        string filepath = s;
        int size = std::experimental::filesystem::file_size(filepath);
        if (size >= MAX_FILE_SIZE)
        {
            sprintf(s, "OVERSIZE %d.in.", i);
            cout << RED << s << RESET << endl;
            exit(0);
        }
        if (type == 0)
        {
#ifdef __linux__
            sprintf(s, "./std < %d.in > %d.out", i, i);
#elif _WIN64 or _WIN32
            sprintf(s, "std < %d.in > %d.out", i, i);
#endif
        }
        else
        {
#ifdef __linux__
            sprintf(s, "python3 std.py < %d.in > %d.out", i, i);
#elif _WIN64 or _WIN32
            sprintf(s, "python std.py < %d.in > %d.out", i, i);
#endif
        }
        cerr << s << endl;
        int err = system(s);
        if (err != 0)
        {
            cout << RED << "Something wrong with running std" << RESET << endl;
            exit(0);
        }
    }
    for (int i = 1; i <= LEAST_DATA; ++i)
    {
        sprintf(s, "%d.out", i);
        string filepath = s;
        int size = std::experimental::filesystem::file_size(filepath);
        if (size >= MAX_FILE_SIZE)
        {
            sprintf(s, "OVERSIZE %d.out.", i);
            cout << RED << s << RESET << endl;
            exit(0);
        }
        system(s);
    }

    system("mkdir 输入");
    system("mkdir 输出");
    for (int i = 1; i <= LEAST_DATA; ++i)
    {
#ifdef __linux__
        sprintf(s, "cp %d.in ./输入/", i);
#elif _WIN64 or _WIN32
        sprintf(s, "copy %d.in \"输入/\"", i);
#endif
        cerr << s << endl;
        system(s);
#ifdef __linux__
        sprintf(s, "cp %d.out ./输出/", i);
#elif _WIN64 or _WIN32
        sprintf(s, "copy %d.out \"输出/\"", i);
#endif
        cerr << s << endl;
        system(s);
    }

    cout << "------------------------------------" << endl;
    cout << "|" << GREEN << "        Generation Over.          " << RESET << "|" << endl;
    cout << "------------------------------------" << endl;
}

void movefiles()
{
    cout << "------------------------------------" << endl;
    cout << "|        Copying Files...          |" << endl;
    cout << "------------------------------------" << endl;
#ifdef __linux__
    sprintf(s, "cp ../../数据分布.* ./");
#elif _WIN64 or _WIN32
    sprintf(s, "copy \"../../数据分布.*\" \"./\"");
#endif
    cerr << s << endl;
    system(s);

    int k = FindFile("题面.docx", "NOT FOUND 题面.docx");
    if (k == 0)
        exit(0);
    k = FindFile("题解.txt", "NOT FOUND 题解.txt");
    if (k == 0)
        exit(0);
    k = FindFile("数据分布.xlsx", "NOT FOUND 数据分布.xlsx");
    if (k == 0)
        exit(0);

    system("mkdir 程序");

    k = FindFile("std.*", "NOT FOUND std.*");
    if (k == 0)
        exit(0);
#ifdef __linux__
    sprintf(s, "cp std.* ./程序/");
#elif _WIN64 or _WIN32
    sprintf(s, "copy std.* \"./程序/\"");
#endif
    cerr << s << endl;
    system(s);

    k = FindFile("std2.*", "NOT FOUND std2.*");
    if (k == 0)
        exit(0);
#ifdef __linux__
    sprintf(s, "cp std2.* ./程序/");
#elif _WIN64 or _WIN32
    sprintf(s, "copy std2.* \"./程序/\"");
#endif
    cerr << s << endl;
    system(s);

#ifdef __linux__
    sprintf(s, "cp gen.* ./程序/");
#elif _WIN64 or _WIN32
    sprintf(s, "copy gen.* \"./程序/\"");
#endif
    cerr << s << endl;
    system(s);

#ifdef __linux__
    sprintf(s, "cp gen-chart ./程序/");
#elif _WIN64 or _WIN32
    sprintf(s, "copy gen-chart \"./程序/\"");
#endif
    cerr << s << endl;
    system(s);

#ifdef __linux__
    sprintf(s, "cp val.* ./程序/");
#elif _WIN64 or _WIN32
    sprintf(s, "copy val.* \"./程序/\"");
#endif
    cerr << s << endl;
    system(s);

    cout << YELLOW << "是否删除该目录下的数据？(y/n[default])" << endl
         << RESET;
    char op = getchar();
    if (op == 'y' || op == 'Y')
    {
        for (int i = 1; i <= LEAST_DATA; ++i)
        {
#ifdef __linux__
            sprintf(s, "rm %d.in", i);
#elif _WIN64 or _WIN32
            sprintf(s, "del %d.in", i);
#endif
            cerr << s << endl;
            system(s);
#ifdef __linux__
            sprintf(s, "rm %d.out", i);
#elif _WIN64 or _WIN32
            sprintf(s, "del %d.out", i);
#endif
            cerr << s << endl;
            system(s);
        }
        cout << "Deleted." << endl;
    }
    else
    {
        cout << "Abort." << endl;
    }

    cout << "------------------------------------" << endl;
    cout << "|" << GREEN << "        Copying Over.             " << RESET << "|" << endl;
    cout << "------------------------------------" << endl;
}

int main()
{
    generation();
    validation();
    movefiles();
    return 0;
}