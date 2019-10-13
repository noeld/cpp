//
// Created by Arnold Müller on 11.02.17.
//

#ifndef CLIONTEST_CLIONTESTFRAME_H
#define CLIONTEST_CLIONTESTFRAME_H


#include <wx/wx.h>
#include "Universe.h"
#include "Histogram.h"
#include "TimeMeasure.h"
//#include "Histogram.h"

class CLionTestFrame : public wxFrame, PropertiesReaderWriter {
public:
    enum { ID_Menu_Start=1000, ID_Menu_Stop, ID_Menu_Restart, ID_Menu_Reload, ID_Menu_Refill
        , ID_Menu_ForceField, ID_Menu_FF_Incr, ID_Menu_FF_Decr };

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
    //Histogram hist_ { 200, 100};
    TimeMeasure tm_ { false };

    Properties prop_;
    Universe universe{500};
    Histogram<std::remove_reference<decltype(universe.getObjects())>::type> hist_{100, 100};

    unsigned simulation_intervall { 50 };
    double   simulation_f { 1.0 };

    void OnRestart(wxCommandEvent &event);
    void OnStop(wxCommandEvent &event);
    void OnStart(wxCommandEvent &event);

    void OnReload(wxCommandEvent &);
    void OnRefill(wxCommandEvent &);
    void OnForceField(wxCommandEvent &);
    void OnForceFieldIncr(wxCommandEvent &);
    void OnForceFieldDecr(wxCommandEvent &);
};


#endif //CLIONTEST_CLIONTESTFRAME_H
