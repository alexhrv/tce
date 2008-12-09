/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file WatchPropertiesDialog.cc
 *
 * Implementation of WatchPropertiesDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/spinctrl.h>
#include <wx/statline.h>
#include "WatchPropertiesDialog.hh"
#include "StopPointManager.hh"
#include "Watch.hh"
#include "WxConversion.hh"
#include "ExpressionScript.hh"
#include "ErrorDialog.hh"
#include "ProximToolbox.hh"
#include "ProximConstants.hh"
#include "Conversion.hh"
#include "ProximLineReader.hh"

BEGIN_EVENT_TABLE(WatchPropertiesDialog, wxDialog)
    EVT_BUTTON(wxID_OK, WatchPropertiesDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog.
 * @param manager Stoppoint manager of the simulator.
 * @param handle Watch handle.
 */
WatchPropertiesDialog::WatchPropertiesDialog(
    wxWindow* parent, wxWindowID id, StopPointManager& manager, int handle) :
  wxDialog(parent, id, _T("Watch point properties"), wxDefaultPosition),
    manager_(manager), handle_(handle){

    createContents(this, true, true);

    expressionCtrl_ = dynamic_cast<wxTextCtrl*>(FindWindow(ID_EXPRESSION));
    conditionCtrl_ = dynamic_cast<wxTextCtrl*>(FindWindow(ID_CONDITION));
    ignoreCtrl_ = dynamic_cast<wxSpinCtrl*>(FindWindow(ID_IGNORE_COUNT));
}


/**
 * The Destructor.
 */
WatchPropertiesDialog::~WatchPropertiesDialog() {
}


/**
 * Initializes watch properties in the dialog widgets.
 *
 * @return True, if the watch properties were succesfully transfered to
 *         the dialog widgets.
 */
bool
WatchPropertiesDialog::TransferDataToWindow() {

    const StopPoint& stoppoint = manager_.stopPointWithHandleConst(handle_);
    const Watch& watch = dynamic_cast<const Watch&>(stoppoint);

    dynamic_cast<wxStaticText*>(FindWindow(ID_WATCH_HANDLE))->SetLabel(
        WxConversion::toWxString(handle_));

    expressionCtrl_->SetValue(
        WxConversion::toWxString(watch.expression().script()[0]));

    if (watch.isConditional()) {
        conditionCtrl_->SetValue(
            WxConversion::toWxString(watch.condition().script()[0]));
    }

    ignoreCtrl_->SetValue(watch.ignoreCount());

    return wxDialog::TransferDataToWindow();
}


/**
 * Event handler for the OK-button.
 *
 * Updates the watch properties accordign to the values in the dialog widgets.
 */
void
WatchPropertiesDialog::onOK(wxCommandEvent&) {

    wxTextCtrl* conditionCtrl =
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_CONDITION));
    std::string condition = WxConversion::toString(
        conditionCtrl->GetValue().Trim(true).Trim(false));

    // Test condition script.
    if (condition != "" && !ProximToolbox::testCondition(this, condition)) {
        // Invalid condition script.
        return;
    }

    // Set watch condition script.
    std::string conditionCommand = ProximConstants::SCL_SET_CONDITION +
        " " + Conversion::toString(handle_);
    ProximLineReader& lineReader = ProximToolbox::lineReader();
    lineReader.input(conditionCommand);
    lineReader.input(condition);

    // Set watch ignore count.
    unsigned ignoreCount = ignoreCtrl_->GetValue();
    std::string ignoreCommand = ProximConstants::SCL_SET_IGNORE_COUNT +
        " " + Conversion::toString(handle_) +
        " " + Conversion::toString(ignoreCount);
    lineReader.input(ignoreCommand);

    // Close dialog.
    EndModal(wxID_OK);
}


/**
 * Creates the dialog contents.
 *
 * Code generated by wxDesgigner. Do not modify manually.
 *
 * @param parent Parent window of the widgets.
 * @param call_fit If true, the dialog is resized to fit the widgets.
 * @param set_sizer If true, the created widgets are set as the parent window
 *        contents.
 */
wxSizer*
WatchPropertiesDialog::createContents(
    wxWindow* parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_LABEL_WATCH, wxT("Watch:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item3 = new wxStaticText( parent, ID_WATCH_HANDLE, wxT("??????"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_EXPRESSION, wxT("Expression:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_EXPRESSION, wxT(""), wxDefaultPosition, wxSize(400,-1), wxTE_READONLY );
    item1->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_LABEL_CONDITION, wxT("Condition:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_CONDITION, wxT(""), wxDefaultPosition, wxSize(400,-1), 0 );
    item1->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item8 = new wxStaticText( parent, ID_LABEL_IGNORE_COUNT, wxT("Ignore count:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item8, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item9 = new wxSpinCtrl( parent, ID_IGNORE_COUNT, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 10000, 0 );
    item1->Add( item9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item10 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item11 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item12 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item12, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item13 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item11, 0, wxALIGN_CENTER, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
