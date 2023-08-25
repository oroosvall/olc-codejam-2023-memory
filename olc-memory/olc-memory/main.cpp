#include "olcPixelGameEngine.h"

class Example : public olc::PixelGameEngine
{
public:
    Example()
    {
        // Name your application
        sAppName = "Memory";
    }

public:
    bool OnUserCreate() override
    {
        // Called once at the start, so create things here
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // Called once per frame, draws random coloured pixels
        return true;
    }
};

int main()
{
    Example demo;
    if (demo.Construct(512, 512, 2, 2))
        demo.Start();
    return 0;
}