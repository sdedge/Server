#include "eventactionexecutor_copyfile.h"
#include <QDebug>
#include <QFile>

#include "settings.h"


bool boolFromString(QString str);

EventActionExecutor_CopyFile::EventActionExecutor_CopyFile()
{
    
}

EventActionExecutor_CopyFile::~EventActionExecutor_CopyFile()
{
    
}

bool EventActionExecutor_CopyFile::executeEvent(EventExecutionParameters *action_parameters)
{

    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QStringList arguments = variables_buffer.split(arguments_spacer);

    QString source_path = arguments.at(0);

    QString target_path = arguments.at(1);

    bool write_to_db = boolFromString(arguments.at(2));

    bool success = false;

    //возврат ошибки по-умолчанию
    if(QFile::exists(source_path))                                                                                  //если исходный файл существует
    {
        if(QFile::exists(target_path))                                                                              //если конечный файл уже существует
        {
            if(QFile::remove(target_path))                                                                          //если предварительное удаление прошло успешно
            {
                if(QFile::copy(source_path, target_path))                                                           //если копирование удалось
                {
                    success =  true;                                                                                //копирование прошло успешно
                }
            }
        }
        else                                                                                                        //если конечный файл НЕ существует
        {
            SettingsParsers::createDirIfDoesNotExist(target_path.left(target_path.lastIndexOf("/") + 1));
            if(QFile::copy(source_path, target_path))                                                               //если копирование удалось
            {
                success = true;                                                                                     //копирование прошло успешно
            }
        }
    }

    if(success && write_to_db)
    {
        //report to db
    }


    return success | action_parameters->skip;
}
