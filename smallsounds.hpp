#ifndef SMALLSOUNDS_H
#define SMALLSOUNDS_H

#include "util.hpp"
#include "tickable.hpp"

LFO lfoSineFull(Range range, double frequency) {
    return LFO(
            range, frequency,
            [](double p) {
                return sin(p);
            });
}

LFO lfoSineHalf(Range range, double frequency) {
    return LFO(
            range, frequency,
            [](double p) {
                if (p > pi) return 0.0;
                return sin(2 * p);
            });
}

LFO lfoSineFourth(Range range, double frequency) {
    return LFO(
            range, frequency,
            [](double p) {
                if (p > 3 * pi / 2) return 0.0;
                return sin(4 * p);
            });
}

LFO lfoTriangleFull(Range range, double frequency) {
    return LFO( range, frequency, [](double p) { return p / twopi; });
}


LFO lfoTriangleHalf(Range range, double frequency) {
    return LFO( range, frequency, [](double p) {
                if (p < pi) { return p / pi; }
                return 0.0;
            });
}

LFO lfoTriangleFourth(Range range, double frequency) {
    return LFO( range, frequency, [](double p) {
                if (p < pi / 2) { return 2 * p / pi; }
                return 0.0;
            });
}

LFO sequence(double keynote, dvector tones, double speed) {
    return LFO(Range(keynote, 2 * keynote), speed, [tones](double p) {
            unsigned int n = tones.size();
            double period = twopi / ((double) n);
            for (unsigned int i = 0; i < n; i++) {
                if (i * period <= p && p < (i + 1) * period) {
                    return tones[i] - 1;
                }
            }
            return 0.0;
        });
}


source<double> sourceFromTickable(Tickable& tickable) {
    return [&]() { return tickable.tick(); };
}


template <typename ModTick1, typename ModTick2>
class CombineMultiModifiables: public Tickable
{
    private:
        ModTick1 mod1;
        ModTick2 mod2;

    public:
        CombineMultiModifiables(ModTick1& m1, ModTick2& m2):
            mod1(m1), mod2(m2) {}

        void modify(unsigned int n , double v) {
            mod1.modify(n, v);
            mod2.modify(n, v);
        }

        double tick() {
            return mod1.tick() + mod2.tick();
        }
};



#endif
