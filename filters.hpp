#ifndef FILTERS_H
#define FILTERS_H

#include <queue>
#include <stack>

class TSmooth
{
    private:
        std::queue<double> savedTicks;
        double tickSum;
        unsigned int steps;

    public:
        TSmooth(unsigned int s = 1): tickSum(0), steps(s) {}

        void setSteps(unsigned int s) { steps = s; }

        double tick(double v) {
            unsigned int num = savedTicks.size();
            if (num <= steps) {
                savedTicks.push(v);
                tickSum += v;
            }
            if (num >= steps) {
                tickSum -= savedTicks.front();
                savedTicks.pop();
            }
            return tickSum / ((double) savedTicks.size());
        }
};


class TDelay
{
    private:
        unsigned int delaysteps; // number of ticks;
        std::queue<double> savedTicks;
        double strength;

    public:
        TDelay(unsigned int t, double s = 1):
            delaysteps(t), strength(s) {}

        void setSteps(unsigned int t) { delaysteps = t; }

        void setStrength(double s) { strength = s; }

        double tick(double v) {
            unsigned int num = savedTicks.size();
            if (num < delaysteps) {
                savedTicks.push(v);
                return v;
            }
            else if (num == delaysteps) {
                savedTicks.push(v);
                double signal = strength * savedTicks.front() + v;
                savedTicks.pop();
                return signal;
            }
            else {
                savedTicks.pop();
                return v;
            }
        }
};


class MixReverse
{
    private:
        unsigned int steps;
        std::stack<double> savedSteps;
        bool reverseFlag;

    public:
        MixReverse(unsigned int st = 1):
            steps(st), reverseFlag(false) {}

        void setSteps(unsigned int s) { steps = s; }

        double tick(double v) {
            unsigned int num = savedSteps.size();
            if (reverseFlag && num == 0) {reverseFlag = !reverseFlag; }
            if (!reverseFlag && num >= steps) {reverseFlag = !reverseFlag; }
            if (reverseFlag) {
                v = savedSteps.top();
                savedSteps.pop();
            }
            else {
                savedSteps.push(v);
            }
            return v;
        }
};

#endif
