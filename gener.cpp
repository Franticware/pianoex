#include "gener.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

struct Scale
{
    std::string key;
    std::vector<std::string> tones;
};

struct ScalePair
{
    Scale major;
    Scale minor;
};

std::vector<ScalePair> getScales(const char* lowerOctave, const char* upperOctave, KeySelection keySel)
{
    const char* scaleBase = "c*d*ef*g*a*b";
    const char* majorFlat = "010101001010";
    const char* minorFlat = "101101010010";
    const int majorInd[] = {0, 2, 4, 5, 7, 9, 11, 12};
    const int minorInd[] = {0, 2, 3, 5, 7, 8, 10, 12};
    std::vector<ScalePair> ret;
    for (size_t i = 0; i != strlen(scaleBase); ++i)
    {
        bool isBlack = scaleBase[i] == '*';
        bool includeKey = false;
        if (isBlack && keySel == KeySelection::Black)
        {
            includeKey = true;
        }
        if (!isBlack && keySel == KeySelection::White)
        {
            includeKey = true;
        }
        if (keySel == KeySelection::Chromatic)
        {
            includeKey = true;
        }
        if (includeKey)
        {
            ScalePair pair;
            for (int m = 0; m != 2; ++m)
            {
                const int* toneInd = m == 0 ? majorInd : minorInd;
                const char* flat = m == 0 ? majorFlat : minorFlat;
                Scale scale;
                for (int j = 0; j != 8; ++j)
                {
                    int toneIndex = toneInd[j] + i;
                    int toneIndexWrap = toneIndex % 12;
                    std::string toneName = scaleBase[toneIndexWrap] == '*' ? ((flat[i] == '1') ? scaleBase[toneIndexWrap + 1] + std::string("f")
                                                                                               : scaleBase[toneIndexWrap - 1] + std::string("s"))
                                                                           : scaleBase[toneIndexWrap] + std::string("");
                    if (j == 0)
                    {
                        scale.key = toneName + (m == 0 ? " \\major" : " \\minor");
                    }
                    toneName += (toneIndex == toneIndexWrap) ? lowerOctave : upperOctave;
                    scale.tones.push_back(toneName);
                }
                if (m == 0)
                {
                    pair.major = scale;
                }
                else
                {
                    pair.minor = scale;
                }
            }
            ret.push_back(pair);
        }
    }
    return ret;
}

static const char* lyHeader = "\\version \"2.24.2\"\n"
                              "\\include \"english.ly\"\n"
                              "\\paper {\n"
                              "  oddFooterMarkup = \\markup {\n"
                              "    \\fill-line {\n"
                              "      \"https://github.com/Franticware/pianoex\"\n"
                              "    }\n"
                              "  }\n"
                              "}\n"
                              "\\header {\n"
                              "  title = \"Klavírní cvičení\"\n"
                              "  tagline = ##f\n"
                              "}\n"
                              "\\score {\n"
                              "  \\layout { }\n"
                              "  \\midi {\n"
                              "    \\tempo 4 = 120\n"
                              "  }\n"
                              "  \\new PianoStaff <<\n"
                              "    \\new Staff {\n";

void execLilypond(const char* fileName)
{
    system(("lilypond -dmidi-extension=mid " + std::string(fileName)).c_str());
}

void writeScale1(FILE* fout, const ScalePair& pair, bool keys, bool slurs)
{
    for (int j = 0; j != 2; ++j)
    {
        const Scale& scale = j == 0 ? pair.major : pair.minor;
        if (keys)
        {
            fprintf(fout, "\\key %s\n", scale.key.c_str());
        }
        for (size_t i = 0; i != 4; ++i)
        {
            fprintf(fout, "%s4 ", scale.tones[i].c_str());
            if (slurs && i == 0 && j == 0)
            {
                fprintf(fout, "( ");
            }
        }
        fprintf(fout, "\n");
        for (size_t i = 0; i != 4; ++i)
        {
            fprintf(fout, "%s4 ", scale.tones[4 - i].c_str());
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "%s1\n", pair.minor.tones[0].c_str());
    if (slurs)
    {
        fprintf(fout, " ) ");
    }
}

void generate1(const char* fileName, KeySelection keySel, bool keys)
{
    FILE* fout = fopen(fileName, "w");
    fprintf(fout, "%s", lyHeader);
    fprintf(fout, "\\numericTimeSignature\n\\time 4/4\n");
    const bool slurs = false;
    for (size_t j = 0; j != 2; ++j)
    {
        auto scales = j == 0 ? getScales("''", "'''", keySel) : getScales("", "'", keySel);
        for (size_t i = 0; i != scales.size(); ++i)
        {
            writeScale1(fout, scales[i], keys, slurs);
            fprintf(fout, "\\bar \"|%c\"\n\\break\n", i + 1 == scales.size() ? '.' : '|');
        }
        if (j == 0)
        {
            fprintf(fout, "%s", "    }\n\\new Staff { \\clef bass\n");
        }
        else
        {
            fprintf(fout, "    }\n  >>\n}\n");
        }
    }
    fclose(fout);
    execLilypond(fileName);
}

void writeScale2(FILE* fout, const ScalePair& pair, bool keys, bool slurs)
{
    for (int j = 0; j != 2; ++j)
    {
        const Scale& scale = j == 0 ? pair.major : pair.minor;
        if (keys)
        {
            fprintf(fout, "\\key %s\n", scale.key.c_str());
        }
        fprintf(fout, "%s8 ", scale.tones[0].c_str());
        if (slurs)
        {
            fprintf(fout, "( ");
        }
        fprintf(fout, "%s8 ", scale.tones[1].c_str());
        fprintf(fout, "%s4 ", scale.tones[2].c_str());
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[1].c_str());
        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s4 ", scale.tones[3].c_str());
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s8 ", scale.tones[3].c_str());
        fprintf(fout, "%s4 ", scale.tones[4].c_str());
        if (slurs)
        {
            fprintf(fout, " ) ");
        }
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[4].c_str());
        if (slurs)
        {
            fprintf(fout, "( ");
        }
        fprintf(fout, "%s8 ", scale.tones[3].c_str());
        fprintf(fout, "%s4 ", scale.tones[2].c_str());
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[3].c_str());
        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s4 ", scale.tones[1].c_str());
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s8 ", scale.tones[1].c_str());
        fprintf(fout, "%s4 ", scale.tones[0].c_str());
        if (slurs)
        {
            fprintf(fout, " ) ");
        }
        fprintf(fout, "\n\\noBreak\n");
    }
}

void generate2(const char* fileName, KeySelection keySel, bool keys)
{
    FILE* fout = fopen(fileName, "w");
    fprintf(fout, "%s", lyHeader);
    fprintf(fout, "\\numericTimeSignature\n\\time 2/4\n");
    const bool slurs = true;
    for (size_t j = 0; j != 2; ++j)
    {
        auto scales = j == 0 ? getScales("''", "'''", keySel) : getScales("", "'", keySel);
        for (size_t i = 0; i != scales.size(); ++i)
        {
            writeScale2(fout, scales[i], keys, slurs);
            fprintf(fout, "\\bar \"|%c\"\n\\break\n", i + 1 == scales.size() ? '.' : '|');
        }
        if (j == 0)
        {
            fprintf(fout, "%s", "    }\n\\new Staff { \\clef bass\n");
        }
        else
        {
            fprintf(fout, "    }\n  >>\n}\n");
        }
    }
    fclose(fout);
    execLilypond(fileName);
}

void writeScale3(FILE* fout, const ScalePair& pair, bool keys)
{
    for (int j = 0; j != 2; ++j)
    {
        const Scale& scale = j == 0 ? pair.major : pair.minor;
        if (keys)
        {
            fprintf(fout, "\\key %s\n", scale.key.c_str());
        }
        fprintf(fout, "<%s %s>8 ", scale.tones[0].c_str(), scale.tones[2].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[1].c_str(), scale.tones[3].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[2].c_str(), scale.tones[4].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[2].c_str(), scale.tones[4].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[1].c_str(), scale.tones[3].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[0].c_str(), scale.tones[2].c_str());
    }
}

void generate3(const char* fileName, KeySelection keySel, bool keys)
{
    FILE* fout = fopen(fileName, "w");
    fprintf(fout, "%s", lyHeader);
    fprintf(fout, "\\numericTimeSignature\n\\time 2/4\n");
    for (size_t j = 0; j != 2; ++j)
    {
        auto scales = j == 0 ? getScales("''", "'''", keySel) : getScales("", "'", keySel);
        for (size_t i = 0; i != scales.size(); ++i)
        {
            writeScale3(fout, scales[i], keys);
            fprintf(fout, "\\bar \"|%c\"\n\\break\n", i + 1 == scales.size() ? '.' : '|');
        }
        if (j == 0)
        {
            fprintf(fout, "%s", "    }\n\\new Staff { \\clef bass\n");
        }
        else
        {
            fprintf(fout, "    }\n  >>\n}\n");
        }
    }
    fclose(fout);
    execLilypond(fileName);
}

void writeScale4(FILE* fout, const ScalePair& pair, bool keys)
{
    for (int j = 0; j != 2; ++j)
    {
        const Scale& scale = j == 0 ? pair.major : pair.minor;
        if (keys)
        {
            fprintf(fout, "\\key %s\n", scale.key.c_str());
        }

        fprintf(fout, "%s8 ", scale.tones[0].c_str());
        fprintf(fout, "%s8 ", scale.tones[2].c_str());

        fprintf(fout, "%s8 ", scale.tones[1].c_str());
        fprintf(fout, "%s8 ", scale.tones[3].c_str());

        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s8 ( ", scale.tones[4].c_str());

        fprintf(fout, "%s4 ) ", scale.tones[4].c_str());

        fprintf(fout, "%s8 ", scale.tones[4].c_str());
        fprintf(fout, "%s8 ", scale.tones[2].c_str());

        fprintf(fout, "%s8 ", scale.tones[3].c_str());
        fprintf(fout, "%s8 ", scale.tones[1].c_str());

        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s8 ( ", scale.tones[0].c_str());

        fprintf(fout, "%s4 ) ", scale.tones[0].c_str());
    }
}

void generate4(const char* fileName, KeySelection keySel, bool keys)
{
    FILE* fout = fopen(fileName, "w");
    fprintf(fout, "%s", lyHeader);
    fprintf(fout, "\\numericTimeSignature\n\\time 4/4\n");
    for (size_t j = 0; j != 2; ++j)
    {
        auto scales = j == 0 ? getScales("''", "'''", keySel) : getScales("", "'", keySel);
        for (size_t i = 0; i != scales.size(); ++i)
        {
            writeScale4(fout, scales[i], keys);
            fprintf(fout, "\\bar \"|%c\"\n\\break\n", i + 1 == scales.size() ? '.' : '|');
        }
        if (j == 0)
        {
            fprintf(fout, "%s", "    }\n\\new Staff { \\clef bass\n");
        }
        else
        {
            fprintf(fout, "    }\n  >>\n}\n");
        }
    }
    fclose(fout);
    execLilypond(fileName);
}

void writeScale5(FILE* fout, const ScalePair& pair, bool keys)
{
    for (int j = 0; j != 2; ++j)
    {
        const Scale& scale = j == 0 ? pair.major : pair.minor;
        if (keys)
        {
            fprintf(fout, "\\key %s\n", scale.key.c_str());
        }

        fprintf(fout, "%s8~ ", scale.tones[0].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[0].c_str(), scale.tones[2].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[0].c_str(), scale.tones[2].c_str());

        fprintf(fout, "%s8~ ", scale.tones[1].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[1].c_str(), scale.tones[3].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[1].c_str(), scale.tones[3].c_str());

        fprintf(fout, "%s8~ ", scale.tones[2].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[2].c_str(), scale.tones[4].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[2].c_str(), scale.tones[4].c_str());

        fprintf(fout, "%s8~ ", scale.tones[4].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[4].c_str(), scale.tones[2].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[4].c_str(), scale.tones[2].c_str());

        fprintf(fout, "%s8~ ", scale.tones[3].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[3].c_str(), scale.tones[1].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[3].c_str(), scale.tones[1].c_str());

        fprintf(fout, "%s8~ ", scale.tones[2].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[2].c_str(), scale.tones[0].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[2].c_str(), scale.tones[0].c_str());
    }
}

void generate5(const char* fileName, KeySelection keySel, bool keys)
{
    FILE* fout = fopen(fileName, "w");
    fprintf(fout, "%s", lyHeader);
    fprintf(fout, "\\numericTimeSignature\n\\time 6/4\n");
    for (size_t j = 0; j != 2; ++j)
    {
        auto scales = j == 0 ? getScales("''", "'''", keySel) : getScales("", "'", keySel);
        for (size_t i = 0; i != scales.size(); ++i)
        {
            writeScale5(fout, scales[i], keys);
            fprintf(fout, "\\bar \"|%c\"\n\\break\n", i + 1 == scales.size() ? '.' : '|');
        }
        if (j == 0)
        {
            fprintf(fout, "%s", "    }\n\\new Staff { \\clef bass\n");
        }
        else
        {
            fprintf(fout, "    }\n  >>\n}\n");
        }
    }
    fclose(fout);
    execLilypond(fileName);
}


void writeScale6(FILE* fout, const ScalePair& pair, bool keys)
{
    for (int j = 0; j != 2; ++j)
    {
        const Scale& scale = j == 0 ? pair.major : pair.minor;

        // ex. 1
        if (keys)
        {
            fprintf(fout, "\\key %s\n", scale.key.c_str());
        }
        fprintf(fout, "\\numericTimeSignature\n\\time 4/4\n");
        bool slurs = true;
        for (size_t i = 0; i != 4; ++i)
        {
            fprintf(fout, "%s4 ", scale.tones[i].c_str());
            if (slurs && i == 0)
            {
                fprintf(fout, "( ");
            }
        }
        fprintf(fout, "\n");
        for (size_t i = 0; i != 4; ++i)
        {
            fprintf(fout, "%s4 ", scale.tones[4 - i].c_str());
        }
        fprintf(fout, "\n");
        fprintf(fout, " %s1 ) \n", scale.tones[0].c_str());

        // ex. 2
        fprintf(fout, "%s8 ", scale.tones[0].c_str());
        if (slurs)
        {
            fprintf(fout, "( ");
        }
        fprintf(fout, "%s8 ", scale.tones[1].c_str());
        fprintf(fout, "%s4 ", scale.tones[2].c_str());
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[1].c_str());
        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s4 ", scale.tones[3].c_str());
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s8 ", scale.tones[3].c_str());
        fprintf(fout, "%s4 ", scale.tones[4].c_str());
        if (slurs)
        {
            fprintf(fout, " ) ");
        }
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[4].c_str());
        if (slurs)
        {
            fprintf(fout, "( ");
        }
        fprintf(fout, "%s8 ", scale.tones[3].c_str());
        fprintf(fout, "%s4 ", scale.tones[2].c_str());
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[3].c_str());
        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s4 ", scale.tones[1].c_str());
        fprintf(fout, "\n\\noBreak\n");
        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s8 ", scale.tones[1].c_str());
        fprintf(fout, "%s4 ", scale.tones[0].c_str());
        if (slurs)
        {
            fprintf(fout, " ) ");
        }
        // fprintf(fout, "\n\\noBreak\n");

        fprintf(fout, "\n\\break\n");

        // ex. 4
        fprintf(fout, "%s8 ", scale.tones[0].c_str());
        fprintf(fout, "%s8 ", scale.tones[2].c_str());

        fprintf(fout, "%s8 ", scale.tones[1].c_str());
        fprintf(fout, "%s8 ", scale.tones[3].c_str());

        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s8 ( ", scale.tones[4].c_str());

        fprintf(fout, "%s4 ) ", scale.tones[4].c_str());

        fprintf(fout, "\n\\noBreak\n");

        fprintf(fout, "%s8 ", scale.tones[4].c_str());
        fprintf(fout, "%s8 ", scale.tones[2].c_str());

        fprintf(fout, "%s8 ", scale.tones[3].c_str());
        fprintf(fout, "%s8 ", scale.tones[1].c_str());

        fprintf(fout, "%s8 ", scale.tones[2].c_str());
        fprintf(fout, "%s8 ( ", scale.tones[0].c_str());

        fprintf(fout, "%s4 ) ", scale.tones[0].c_str());

        fprintf(fout, "\n\\noBreak\n");

        // ex. 5
        fprintf(fout, "\\numericTimeSignature\n\\time 6/4\n");

        fprintf(fout, "%s8~ ", scale.tones[0].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[0].c_str(), scale.tones[2].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[0].c_str(), scale.tones[2].c_str());

        fprintf(fout, "%s8~ ", scale.tones[1].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[1].c_str(), scale.tones[3].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[1].c_str(), scale.tones[3].c_str());

        fprintf(fout, "%s8~ ", scale.tones[2].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[2].c_str(), scale.tones[4].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[2].c_str(), scale.tones[4].c_str());

        fprintf(fout, "\n\\noBreak\n");

        fprintf(fout, "%s8~ ", scale.tones[4].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[4].c_str(), scale.tones[2].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[4].c_str(), scale.tones[2].c_str());

        fprintf(fout, "%s8~ ", scale.tones[3].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[3].c_str(), scale.tones[1].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[3].c_str(), scale.tones[1].c_str());

        fprintf(fout, "%s8~ ", scale.tones[2].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[2].c_str(), scale.tones[0].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[2].c_str(), scale.tones[0].c_str());

        fprintf(fout, "\n\\noBreak\n");

        // ex. 3
        fprintf(fout, "\\numericTimeSignature\n\\time 4/4\n");

        fprintf(fout, "<%s %s>8 ", scale.tones[0].c_str(), scale.tones[2].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[1].c_str(), scale.tones[3].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[2].c_str(), scale.tones[4].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[2].c_str(), scale.tones[4].c_str());
        fprintf(fout, "<%s %s>8 ", scale.tones[1].c_str(), scale.tones[3].c_str());
        fprintf(fout, "<%s %s>4 ", scale.tones[0].c_str(), scale.tones[2].c_str());

        fprintf(fout, "\n\\break\n");
    }
}

void generate6(const char* fileName, KeySelection keySel, bool keys)
{
    FILE* fout = fopen(fileName, "w");
    fprintf(fout, "%s", lyHeader);
    for (size_t j = 0; j != 2; ++j)
    {
        auto scales = j == 0 ? getScales("''", "'''", keySel) : getScales("", "'", keySel);
        for (size_t i = 0; i != scales.size(); ++i)
        {
            if (keySel == KeySelection::Chromatic)
            {
                if (i % 4 == 0 && j == 0)
                {
                    fprintf(fout, "\n\\pageBreak\n");
                }
            }
            writeScale6(fout, scales[i], keys);
            fprintf(fout, "\\bar \"|%c\"\n\\break\n", i + 1 == scales.size() ? '.' : '|');
        }
        if (j == 0)
        {
            fprintf(fout, "%s", "    }\n\\new Staff { \\clef bass\n");
        }
        else
        {
            fprintf(fout, "    }\n  >>\n}\n");
        }
    }
    fclose(fout);
    execLilypond(fileName);
}
