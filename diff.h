#ifndef EMATTSAN_DIFF_H
#define EMATTSAN_DIFF_H

#include <string>
#include <vector>
#include <algorithm>
#include <exception>

namespace emattsan
{

class Diff
{
public:
    class Exception : public std::exception
    {
    public:
        explicit Exception(const char* what) : std::exception(), what_(what) {}
        ~Exception() throw() {}
        const char* what() const throw() { return what_.c_str(); }

    private:
        std::string what_;
    };

    enum EditType
    {
        DELETE,
        COMMON,
        ADD
    };

    typedef std::vector<EditType> EditSequence;

    struct TreeNode
    {
        TreeNode(EditType editType, int prev) : editType(editType), prev(prev) {}

        EditType editType;
        int      prev;
    };

    static const int NO_LINK = -1;

    struct VItem
    {

        VItem() : y(0), tail(NO_LINK) {}
        VItem(int y, int tail) : y(y), tail(tail) {}

        int y;
        int tail;
    };

    Diff() : tree_(), tail_(NO_LINK)
    {
    }

    struct Comparer
    {
        virtual ~Comparer() {}
        virtual bool isEqual(int x, int y) const = 0;
    };

    int ondImpl(int sizeA, int sizeB, const Comparer& comparer)
    {
        const int& offset = sizeA;

        VItem* v = new VItem[sizeA + sizeB + 1];

        tree_.clear();
        tail_ = NO_LINK;

        for(int d = 0; d <= sizeA + sizeB; ++d)
        {
            for(int k = -d; k <= d; k += 2)
            {
                if((k < -sizeA) || (sizeB < k))
                {
                    continue;
                }

                VItem* v_k   = v + k + offset;
                VItem* v_kp1 = v_k + 1;
                VItem* v_km1 = v_k - 1;

                if(d != 0)
                {
                    if(((k == -d) || (k == -sizeA)) || (((k != d) && (k != sizeB)) && ((v_km1->y + 1) < v_kp1->y)))
                    {
                        v_k->y    = v_kp1->y;
                        v_k->tail = tree_.size();
                        tree_.push_back(TreeNode(DELETE, v_kp1->tail));
                    }
                    else
                    {
                        v_k->y    = v_km1->y + 1;
                        v_k->tail = tree_.size();
                        tree_.push_back(TreeNode(ADD, v_km1->tail));
                    }
                }

                while(((v_k->y - k) < sizeA) && (v_k->y < sizeB) && comparer.isEqual(v_k->y - k, v_k->y))
                {
                    TreeNode node(COMMON, v_k->tail);
                    v_k->tail = tree_.size();
                    tree_.push_back(node);
                    ++v_k->y;
                }

                if(((v_k->y - k) >= sizeA) && (v_k->y >= sizeB))
                {
                    tail_ = v_k->tail;
                    delete[] v;
                    return d;
                }
            }
        }
        delete[] v;
        throw Exception("not found");
    }

    template<typename sequence_t>
    int ond(const sequence_t& sequenceA, const sequence_t& sequenceB)
    {
        struct ComparerImpl : public Comparer
        {
            sequence_t sequenceA;
            sequence_t sequenceB;

            ComparerImpl(const sequence_t& sequenceA, const sequence_t& sequenceB) : sequenceA(sequenceA), sequenceB(sequenceB) {}

            bool isEqual(int x, int y) const
            {
                return sequenceA[x] == sequenceB[y];
            }
        };

        return ondImpl(sequenceA.size(), sequenceB.size(), ComparerImpl(sequenceA, sequenceB));
    }

    void getSES(EditSequence& ses) const
    {
        ses.clear();
        for(int i = tail_; i != NO_LINK; i = tree_[i].prev)
        {
            ses.push_back(tree_[i].editType);
        }
        std::reverse(ses.begin(), ses.end());
    }

private:
    std::vector<TreeNode> tree_;
    int                   tail_;
};

} // namespace emattsan

#endif//EMATTSAN_DIFF_H
