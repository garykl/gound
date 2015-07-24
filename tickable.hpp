#ifndef TICKABLE_H
#define TICKABLE_H
#include "util.hpp"

#include <functional>
#include <vector>
#include <algorithm>



class Tickable
{
    public:
        virtual double tick() = 0;
};


template <typename Generator>
class TickableGenerator: public Tickable
{
    private:
        Generator& generator;

    public:
        TickableGenerator(Generator& gen): generator(gen) {}

        double tick()
        {
            return generator.tick();
        }
};


template <typename FilterType, typename SignalType>
class Filtering: public Tickable
{
    private:
        FilterType& filter;
        SignalType& signal;

    public:
        Filtering(FilterType& f, SignalType& s): filter(f), signal(s) {}

        double tick() { return filter.tick(signal.tick()); }
};


template <typename Synth>
class JoinedGenerators: public Tickable
{
    private:
        const std::vector<Synth*>& synths;
        MultiOp<double> join;

    public:
        JoinedGenerators(const std::vector<Synth*>& s, MultiOp<double> j):
            synths(s), join(j) {}

        double tick()
        {
            std::vector<double> ticks(synths.size());
            std::transform(synths.begin(), synths.end(), ticks.begin(),
                    [](Synth* s){ return s->tick(); });
            return join(ticks);
        }
};


class LFO: public Tickable
{
    private:
        double phase;
        Range range;
        double frequency;
        UnaryOp<double> envelope;
        double dphi;
        double sampleRate;

        void setPhaseShift() { dphi = twopi / sampleRate * frequency; }

    public:
        LFO(Range rng, double freq=2,
            UnaryOp<double> e=[](double p){ return 0;}, double sr=44100):
            phase(0), range(rng), frequency(freq),
            envelope(e), sampleRate(sr)
        {
            setPhaseShift();
        }


        void setFrequency(double freq)
        {
            frequency = freq;
            setPhaseShift();
        }

        void shiftPhase(double p)
        {
            phase += p;
        }

        double tick()
        {
            phase += dphi;
            if (phase > twopi) { phase -= twopi; }
            return range.stretchNormal(envelope(phase));
        }
};


template <typename Synth>
class TLift: public Tickable
{
    private:
        Synth synth;

    public:
        TLift(Synth s): synth(s) {}

        void setFrequency(double v) { synth.setFrequency(v); }

        double tick() { return synth.tick(); }
};



template <typename Synth>
class Modifiable: public Tickable
{
    private:
        Synth& synth;

    public:
        sink<double> modify;

        Modifiable(Synth& s, sink<double> f): synth(s), modify(f) {}

        double tick() {
            return synth.tick();
        }


};

template <typename ModSynth> // has methods tick and modify
class Modulated: public Tickable
{
    private:
        Modifiable<ModSynth>& synth;
        source<double> f;

    public:
        Modulated(Modifiable<ModSynth>& s, source<double> l):
            synth(s), f(l) {}

        double tick() {
            synth.modify(f());
            return synth.tick();
        }
};





#endif
