#include "tickable.hpp"
#include "filters.hpp"
#include "io.hpp"

#include "BlitSaw.h"


class MetalBass: public Tickable
{
    private:

        double amp = 0;

        stk::BlitSaw gen;
        TickableGenerator<stk::BlitSaw> tgen;

        TSmooth smooth;
        Filtering<TSmooth, Tickable> smoothfiltered;

        TDelay delay;
        Filtering<TDelay, Tickable> delayfiltered;


    public:

        MetalBass():

            tgen(TickableGenerator<stk::BlitSaw>(gen)),

            smooth(20),
            smoothfiltered(Filtering<TSmooth, Tickable>(smooth, tgen)),

            delay(20),
            delayfiltered(Filtering<TDelay, Tickable>(delay, smoothfiltered)),

            modify(MultiModifiable(
                        {Range(0, 1),
                         Range(3, 100),
                         Range(20, 500),
                         Range(10, 100000),
                         Range(0, 1)},
                        {[this](double v) {
                            amp = v;
                         },
                         [this](double v) {
                            smooth.setSteps((unsigned int) v);
                         },
                         [this](double v) {
                            gen.setFrequency(v);
                         },
                         [this](double v) {
                            delay.setSteps(v);
                         },
                         [this](double v) {
                            delay.setStrength(v);
                         }}))
        {}

        MultiModifiable modify;

        double tick()
        {
            return amp * delayfiltered.tick() * 0.15;
        }
};


int main()
{
    MetalBass metalBass;
    showOff(metalBass);

    return 0;
}
