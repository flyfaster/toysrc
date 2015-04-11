/*
 * wxdumpsyms.h
 *
 *  Created on: Feb 2, 2015
 *      Author: onega
 */

#ifndef WXDUMPSYMS_H_
#define WXDUMPSYMS_H_

class wxdumpsyms : public wxApp {
public:
	wxdumpsyms();
	virtual ~wxdumpsyms();
    bool OnInit();
    int OnExit();
};
DECLARE_APP(wxdumpsyms)
#endif /* WXDUMPSYMS_H_ */
