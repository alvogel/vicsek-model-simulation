#ifndef RESULT_H_INCLUDED
#define RESULT_H_INCLUDED

#include <list>
#include <fstream>

#include "ResultEntry.h"

class Result
{

private:

    std::list<ResultEntry> data;

public:

    ~Result();

    Result();

    Result(std::string file_name);

    void addEntry(unsigned int n_i, unsigned int n_p, float anv, float v, float eta, float radius);

    bool isStable();

    ResultEntry getAvgLastN(int last_n);

    void saveToFile();

    void clear();

};

#endif // RESULT_H_INCLUDED
