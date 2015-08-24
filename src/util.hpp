#ifndef UTIL_H
#define UTIL_H
#include <functional>
#include <vector>

template <typename T> using UnaryOp = std::function<T(T)>;
template <typename T> using MultiOp = std::function<T(std::vector<T>)>;
template <typename T> using source = std::function<T(void)>;
template <typename T> using sink = std::function<void(T)>;
template <typename T> using BinaryOp = std::function<T(T, T)>;
using ArbitraryOp = std::function<double(double...)>;
using dvector = std::vector<double>;


double pi = 3.141592653589793;
double twopi = 2 * pi;


class Range
{
    private:
        double min;
        double max;

    public:
        Range(double in, double ax): min(in), max(ax) {}

        double normalize (double value)
        {
            if (value < min && min < max) return min;
            else if (value > max && min < max) return max;
            else if (value > min && min > max) return min;
            else if (value < max && min > max) return max;
            return (value - min) / (max - min);
        }

        double stretchNormal (double value)
        {
            if (value < 0) value = 0;
            else if (value > 1) value = 1;
            return value * (max - min) + min;
        }
};



class MultiModifiable
{
    private:
        std::vector<Range> ranges;
        std::vector<sink<double> > modifyRaw;

    public:

        MultiModifiable(std::vector<Range> rs, std::vector<sink<double> > m):
            ranges(rs), modifyRaw(m) {}

        void operator()(unsigned int n, double value)
        {
            if (ranges.size() <= n) return;
            if (modifyRaw.size() <= n) return;

            modifyRaw[n](ranges[n].stretchNormal(value));
        }
};


#endif
