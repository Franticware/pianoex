#include "gener.h"

#include <cassert>
#include <string>

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    for (KeySelection keySel = KeySelection::Begin; keySel != KeySelection::End; ++keySel)
    {
        for (int keys = 0; keys != 2; ++keys)
        {
            std::string fnameExt;
            switch (keySel)
            {
                case KeySelection::White:
                    break;
                case KeySelection::Black:
                    fnameExt += "_black";
                    break;
                case KeySelection::Chromatic:
                    fnameExt += "_chromatic";
                    break;
                default:
                    assert(false && "unexpected enum value");
                    break;
            }
            if (keys)
            {
                fnameExt += "_keys";
            }
            fnameExt += ".ly";
            generate1(("pianoex1" + fnameExt).c_str(), keySel, keys);
            generate2(("pianoex2" + fnameExt).c_str(), keySel, keys);
            generate3(("pianoex3" + fnameExt).c_str(), keySel, keys);
            generate4(("pianoex4" + fnameExt).c_str(), keySel, keys);
            generate5(("pianoex5" + fnameExt).c_str(), keySel, keys);
            generate6(("pianoex6" + fnameExt).c_str(), keySel, keys);
        }
    }
    return 0;
}
