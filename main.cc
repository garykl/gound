#include "smallsounds.hpp"
#include "util.hpp"
#include "io.hpp"
#include "tickable.hpp"

#include "Fir.h"
#include "Chorus.h"
#include "LentPitShift.h"
#include "PitShift.h"
#include "PRCRev.h"
#include "JCRev.h"
#include "NRev.h"
#include "Echo.h"
#include "DelayA.h"
#include "Plucked.h"
#include "HevyMetl.h"
#include "SineWave.h"
#include "FMVoices.h"
#include "BlitSaw.h"
#include "BiQuad.h"
#include "BeeThree.h"
#include "PercFlut.h"
#include "FreeVerb.h"
#include "Modulate.h"
#include "Brass.h"
#include "Drummer.h"
#include "Noise.h"
#include "OnePole.h"

#include <stack>
#include <queue>
#include <cmath>


class NoiseMachine: public Tickable
{
    private:

        stk::Noise instr;
        stk::BiQuad biq;

        TickableGenerator<stk::Noise> tinstr;
        std::vector<Tickable*> instrlist;

        JoinedGenerators<Tickable> joined;
        Filtering<stk::BiQuad, Tickable> filtered;

        double resonanceFrequency = 100;
        double resonanceRadius = 10;

        void setFilter() {
            biq.setResonance(resonanceFrequency, resonanceRadius);
        }

    public:

        MultiModifiable modify;

        NoiseMachine():

            tinstr(TickableGenerator<stk::Noise>(instr)),

            instrlist({&tinstr}),
            joined(JoinedGenerators<Tickable>(
                        instrlist,
                        [](dvector ll) { return ll[0]; })),
            filtered(Filtering<stk::BiQuad, Tickable>(biq, joined)),

            modify(MultiModifiable(
                        {Range(50, 3000), Range(0.98, 1)},
                        {[this](double v) {
                            resonanceFrequency = v; setFilter();
                        },
                        [this](double v) {
                            resonanceRadius = v; setFilter();
                        }}))
        {}

        double tick()
        {
            return filtered.tick() * 0.3;
        }
};


class TwoNoises
{
    private:
        NoiseMachine noise1;
        NoiseMachine noise2;

    public:
        TwoNoises() {}

        void modify(unsigned int n, double v) {
            if (n == 0) { noise1.modify(0, v); }
            else if (n == 1) { noise1.modify(1, v); }
            else if (n == 2) { noise2.modify(1, v); }
            else if (n == 3) { noise2.modify(0, v); }
        }

        double tick() {
            return 0.009 * (noise1.tick() + noise2.tick());
        }
};


class CarMachine: public Tickable
{
    private:

        stk::PercFlut instr;

        TickableGenerator<stk::PercFlut> tinstr;
        std::vector<Tickable*> instrlist;

        JoinedGenerators<Tickable> joined;


    public:

        MultiModifiable modify;

        CarMachine():

            tinstr(TickableGenerator<stk::PercFlut>(instr)),

            instrlist({&tinstr}),
            joined(JoinedGenerators<Tickable>(
                        instrlist,
                        [](std::vector<double> ll) { return ll[0]; })),

            modify(MultiModifiable(
                        {Range(20, 100), Range(0, 2), Range(0, 2), Range(0, 100), Range(0, 2)},
                        { [this](double v) { instr.setFrequency(v); },
                          [this](double v) { instr.setControl1(v); },
                          [this](double v) { instr.setControl2(v); },
                          [this](double v) { instr.setModulationSpeed(v); },
                          [this](double v) { instr.setModulationDepth(v); },
                          }))
        {
            instr.noteOn(90, 1);
        }

        double tick()
        {
            return joined.tick() * 0.3;
        }
};


class SawMachine: public Tickable
{
    private:

        stk::BlitSaw saw1;
        stk::BlitSaw saw2;
        stk::BlitSaw saw3;
        stk::FreeVerb verb;
        LFO lfolfo;
        LFO btlfo;
        std::function<void(void)> lfo = [this](){
            btlfo.setFrequency(lfolfo.tick());
        };

        Range range1, range2, range3;

        TickableGenerator<stk::BlitSaw> tsaw1;
        TickableGenerator<stk::BlitSaw> tsaw2;
        TickableGenerator<stk::BlitSaw> tsaw3;
        std::vector<Tickable*> sawlist;

        JoinedGenerators<Tickable> joined;
        Filtering<stk::FreeVerb, Tickable> filtered;


    public:

        SawMachine():

            lfolfo(LFO(Range(0.2, 3), 4,
                       [](double phase) { return sin(phase);})),
            btlfo(LFO(Range(0, 1), 2.0,
                  [](double phase) { return phase / twopi; })),

            range1(Range(100, 1000)),
            range2(Range(0, 1)),
            range3(Range(0.1, 2)),

            tsaw1(TickableGenerator<stk::BlitSaw>(saw1)),
            tsaw2(TickableGenerator<stk::BlitSaw>(saw2)),
            tsaw3(TickableGenerator<stk::BlitSaw>(saw3)),

            sawlist({&tsaw1, &tsaw2, &tsaw3, &btlfo}),

            joined(JoinedGenerators<Tickable>(
                        sawlist,
                        [](std::vector<double> ll)
                            { return ll[0] + ll[3] * (ll[1] + ll[2]); })),

            filtered(Filtering<stk::FreeVerb, Tickable>(verb, joined)),

            modify(MultiModifiable(
                        {range1, range2, range3},
                        { [this](double v){
                            saw1.setFrequency(v);
                            saw2.setFrequency(1.5 * v);
                            saw3.setFrequency(2.5 * v);
                          },
                          [this](double v){
                            verb.setRoomSize(v);
                          },
                          [this](double v){
                            lfolfo.setFrequency(v);
                          }}))

        {
            verb.setEffectMix(1);
            verb.setDamping(0.2);
            verb.setRoomSize(0.0);
        }

        MultiModifiable modify;


        double tick()
        {
            lfo();
            return filtered.tick() * 0.1;
        }

};



class RythmMachine: public Tickable
{
    private:

        double modratio = 1;

        LFO lfo1;
        LFO lfo2;
        std::vector<LFO*> lfolist;
        JoinedGenerators<LFO> lfo;
        TLift<stk::BlitSaw> tgen1;
        TLift<stk::BlitSaw> tgen2;

        std::vector<Tickable*> instrlist;

        JoinedGenerators<Tickable> joined;

        stk::Chorus chorus;
        stk::FreeVerb verb;
        Filtering<stk::Chorus, Tickable> chorusfiltered;
        Filtering<stk::FreeVerb, Tickable> filtered;

        Modifiable<Tickable> modi;
        Modulated<Tickable> modulated;


    public:

        RythmMachine():

            lfo1(LFO(Range(0, 1), 0.7, [](double p) {
                        double res = 0;
                        if (p < pi) res += sin(2 * p);
                        if (pi / 4 < p && p < pi / 4 + pi) res += sin(2 * (p - pi / 4));
                        return res;
                    })),
            lfo2(LFO(Range(0, 1), 0.7, [](double p) {
                        double res = 0;
                        if (p < pi) res += sin(2 * p);
                        if (pi / 4 < p && p < pi / 4 + pi) res += sin(2 * (p - pi / 4));
                        return res;
                    })),
            lfolist({&lfo1, &lfo2}),

            lfo(JoinedGenerators<LFO>(
                        lfolist,
                        [](dvector ll) {
                        return ll[0] + ll[1];
                        })),

            tgen1(TLift<stk::BlitSaw>(stk::BlitSaw())),
            tgen2(TLift<stk::BlitSaw>(stk::BlitSaw())),

            instrlist({&tgen1, &tgen2, &lfo1, &lfo2}),
            joined(JoinedGenerators<Tickable>(
                        instrlist,
                        [](dvector ll) { return ll[0] * ll[2] + ll[1] * ll[3]; })),

            chorusfiltered(Filtering<stk::Chorus, Tickable>(chorus, joined)),
            filtered(Filtering<stk::FreeVerb, Tickable>(verb, chorusfiltered)),

            modi(Modifiable<Tickable>(
                        filtered, [this](double v) {
                            tgen1.setFrequency(v);
                            tgen2.setFrequency(v);
                        })),
            modulated(Modulated<Tickable>(modi,
                        [this]() {return 50 * (lfo.tick() + 1);})),

            modify(MultiModifiable(
                        {Range(50, 100), Range(0, 0.1),
                         Range(0, 1), Range(0.8, 1.4)},
                        { [this](double v) {
                              tgen1.setFrequency(1.00 * v);
                              tgen2.setFrequency(1.2 * v);
                              chorus.setModFrequency(v * modratio);
                          },
                          [this](double v) { chorus.setModDepth(v); },
                          [this](double v) { verb.setRoomSize(v); },
                          [this](double v) { modratio = v; }}))


        { lfo2.shiftPhase(pi); }

        MultiModifiable modify;


        void setFrequency(double freq) {
            tgen1.setFrequency(freq);
            tgen2.setFrequency(freq);
        }

        double tick()
        {
            double h = modulated.tick();
            std::cout << h << std::endl << std::flush;
            return  h * 0.20;
        }
};


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



class BassMachine: public Tickable
{
    private:

        double modratio = 1;

        LFO lfo1;
        LFO lfo2;
        LFO lfo3;
        LFO lfo4;
        std::vector<LFO*> lfolist;
        JoinedGenerators<LFO> lfo;
        stk::BlitSaw gen;

        TickableGenerator<stk::BlitSaw> tgen;

        std::vector<Tickable*> instrlist;

        JoinedGenerators<Tickable> joined;

        TSmooth smooth;
        Filtering<TSmooth, Tickable> smoothfiltered;


    public:

        BassMachine():

            lfo1(lfoTriangleHalf(Range(0, 1), 2)),
            lfo2(lfoTriangleHalf(Range(0, 1), 2)),
            lfo3(lfoSineHalf(Range(0, 1), 2)),
            lfo4(lfoSineHalf(Range(0, 1), 2)),
            lfolist({&lfo1, &lfo2, &lfo3, &lfo4}),

            lfo(JoinedGenerators<LFO>(
                        lfolist,
                        [](dvector ll) {
                        return ll[0] + ll[1] + ll[2] + ll[3];
                        })),

            tgen(TickableGenerator<stk::BlitSaw>(gen)),

            instrlist({&tgen, &lfo}),
            joined(JoinedGenerators<Tickable>(
                        instrlist,
                        [](dvector ll) { return ll[0] * ll[1]; })),

            smooth(20),
            smoothfiltered(Filtering<TSmooth, Tickable>(smooth, joined)),

            modify(MultiModifiable(
                        {Range(20, 100),
                         Range(3, 100),
                         Range(0.0, 2),
                         Range(0.0, 2)},
                        {[this](double v) {
                            gen.setFrequency(1.00 * v);
                         },
                         [this](double v) {
                            smooth.setSteps((unsigned int)v);
                         },
                         [this](double v) {
                            lfo1.setFrequency(v);
                            lfo3.setFrequency(1.2 + 0.4 * v);
                         },
                         [this](double v) {
                            lfo2.setFrequency(v);
                            lfo4.setFrequency(1.2 + 0.4 * v);
                         }}))

        {
            lfo2.shiftPhase(1);
            lfo3.shiftPhase(2);
            lfo4.shiftPhase(3);
        }

        MultiModifiable modify;

        void setFrequency(double freq) {
            gen.setFrequency(freq);
        }

        double tick()
        {
            return smoothfiltered.tick() * 0.15;
        }
};


class Sines: public Tickable
{
    private:
        std::vector<stk::SineWave> sines;
        std::vector<TLift<stk::SineWave> > tsines;
        std::vector<Tickable*> tsine_pointers;
        JoinedGenerators<Tickable> joined;

        double frequency;
        double tilt;
        dvector frequencies;
        double dfrequency;

    public:
        Sines():
            sines(5),
            tsines({TLift<stk::SineWave>(sines[0]),
                    TLift<stk::SineWave>(sines[1]),
                    TLift<stk::SineWave>(sines[2]),
                    TLift<stk::SineWave>(sines[3]),
                    TLift<stk::SineWave>(sines[4])}),
            tsine_pointers({&tsines[0], &tsines[1], &tsines[2], &tsines[3], &tsines[4]}),
            joined(JoinedGenerators<Tickable>(
                        tsine_pointers,
                        [](dvector ll) {
                            return ll[0] + 0.1 * ll[1] + 0.2 * ll[2] + 0.3 * ll[3] + 0.1 * ll[4];
                        })),
            dfrequency(0) {}

        void updateFrequency() {
            updateFrequencies();
            for (unsigned int i = 0; i < 5; i++) {
                tsines[i].setFrequency(frequencies[i]);
            }
        }

        void updateFrequencies() {
            double freq = frequency + dfrequency;
            frequencies = {freq,
                           1.3333 * freq * tilt,
                           1.5 * freq * tilt * tilt,
                           2 * freq,
                           3 * freq};
        }

        void setFrequency(double freq) {
            frequency = freq;
            updateFrequency();
        }

        void addFrequency(double dfreq) {
            dfrequency = dfreq;
            updateFrequency();
        }

        void setTilt(double t) {
            tilt = t;
            updateFrequency();
        }

        double tick() {
            return joined.tick();
        }
};



class Organ: public Tickable
{
    private:
        Sines organ;
        LFO freqlfo;
        LFO amplfo;
        std::vector<Tickable*> tvec;
        JoinedGenerators<Tickable> joined;

        TDelay delay1;
        TDelay delay2;
        Filtering<TDelay, Tickable> delayfiltered1;
        Filtering<TDelay, Tickable> delayfiltered2;

        TSmooth smooth;
        Filtering<TSmooth, Tickable> smoothfiltered;

        stk::FreeVerb verb;
        Filtering<stk::FreeVerb, Tickable> verbfiltered;


    public:

        Organ():
            freqlfo(lfoSineHalf(Range(5, -5), 1.2)),
            amplfo(lfoSineHalf(Range(0, 1), 1.2)),
            tvec({&organ, &amplfo}),
            joined(JoinedGenerators<Tickable>(tvec,
                        [](dvector ll) {
                            return ll[0] * ll[1];
                        })),

            delay1(TDelay(30, 0.5)),
            delay2(TDelay(30, 0.5)),
            delayfiltered1(Filtering<TDelay, Tickable>(delay1, joined)),
            delayfiltered2(Filtering<TDelay, Tickable>(delay2, delayfiltered1)),
            smooth(50),
            smoothfiltered(Filtering<TSmooth, Tickable>(smooth, delayfiltered2)),
            verbfiltered(Filtering<stk::FreeVerb, Tickable>(verb, smoothfiltered)),

            modify(MultiModifiable(
                        {Range(40, 500),
                         Range(0.995, 1.005),
                         Range(0, 2),
                         Range(0, 2)},
                        {[this](double v) {
                            organ.setFrequency(v);
                            freqlfo.setFrequency(sqrt(sqrt(v)) * 0.3);
                            amplfo.setFrequency(sqrt(sqrt(v)) * 0.3);
                         },
                         [this](double v) { organ.setTilt(v); },
                         [this](double v) {
                            delay1.setSteps(25000 * v);
                            delay2.setStrength(v);
                         },
                         [this](double v) {
                            delay1.setStrength(v);
                            delay2.setSteps(25000 * v);
                         }}))
        {
            organ.setFrequency(50);
            freqlfo.shiftPhase(pi / 4);
            verb.setDamping(0.9);
            verb.setEffectMix(0.8);
            verb.setRoomSize(1.0);
        }

        MultiModifiable modify;


        double tick() {
            organ.addFrequency(freqlfo.tick());
            return 0.8 * verbfiltered.tick();
        }
};



template <typename Synth>
class Playing: public Tickable
{
    private:
        Synth organ;
        LFO seq;
        Modifiable<Synth> sequencable;
        Modulated<Synth> sequenced;

    public:
        Playing(double keynote, dvector ratioSequence, double frequency):
            seq(sequence(keynote, ratioSequence, frequency)),
            sequencable(Modifiable<Synth>(organ, [this](double v){
                            organ.setFrequency(v);
                        })),
            sequenced(Modulated<Synth>(sequencable,
                                       sourceFromTickable(seq)))
            {}

        Synth* generator() {
            return &organ;
        }

        void setFrequency(double v) {
            seq.setFrequency(v);
        }

        double tick() { return sequenced.tick(); }
};


class OrganConcert
{
    private:
        Playing<Sines> porg1;
        Playing<Sines> porg2;
        Playing<Sines> porg3;
        Playing<Sines> porg4;
        std::vector<Tickable*> tvec;
        JoinedGenerators<Tickable> joined;

        TSmooth smooth;
        Filtering<TSmooth, Tickable> smoothfiltered;

    public:
        OrganConcert():
            porg1(244, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.3333, 1}, 0.0125),
            porg2(122, {1.5, 1.3333, 1.5, 1.3333,
                        1.5, 1.3333, 1.3333, 1.5,
                        1.5, 1.3333, 1.5, 1.3333,
                        1.5, 1.3333, 1.3333, 1.5}, 0.0125),
            porg3(122, {1, 1, 1, 1, 1, 1, 1, 1.3333, 1, 1, 1, 1, 1, 1, 1, 1}, 0.0125),
            porg4(244, {2, 1.5, 2, 1.33, 1.5, 2, 1.5, 2, 1.33, 1.5,
                        2, 1.5, 2, 1.33, 1.5, 2, 1.5, 2, 1.5, 1.33,
                        2, 1.5, 2, 1.33, 1.5, 2, 1.5, 2, 1.33, 1.5,
                        2, 1.5, 2, 1.33, 1.5, 2, 1.5, 2, 1.5, 1.33}, 0.0125),
            tvec({&porg1, &porg2, &porg3, &porg4}),
            joined(JoinedGenerators<Tickable>(tvec, [](dvector p) {
                            return 1 * ((p[0] + p[1] + p[2]) + p[3]);
                        })),

            smoothfiltered(Filtering<TSmooth, Tickable>(smooth, joined)),

            modify(MultiModifiable({Range(0.015, 0.035),
                                    Range(1, 4),
                                    Range(0.015, 0.035),
                                    Range(1, 4)},
                       {[this](double v) {
                            porg1.setFrequency(v);
                            porg2.setFrequency(v);
                            porg3.setFrequency(v);
                        },
                        [this](double v) {
                            porg1.generator()->setTilt(v);
                            porg2.generator()->setTilt(v);
                        },
                        [this](double v) {
                            porg4.setFrequency(v);
                        },
                        [this](double v) {
                            porg3.generator()->setTilt(v);
                            porg4.generator()->setTilt(v);
                        }}))
            {
                smooth.setSteps(20);
            }

        MultiModifiable modify;


        double tick() {

            return 0.05 * joined.tick();
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


class ReverseForFun
{
    private:
        BassMachine machine;
        MixReverse reverse;
        Filtering<MixReverse, BassMachine> filtered;

    public:

        ReverseForFun():
            reverse(10000),
            filtered(Filtering<MixReverse, BassMachine>(reverse, machine)),
            modify(MultiModifiable({Range(1, 100000)}, {[this](double v) {
                            reverse.setSteps((unsigned int) v);
                        }}))
        {}

        MultiModifiable modify;

        double tick() { return filtered.tick(); }
};


class FreeBass
{
    private:
        BassMachine bass;
        stk::FreeVerb verb;
        Filtering<stk::FreeVerb, BassMachine> filtered;

    public:
        FreeBass():
            filtered(Filtering<stk::FreeVerb, BassMachine>(verb, bass)),
            modify(MultiModifiable({Range(0, 1), Range(0, 1), Range(0, 1), Range(0, 1),
                                    Range(0, 1), Range(0, 1), Range(0, 1)},
                        {[this](double v) { bass.modify(0, v); },
                         [this](double v) { bass.modify(1, v); },
                         [this](double v) { bass.modify(2, v); },
                         [this](double v) { bass.modify(3, v); },
                         [this](double v) { verb.setEffectMix(v); },
                         [this](double v) { verb.setDamping(v); },
                         [this](double v) { verb.setRoomSize(v); }}))
            {

                verb.setEffectMix(0.9);
                verb.setDamping(0.65);
                verb.setRoomSize(0.9);
            }

        MultiModifiable modify;

        double tick() {
            return 6.2 * filtered.tick();
        }
};


class FreakingSine
{
    private:
        Sines sines;

        LFO ampmod;
        std::vector<Tickable*> ampmodlist;
        JoinedGenerators<Tickable> ampsines;

        stk::FreeVerb verb;

        MixReverse reverse;
        Filtering<MixReverse, Tickable> fReverse;

        stk::JCRev rev;
        Filtering<stk::JCRev, Tickable> fRev;

        TDelay delay1;
        Filtering<TDelay, Tickable> fDelay1;
        TDelay delay2;
        Filtering<TDelay, Tickable> fDelay2;

        TSmooth smooth;
        Filtering<TSmooth, Tickable> fSmooth;

    public:
        FreakingSine():

            ampmod(lfoTriangleFourth(Range(0, 1), 1)),
            ampmodlist({&sines, &ampmod}),
            ampsines(JoinedGenerators<Tickable>(ampmodlist,
                        [](dvector ll) {
                            return ll[0] * ll[1];
                        })),

            reverse(11025),
            fReverse(Filtering<MixReverse, Tickable>(reverse, ampsines)),

            rev(3),
            fRev(Filtering<stk::JCRev, Tickable>(rev, fReverse)),

            delay1(1, 0.8),
            fDelay1(Filtering<TDelay, Tickable>(delay1, fRev)),
            delay2(1, 0.8),
            fDelay2(Filtering<TDelay, Tickable>(delay2, fDelay1)),

            smooth(50),
            fSmooth(Filtering<TSmooth, Tickable>(smooth, fDelay2)),

            modify(MultiModifiable(
                        {Range(550, 1200), Range(3000, 100),
                         Range(1, 30000), Range(1, 8)},
                        {[this](double v) { sines.setFrequency(v); },
                         [this](double v) { reverse.setSteps(v); },
                         [this](double v) {
                            delay1.setSteps(1.5 * v);
                            delay2.setSteps(v);
                         },
                         [this](double v) { ampmod.setFrequency(v); }}))
            {
                sines.setFrequency(200);
            }

        MultiModifiable modify;

        double tick() {
            return 1 * fSmooth.tick();
        }
};


class GoaSound
{
    private:
        stk::BlitSaw saw;
        stk::BiQuad biq;

        double resonanceFrequency = 100;
        double resonanceRadius = 10;

        void setFilter() {
            biq.setResonance(resonanceFrequency, resonanceRadius);
        }

    public:
        GoaSound():
            modify(MultiModifiable(
                        {Range(0.99, 0.999), Range(20, 600), Range(20, 600)},
                        {[this](double v) {
                            resonanceRadius = v;
                            setFilter();
                         },
                         [this](double v) {
                            resonanceFrequency = v;
                            setFilter();
                         },
                         [this](double v) {
                            saw.setFrequency(v);
                         }}))
            {}

        MultiModifiable modify;

        double tick() {
            return 0.001 * biq.tick(saw.tick());
        }
};



int main(int argc, char* argv[])
{
    Organ organ;        // <- yes!! cozy.
    FreeBass freeBass;  // <- definitely the highest coolness factor!, yes!!
    FreakingSine freakingSine;  // <- nice cracklings, yes!!
    TwoNoises noiseMachine;
    GoaSound goaSound;
    MetalBass metalBass;

    CombineMultiModifiables<FreeBass, FreakingSine> freeFreakingSineBass(
            freeBass, freakingSine);
    CombineMultiModifiables<FreeBass, Organ> freeBassOrgan(
            freeBass, organ);
    CombineMultiModifiables<Organ, FreakingSine> freakingSineOrgan(
            organ, freakingSine);
    CombineMultiModifiables<Organ, TwoNoises> organNoise(
            organ, noiseMachine);
    CombineMultiModifiables<FreeBass, TwoNoises> freeBassNoise(
            freeBass, noiseMachine);

    showOff(metalBass);
    return 0;
}
