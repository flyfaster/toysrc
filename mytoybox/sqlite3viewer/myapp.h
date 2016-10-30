/*
 * myapp.h
 *
 *  Created on: Oct 30, 2016
 *      Author: onega
 */

#ifndef MYAPP_H_
#define MYAPP_H_

class MyApp: public wxApp
{
    wxFrame* m_frame;
public:

    bool OnInit();

};
DECLARE_APP(MyApp)



#endif /* MYAPP_H_ */
