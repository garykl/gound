#ifndef IO_H

#include <unistd.h>
#include <thread>
#include <limits>

#include "RtWvOut.h"


class Channel
{

    private:
        stk::RtWvOut channel;
        //unsigned int steps = 100000;
        //unsigned int counter = 0;

    public:
        Channel(): channel(stk::RtWvOut(2, stk::Stk::sampleRate(),
                           0, stk::RT_BUFFER_SIZE, 12)) { }

        template <typename Synth>
        void tick(Synth &osci) {
            //if (counter < steps) {
            //    osci.tick();
            //    counter++;
            //    return;
            //}
            try {
                double h = osci.tick();
                //std::cout << h << std::endl << std::flush;
                channel.tick(h);
            }
            catch (stk::StkError&) {
                std::cerr << "either channel or osci does not tick" << std::endl;
                std::exit(1);
            }
        }

};


template <typename Synthi>
void inputLoop (Synthi& machine)
{
    std::string str;
    int counter;
    std::string hs;
    try {
        while (std::getline(std::cin, str)) {

            std::istringstream sstr(str);
            counter = 0;
            while (sstr >> hs) {
                machine.modify(counter, atof(hs.c_str()));
                counter++;
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << "error!" << std::flush << std::endl;
    }
}


template <typename Synth>
void soundLoop (Synth& machine)
{
    auto channel = Channel();

    while (true)
    {
        channel.tick(machine);
    }
}


template <typename Synth>
void showOff(Synth& synth)
{
    stk::Stk::setSampleRate(44100.0);
    stk::Stk::showWarnings(true);
    unsigned int forever = std::numeric_limits<unsigned int>::max();

    std::thread soundLoopThread(soundLoop<Synth>, std::ref(synth));
    std::thread inputLoopThread (inputLoop<Synth>, std::ref(synth));
    sleep(forever);
}

#endif
#define IO_H
