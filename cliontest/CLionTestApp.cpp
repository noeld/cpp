//
// Created by Arnold Müller on 11.02.17.
//

#include "CLionTestApp.h"
#include "CLionTestFrame.h"

bool CLionTestApp::OnInit() {
    CLionTestFrame* frame = new CLionTestFrame();
    frame->Show(true);
    frame->SetSize(1000, 700);
    return true; //wxAppConsoleBase::OnInit();
}
