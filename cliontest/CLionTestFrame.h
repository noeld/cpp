//
// Created by Arnold Müller on 11.02.17.
//

#ifndef CLIONTEST_CLIONTESTFRAME_H
#define CLIONTEST_CLIONTESTFRAME_H


#include <wx/wx.h>
#include "Universe.h"

class CLionTestFrame : public wxFrame, PropertiesReaderWriter {
public:
    CLionTestFrame();

    virtual ~CLionTestFrame();

private:
    void OnPaint(wxPaintEvent &event);
    void OnTimer(wxTimerEvent &);

    void ReadProperties(const Properties &properties) override;

    void WriteProperties(Properties &properties) override;

    void Start();
    void Stop();
    void Restart();

private:
    wxBitmap bmp_;
    wxTimer tim_;
    unsigned c_ { 0 };

    Properties prop_;
    Universe universe{500};

    unsigned simulation_intervall { 50 };
    double   simulation_f { 1.0 };
};


#endif //CLIONTEST_CLIONTESTFRAME_H
