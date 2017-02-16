//
// Created by Arnold Müller on 10.02.17.
//

#ifndef CLIONTEST_ENDIANESS_H
#define CLIONTEST_ENDIANESS_H


class Endianness {
public:
    Endianness() = delete;
    ~Endianness() = delete;
    static bool IsLittleEndian();
    static bool IsBigEndian() { return !IsLittleEndian(); }
private:
    //const static int i;
};


#endif //CLIONTEST_ENDIANESS_H
