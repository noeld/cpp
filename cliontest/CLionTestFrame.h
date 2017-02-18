//
// Created by Arnold Müller on 11.02.17.
//

#ifndef CLIONTEST_CLIONTESTFRAME_H
#define CLIONTEST_CLIONTESTFRAME_H


#include <wx/wx.h>
#include "Universe.h"

class CLionTestFrame : public wxFrame {
public:
    CLionTestFrame();

private:
    wxBitmap bmp_;
    wxTimer tim_;
    unsigned c_ { 0 };
    void OnPaint(wxPaintEvent &event);
    Universe universe{500};

    void OnTimer(wxTimerEvent &);
};


#endif //CLIONTEST_CLIONTESTFRAME_H
