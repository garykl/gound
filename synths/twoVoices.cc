#include "tickable.hpp"
#include "filters.hpp"
#include "io.hpp"

#include "BlitSaw.h"


class TwoVoices: public Tickable
{
    private:

        stk::BlitSaw gen_1;
        stk::BlitSaw gen_2;
        TickableGenerator<stk::BlitSaw> tgen_1;
        TickableGenerator<stk::BlitSaw> tgen_2;

        TSmooth smooth_1;
        Filtering<TSmooth, Tickable> smoothfiltered_1;
        TSmooth smooth_2;
        Filtering<TSmooth, Tickable> smoothfiltered_2;



    public:

        TwoVoices():

            tgen_1(TickableGenerator<stk::BlitSaw>(gen_1)),
            tgen_2(TickableGenerator<stk::BlitSaw>(gen_2)),

            smooth_1(20),
            smoothfiltered_1(Filtering<TSmooth, Tickable>(smooth_1, tgen_1)),
            smooth_2(20),
            smoothfiltered_2(Filtering<TSmooth, Tickable>(smooth_2, tgen_2)),



            modify(MultiModifiable(

                        {Range(20, 500),
                         Range(20, 500),
                         Range(3, 100)},

                        {[this](double v) {
                            gen_1.setFrequency(v);
                          },
                            [this](double v) {
                              gen_2.setFrequency(v);
                            },
                              [this](double v) {
                                smooth_1.setSteps((unsigned int) v);
                                smooth_2.setSteps((unsigned int) v);
                              }}))
  {}

        MultiModifiable modify;

        double tick()
        {
          return smoothfiltered_1.tick() + smoothfiltered_2.tick();
        }
};


int main()
{
    TwoVoices twoVoices;
    showOff(twoVoices);

    return 0;
}
