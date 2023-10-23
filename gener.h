#ifndef GENER_H
#define GENER_H

#include <type_traits>

enum class KeySelection
{
    Begin,
    Black = Begin,
    White,
    Chromatic,
    End
};

inline KeySelection& operator++(KeySelection& e)
{
    e = KeySelection(static_cast<std::underlying_type<KeySelection>::type>(e) + 1);
    return e;
}

void generate1(const char* fileName, KeySelection keySel, bool keys);
void generate2(const char* fileName, KeySelection keySel, bool keys);
void generate3(const char* fileName, KeySelection keySel, bool keys);
void generate4(const char* fileName, KeySelection keySel, bool keys);
void generate5(const char* fileName, KeySelection keySel, bool keys);
void generate6(const char* fileName, KeySelection keySel, bool keys);

#endif // GENER_H
