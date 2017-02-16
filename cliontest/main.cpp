#include <iostream>
#include <sstream>
#include "Endianness.h"
#include "Range2D.h"
#include "Mandelbrot.h"
#include <wx/wx.h>
#include "CLionTestApp.h"

using std::cout;
using std::endl;

wxIMPLEMENT_APP(CLionTestApp);
/*
int main() {
    std::cout << "Hello, World!" << std::endl;
    if (Endianness::IsLittleEndian()) {
        cout << "IsLittle" << endl;
    } else {
        cout << "Must be BigEndian" << endl;
    }

    DoubleRange range_screen(0, 0, 639, 479);
    DoubleRange range_window;

    cout << range_window << endl;
    cout << range_screen << endl;
    cout << range_window.fit_copy(range_screen) << endl;
    cout << range_window.fit_copy(range_screen).scale(0.25) << endl;

    std::ostringstream o;
    Mandelbrot mb;
    Mandelbrot mb2(mb.getRange_window(), mb.getRange_screen(), o);
    mb2.setMaxIt(20);
    mb2.run();
    cout << o.str() << endl;

    return 0;
}
 */