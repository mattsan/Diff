#include "diff.h"

#include <iostream>
#include <fstream>

using namespace emattsan;

std::ostream& operator << (std::ostream& out, Diff::EditType editType)
{
    switch(editType)
    {
    case Diff::DELETE: out << '-'; break;
    case Diff::COMMON: out << ' '; break;
    case Diff::ADD:    out << '+'; break;
    default:           out << '?'; break;
    }
    return out;
}

void load(const std::string& filename, std::vector<std::string>& strings)
{
    strings.clear();

    std::ifstream src(filename.c_str());
    std::string   s;
    while(std::getline(src, s).good())
    {
        strings.push_back(s);
    }
}

template<typename sequence_t>
void showDiff(const Diff::EditSequence& ses, const sequence_t& sequenceA, const sequence_t& sequenceB)
{
    typename sequence_t::const_iterator a = sequenceA.begin();
    typename sequence_t::const_iterator b = sequenceB.begin();
    for(std::vector<Diff::EditType>::const_iterator i = ses.begin(); i != ses.end(); ++i)
    {
        std::cout << *i << " ";
        switch(*i)
        {
        case Diff::DELETE:
            std::cout << *a;
            ++a;
            break;

        case Diff::COMMON:
            std::cout << *a;
            ++a;
            ++b;
            break;

        case Diff::ADD:
            std::cout << *b;
            ++b;
            break;

        default:
            break;
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cout << argv[0] << " <file_a> <file_b>\n";
        return 0;
    }

    try
    {
        Diff               diff;
        Diff::EditSequence ses;

        std::string filenameA(argv[1]);
        std::string filenameB(argv[2]);

        std::cout << "FILE NAME: " << diff.ond(filenameA, filenameB) << "\n";
        diff.getSES(ses);
        showDiff(ses, filenameA, filenameB);

        std::cout << "\n";

        std::vector<std::string> stringsA;
        std::vector<std::string> stringsB;

        load(filenameA, stringsA);
        load(filenameB, stringsB);

        std::cout << "CONTENTS: " << diff.ond(stringsA, stringsB) << "\n";
        diff.getSES(ses);
        showDiff(ses, stringsA, stringsB);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
