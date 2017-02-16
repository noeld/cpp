//
// Created by Arnold Müller on 11.02.17.
//

#include "CLionTestFrame.h"

CLionTestFrame::CLionTestFrame() : wxFrame(NULL, wxID_ANY, "Hallo"), tim_(this) {
    CreateStatusBar();
    SetStatusText("Hallo");
    SetSize(900, 600);
    Bind(wxEVT_PAINT, &CLionTestFrame::OnPaint, this, wxID_ANY);
    Bind(wxEVT_TIMER, &CLionTestFrame::OnTimer, this, wxID_ANY);
    tim_.Start(50);
}

void CLionTestFrame::OnPaint(wxPaintEvent &event) {
    wxPaintDC dc(this);
    auto s = GetClientSize();
    dc.DrawText("Hallo", s.GetWidth()/2, s.GetHeight()/2);
    for (auto& o : universe.getObjects()) {
        if (!o.isActive())
            continue;
        dc.DrawCircle(o.getPos().getX(), o.getPos().getY(), o.getR());
    }
}

void CLionTestFrame::OnTimer(wxTimerEvent &event) {
    universe.advance(50.0/1000.0);
    Refresh();
}
