#pragma once 

class IOptionsListener
{
public:
    virtual void OnOptionsChanged() = 0;
};

extern LPCTSTR OPTIONS_IgnorePathList;
extern LPCTSTR OPTIONS_EnableIgnoreList;