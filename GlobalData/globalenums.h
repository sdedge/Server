#ifndef GLOBALENUMS_H
#define GLOBALENUMS_H


enum ActionType
{
    UNDEFINED,
    StartAction,
    CopyAction,
    DeleteAction,
    GetAction,
    SendAction
};


static ActionType fromIntToActionType(int type)
{
    switch (type)
    {
        case 1:
        {
            return StartAction;
        }
        case 2:
        {
            return  CopyAction;
        }
        case 3:
        {
            return DeleteAction;
        }
        case 4:
        {
            return GetAction;
        }
        case 5:
        {
            return SendAction;
        }
    default:
        return UNDEFINED;
    }
}


#endif // GLOBALENUMS_H
