#include "settingsparsers.h"

#include <QFile>
#include <QDir>
#include <QHash>
#include <QList>
#include <QMap>

#include <QDebug>

#include "GlobalData/globalfunctions.h"

extern bool boolFromString(QString str);



SettingsParsers::SettingsParsers()
{

}

SettingsParsers::~SettingsParsers()
{

}


//1. Путь к отслеживаемым каталогам и соответствующим настройкам №2 (настройка № 1)
void SettingsParsers::parseSettings1(QString settings_1_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile parse_file_obj(settings_1_path);
    if(parse_file_obj.exists())
    {
        QDomDocument settings_1_document;
        if(!settings_1_document.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек № 1: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(settings_1_document.documentElement().tagName().toUpper() == "SETTING_1")
        {
            QDomElement root = settings_1_document.documentElement();
            QDomNode filters_dir = root.firstChild();
            if(filters_dir.toElement().tagName().toUpper() == "DIR")
            {
                while (!filters_dir.isNull())
                {

                    QString filter_dir_path = filters_dir.toElement().attribute("path");

                    Settings::settings()->replaceVariablesIn(filter_dir_path);

                    createDirIfDoesNotExist(filter_dir_path);

                    QString settings_2_path = filters_dir.toElement().attribute("setting_2");

                    Settings::settings()->replaceVariablesIn(settings_2_path);

                    parseSettings2(settings_2_path, filter_dir_path);


                    filters_dir = filters_dir.nextSibling();
                }
            }
            else
            {
                QString error_text = "Ошибка при чтении файла настроек №1: неверное имя тега отслеживаемых директорий. \n Строка "
                                     + QString::number(filters_dir.lineNumber()) + ", символ " + QString::number(filters_dir.columnNumber()) + ".";
                qDebug() << error_text;
            }
        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №1: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №1: файл настроек отсутствует.";
        qDebug() << error_text  << settings_1_path;
    }

}

//2. Действия при поступлении файла в один из отслеживаемых каталогов на серевере (настройка № 2)
void SettingsParsers::parseSettings2(QString settings_2_path, QString filter_dir_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile parse_file_obj(settings_2_path);
    if(parse_file_obj.exists())
    {
        QDomDocument settings_2_document;
        if(!settings_2_document.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек № 2: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(settings_2_document.documentElement().tagName().toUpper() == "SETTING_2")
        {
            QDomElement root = settings_2_document.documentElement();
            QDomNode file_node = root.firstChild();

            QList<ReceivingFileActions*> files_execut_params;
            QString junk_path;

            while(!file_node.isNull())
            {
                if(file_node.toElement().tagName().toUpper() == "FILE")
                {
                    ReceivingFileActions* file_actions = new ReceivingFileActions();

                    QString buffer_file_name = file_node.toElement().attribute("name");
                    Settings::settings()->replaceVariablesIn(buffer_file_name);
                    QRegExp file_name_reg_exp(buffer_file_name);
                    QDomNode action_node = file_node.firstChild();

                    QList<EventExecutionParameters*> actions_list;

                    while(!action_node.isNull())
                    {

                        if(action_node.toElement().tagName().toUpper() == "ACTION")
                        {

                            EventExecutionParameters* action_parameters = new EventExecutionParameters();


                            int action_type = action_node.toElement().attribute("type").toInt();
                            action_parameters->task_type = action_type;

                            action_parameters->message_for_user = action_node.toElement().attribute("message");

                            action_parameters->skip = boolFromString(action_node.toElement().attribute("skip", "0"));

                            QDomNode parameters_node = action_node.firstChild();




                            if(parameters_node.toElement().tagName().toUpper() == "PARAMETERS")
                            {
                                QString parameters_string = formParametersString(action_type, &parameters_node);
                                if(parameters_string == "")
                                {
                                    QString error_text = "Ошибка при чтении файла настроек №2 " + parse_file_obj.fileName() + " : ошибка при чтении параметров команды для поступившего файла " + file_name_reg_exp.pattern() + ".";
                                    qDebug() << error_text << action_type;
                                }
                                else
                                {
                                    action_parameters->task_exec_parameters = parameters_string;
                                    actions_list.append(action_parameters);
                                }
                            }
                            else
                            {
                                QString error_text = "Ошибка при чтении файла настроек №2: неверное имя тега параметров действия.";
                                qDebug() << error_text;
                            }

                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №2: неверное имя тега выполняемого действия.";
                            qDebug() << error_text;
                        }

                        action_node = action_node.nextSibling();
                    }

                    file_actions->file_name_reg_exp = file_name_reg_exp;
                    file_actions->actions = actions_list;

                    files_execut_params.append(file_actions);

                }
                else if(file_node.toElement().tagName().toUpper() == "JUNK")
                {
                    junk_path = file_node.toElement().attribute("path");

                    Settings::settings()->replaceVariablesIn(junk_path);
                    createDirIfDoesNotExist(junk_path);
                }
                else
                {
                    QString error_text = "Ошибка при чтении файла настроек №2: неверное имя тега обнаруживаемого файла. \n Строка "
                                         + QString::number(file_node.lineNumber()) + ", символ " + QString::number(file_node.columnNumber()) + ".";
                    qDebug() << error_text;
                }
                file_node = file_node.nextSibling();

            }

            Settings::settings()->directoryManager()->lookForDir(filter_dir_path, files_execut_params, junk_path);

        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №2: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №2: файл настроек отсутствует.";
        qDebug() << error_text  << settings_2_path;
    }

}

QString SettingsParsers::formParametersString(int action_type, QDomNode *parameters_node)
{
    QString splitter = "!";
    QString parameters_string = "";
    switch (action_type)
    {
        case 1:
        {
            parameters_string = parameters_node->toElement().attribute("scen_path") + splitter + parameters_node->toElement().attribute("wait_list_id", "0");
            break;
        }
        case 2:
        {
            parameters_string = parameters_node->toElement().attribute("file_path");
            break;
        }
        case 3:
        {
            parameters_string = parameters_node->toElement().attribute("source_path") + splitter + parameters_node->toElement().attribute("target_path") + splitter + parameters_node->toElement().attribute("write_to_db", "0");
            break;
        }
        case 4:
        {
            parameters_string = parameters_node->toElement().attribute("delete_path");
            break;
        }
        case 5:
        {
            parameters_string = parameters_node->toElement().attribute("pack_path") + splitter + parameters_node->toElement().attribute("unpack_path");
            break;
        }
        case 6:
        {
            parameters_string = parameters_node->toElement().attribute("manual_path");
            break;
        }
        case 7:
        {
            parameters_string = parameters_node->toElement().attribute("var_name") + splitter + parameters_node->toElement().attribute("start_marker") + splitter + parameters_node->toElement().attribute("finish_marker");
            break;
        }
        case 8:
        {
            parameters_string = parameters_node->toElement().attribute("var_name") + splitter + parameters_node->toElement().attribute("operator") + splitter + parameters_node->toElement().attribute("number");
            break;
        }
        case 9:
        {
            parameters_string = parameters_node->toElement().attribute("settings_path");
            break;
        }
        case 10:
        {
            parameters_string = parameters_node->toElement().attribute("file_path") + splitter + parameters_node->toElement().attribute("trash_path");
            break;
        }
        case 11:
        {
            parameters_string = parameters_node->toElement().attribute("settings_path");
            break;
        }
        case 12:
        {
            parameters_string = parameters_node->toElement().attribute("temp_file_path") + splitter + parameters_node->toElement().attribute("file_name");
            break;
        }
        default:
        {
            QString error_text = "Ошибка при чтении файла настроек №2: неизвестный тип действия";
            qDebug() << error_text;
            break;
        }
    }

    return parameters_string;
}


//3. Структура для разбора плана (настройка № 3)
void SettingsParsers::parseSettings3(QString settings_3_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile parse_file_obj(settings_3_path);
    if(parse_file_obj.exists())
    {
        QDomDocument settings_3_document;
        if(!settings_3_document.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек № 3 " + settings_3_path + ": \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(settings_3_document.documentElement().tagName().toUpper() == "SETTING_3")
        {
            QDomElement root = settings_3_document.documentElement();
            QDomNode settings_node = root.firstChild();

            PlanParseRules *plan_parse_rules = new PlanParseRules();

            while(!settings_node.isNull())
            {
                if(settings_node.toElement().tagName().toUpper() == "PLAN")
                {
                    plan_parse_rules->plan_tag = settings_node.toElement().attribute("plan_tag");
                }
                else if(settings_node.toElement().tagName().toUpper() == "WORKPROGRAM")
                {
                    plan_parse_rules->work_program_tag = settings_node.toElement().attribute("work_program_tag");
                    plan_parse_rules->work_program_parameter_index = settings_node.toElement().attribute("work_program_parameter_index");
                    plan_parse_rules->work_program_parameter_begin_time = settings_node.toElement().attribute("work_program_parameter_begin_time");
                    plan_parse_rules->work_program_parameter_end_time = settings_node.toElement().attribute("work_program_parameter_end_time");

                }
                else if(settings_node.toElement().tagName().toUpper() == "DROP")
                {
                    plan_parse_rules->drop_tag = settings_node.toElement().attribute("drop_tag");
                    plan_parse_rules->drop_parameter_drop_index = settings_node.toElement().attribute("drop_parameter_drop_index");
                    plan_parse_rules->drop_parameter_circle_index = settings_node.toElement().attribute("drop_parameter_circle_index");
                    plan_parse_rules->drop_parameter_begin_time = settings_node.toElement().attribute("drop_parameter_begin_time");
                    plan_parse_rules->drop_parameter_end_time = settings_node.toElement().attribute("drop_parameter_end_time");
                }
                else
                {
                    QString error_text = "Ошибка при чтении файла настроек №3: неверное имя тега секции настройки. \n Строка "
                                         + QString::number(settings_node.lineNumber()) + ", символ " + QString::number(settings_node.columnNumber()) + ".";
                    qDebug() << error_text;
                }
                settings_node = settings_node.nextSibling();
            }

            Settings::settings()->setPlanParseRules(plan_parse_rules);
        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №3: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №3: файл настроек отсутствует.";
        qDebug() << error_text  << settings_3_path;
    }
}



//4. Действия при поступлении файла в один из отслеживаемых каталогов на клиенте(настройка № 4)
void SettingsParsers::parseSettings4(QString settings_4_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile parse_file_obj(settings_4_path);
    if(parse_file_obj.exists())
    {
        QDomDocument settings_4_document;
        if(!settings_4_document.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек № 4: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(settings_4_document.documentElement().tagName().toUpper() == "SETTING_4")
        {
            QDomElement root = settings_4_document.documentElement();
            QDomNode file_node = root.firstChild();

            QList<ReceivingFileActions*> files_execut_params;;
            QString junk_path;


            while(!file_node.isNull())
            {
                if(file_node.toElement().tagName().toUpper() == "FILE")
                {
                    ReceivingFileActions* file_actions = new ReceivingFileActions();

                    QString buffer_file_name = file_node.toElement().attribute("name");
                    Settings::settings()->replaceVariablesIn(buffer_file_name);
                    QRegExp file_name_reg_exp(buffer_file_name);
                    QDomNode action_node = file_node.firstChild();;


                    QList<EventExecutionParameters*> actions_list;

                    while(!action_node.isNull())
                    {

                        if(action_node.toElement().tagName().toUpper() == "ACTION")
                        {

                            EventExecutionParameters* action_parameters = new EventExecutionParameters();


                            int action_type = action_node.toElement().attribute("type").toInt();
                            action_parameters->task_type = action_type;

                            action_parameters->message_for_user = action_node.toElement().attribute("message");

                            action_parameters->skip = boolFromString(action_node.toElement().attribute("skip", "0"));

                            QDomNode parameters_node = action_node.firstChild();




                            if(parameters_node.toElement().tagName().toUpper() == "PARAMETERS")
                            {
                                QString parameters_string = formParametersString(action_type, &parameters_node);
                                if(parameters_string == "")
                                {
                                    QString error_text = "Ошибка при чтении файла настроек №4: ошибка при чтении параметров команды для поступившего файла " + file_name_reg_exp.pattern() + ".";
                                    qDebug() << error_text;
                                }
                                else
                                {
                                    action_parameters->task_exec_parameters = parameters_string;
                                    actions_list.append(action_parameters);
                                }
                            }
                            else
                            {
                                QString error_text = "Ошибка при чтении файла настроек №4: неверное имя тега параметров действия.";
                                qDebug() << error_text;
                            }

                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №4: неверное имя тега выполняемого действия.";
                            qDebug() << error_text;
                        }

                        action_node = action_node.nextSibling();
                    }

                    file_actions->file_name_reg_exp = file_name_reg_exp;
                    file_actions->actions = actions_list;

                    files_execut_params.append(file_actions);

                }
                else if(file_node.toElement().tagName().toUpper() == "JUNK")
                {
                    junk_path = file_node.toElement().attribute("path");
                    Settings::settings()->replaceVariablesIn(junk_path);
                    createDirIfDoesNotExist(junk_path);
                }
                else
                {
                    QString error_text = "Ошибка при чтении файла настроек №4: неверное имя тега обнаруживаемого файла. \n Строка "
                                         + QString::number(file_node.lineNumber()) + ", символ " + QString::number(file_node.columnNumber()) + ".";
                    qDebug() << error_text;
                }
                file_node = file_node.nextSibling();

            }

            Settings::settings()->clientsFileExecutor()->setExecutionSettings(files_execut_params);
            Settings::settings()->clientsFileExecutor()->setJunkDir(junk_path);

        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №4: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №4: файл настроек отсутствует.";
        qDebug() << error_text  << settings_4_path;
    }
}


//5. Роли и группы с указанием выполняемых сценариев(настройка № 5)
void SettingsParsers::parseSettings5(QString settings_5_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile parse_file_obj(settings_5_path);
    if(parse_file_obj.exists())                                                             //если файл настроек существует
    {

        QDomDocument doc;

        if (!doc.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))    //если файл НЕ читается в дом-структуру
        {
            QString error_text = "Ошибка при чтении файла настроек №5: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        else                                                                                //если файл читается в дом-структуру
        {
            QDomElement root = doc.documentElement();                                       //<GROUPS>
            QDomNode groupNode = root.firstChild();                                         //<group>
            while (!groupNode.isNull())                                                     //пока не конец дом-структуры
            {
                if(groupNode.toElement().tagName().toUpper() == "GROUP")
                {
                    QString group_name = groupNode.toElement().attribute("name");
                    Settings::settings()->replaceVariablesIn(group_name);

                    QDomNode node = groupNode.firstChild(); //<default/role>
                    QMap<QString, QStringList> group_roles;
                    while(!node.isNull())
                    {
                        if (node.toElement().tagName().toUpper() == "ROLE")
                        {
                            QString roleName = node.toElement().attribute("name");
                            Settings::settings()->replaceVariablesIn(roleName);

                            QStringList scens;
                            scens = node.toElement().attribute("scens").split(", ");


                            for(QString scen : scens)
                                Settings::settings()->replaceVariablesIn(scen);


                            group_roles[roleName] = scens;
                        }
                        else if(node.toElement().tagName().toUpper() == "DEFAULT")
                        {
                            QString default_role = node.toElement().attribute("name");
                            Settings::settings()->replaceVariablesIn(default_role);


                            Settings::settings()->setDefaultRoleForGroup(group_name, default_role);
                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №5: \n Строка "
                                                 + QString::number(node.lineNumber()) + ", символ " + QString::number(node.columnNumber()) + "\n" +
                                                 "Ошибка в названии тега настроек групп.";
                            qDebug() << error_text;
                            break;
                        }
                        node = node.nextSibling();
                    }
                    Settings::settings()->setRolesForGroup(group_name, group_roles);
                }
                else if(groupNode.toElement().tagName().toUpper() == "DEFAULTEXECUTOR")
                {
                    QString default_executor = groupNode.toElement().attribute("name");
                    Settings::settings()->replaceVariablesIn(default_executor);

                    Settings::settings()->setDafaultExecutorRole(groupNode.toElement().attribute("name"));
                }
                else
                {
                    QString error_text = "Ошибка при чтении файла настроек №5: \n Строка "
                                         + QString::number(groupNode.lineNumber()) + ", символ " + QString::number(groupNode.columnNumber()) + "\n" +
                                         "Ошибка в названии тега настроек групп.";
                    qDebug() << error_text;
                    break;
                }
                groupNode = groupNode.nextSibling();
            }
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №5" + settings_5_path + ": файл настроек отстуствует.";
        qDebug() << error_text;
    }
}

//6. Переменные пользователя, глобальные пути программы (настройка 6)
void SettingsParsers::parseSettings6(QString settings_6_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;


    QFile parse_file_obj(settings_6_path);
    if(parse_file_obj.exists())
    {
        QDomDocument doc;
        if(!doc.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек №6: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(doc.documentElement().tagName().toUpper() == "SETTING_6")
        {
            QDomElement root = doc.documentElement();
            QDomNode node = root.firstChild();
            while (!node.isNull())
            {
                if(node.toElement().tagName().toUpper() == "DIR_CONSTANTS")
                {
                    QDomNode constant = node.firstChild();
                    while (!constant.isNull())
                    {
                        if(constant.toElement().tagName().toUpper() == "CONST")
                        {
                            QString const_name = constant.toElement().attribute("name");
                            QString const_value = constant.toElement().attribute("value");
                            QString type = constant.toElement().attribute("type");
                            VariableType const_type;

                            if(type == "str")
                            {
                                const_type = string;
                                Settings::settings()->createVar(const_name, const_type, const_value);
                                if(createDirIfDoesNotExist(const_value) == false)
                                {
                                    qDebug() << "Ошибка! Не удалось создать каталог. " << const_value;
                                }
                            }
                            else if(type == "int")
                            {
                                qDebug() << "Ошибка! Задан неверный тип для константы пути к директории. " << const_name;
                            }
                            else
                            {
                                qDebug() << "Некорректный тип константы " << const_name;
                            }
                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №6: неверное имя тега константы пути. \n Строка "
                                                 + QString::number(constant.lineNumber()) + ", символ " + QString::number(constant.columnNumber()) + ".";
                            qDebug() << error_text;
                        }
                        constant = constant.nextSibling();
                    }
                }
                else if(node.toElement().tagName().toUpper() == "CONSTANTS")
                {
                    QDomNode constant = node.firstChild();
                    while (!constant.isNull())
                    {
                        if(constant.toElement().tagName().toUpper() == "CONST")
                        {
                            QString const_name = constant.toElement().attribute("name");
                            QString const_value = constant.toElement().attribute("value");
                            QString type = constant.toElement().attribute("type");
                            VariableType const_type;

                            if(type == "str")
                            {
                                const_type = string;
                                Settings::settings()->createVar(const_name, const_type, const_value);
                            }
                            else if(type == "int")
                            {
                                const_type = string;
                                Settings::settings()->createVar(const_name, const_type, const_value);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип константы " << const_name;
                            }
                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №6: неверное имя тега константы пути. \n Строка "
                                                 + QString::number(constant.lineNumber()) + ", символ " + QString::number(constant.columnNumber()) + ".";
                            qDebug() << error_text;
                        }
                        constant = constant.nextSibling();
                    }
                }
                else if(node.toElement().tagName().toUpper() == "VARIABLES")
                {
                    QDomNode variable = node.firstChild();
                    while (!variable.isNull())
                    {
                        if(variable.toElement().tagName().toUpper() == "VAR_WORK_PROGRAM_NUMBER")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForWorkProgramCurrentIndex(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                var_type = undefined;
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_CURR_ROTATE_NUMBER")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForRotateCurrentIndex(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_SEANCE_NUMBER")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForSeanceCurrentIndex(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }


                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_MESSAGE_FILE_PATH")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForMessageFilePath(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_RETURN_CODE")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForReturnCode(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_MESSAGE_FILE_NAME")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForMessageFileName(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_PACKER_PREKEYS")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForPackerPrekeys(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_PACKER_AFTERKEYS")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForPackerAfterkeys(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №6: неверное имя тега переменной. \n Строка "
                                                 + QString::number(variable.lineNumber()) + ", символ " + QString::number(variable.columnNumber()) + ".";
                            qDebug() << error_text;
                        }
                        variable = variable.nextSibling();
                    }
                }
                else if(node.toElement().tagName().toUpper() == "WAIT_LISTS")
                {
                    QDomNode list_node = node.firstChild();
                    while(!list_node.isNull())
                    {
                        QDomElement list_element = list_node.toElement();
                        if(list_element.tagName().toUpper() == "LIST")
                        {
                            WaitListClearEventActions *new_wait_list_clear_event_actions = new WaitListClearEventActions();
                            new_wait_list_clear_event_actions->wait_list_id = list_element.attribute("id");
                            new_wait_list_clear_event_actions->message = list_element.attribute("message");


                            QString settings_9_path_buffer = list_element.attribute("actions_file");
                            Settings::settings()->replaceVariablesIn(settings_9_path_buffer);
                            QString settings_9_path = settings_9_path_buffer;

                            new_wait_list_clear_event_actions->events = SettingsParsers::parseSettings9(settings_9_path);

                            if(!new_wait_list_clear_event_actions->events.isEmpty())
                            {
                                Settings::settings()->setWaitListClearEventActions(new_wait_list_clear_event_actions);
                            }
                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №6: неверное имя тега листа ожидания. \n Строка "
                                                 + QString::number(list_element.lineNumber()) + ", символ " + QString::number(list_element.columnNumber()) + ".";
                            qDebug() << error_text;
                        }
                        list_node = list_node.nextSibling();
                    }

                }
                else if(node.toElement().tagName().toUpper() == "PACKER_COMMANDS")
                {
                    Settings::settings()->setPackCommand(node.toElement().attribute("pack"));
                    QString str = node.toElement().attribute("pack");
                    Settings::settings()->setUnpackCommand(node.toElement().attribute("unpack"));
                    QString str1 = node.toElement().attribute("unpack");
                }
                else
                {
                    QString error_text = "Ошибка при чтении файла настроек №6: неверное имя тега второго уровня. \n Строка "
                                         + QString::number(node.lineNumber()) + ", символ " + QString::number(node.columnNumber()) + ".";
                    qDebug() << error_text;
                }
                node = node.nextSibling();
            }
        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №6: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №6: файл настроек отсутствует.";
        qDebug() << error_text;
    }
}

//7. Содержимое справки и упаковки (настройка № 7)
PackSettings* SettingsParsers::parseSettings7(QString settings_7_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;


    QFile parse_file_obj(settings_7_path);
    if(parse_file_obj.exists())
    {
        QDomDocument manual_forming_document;
        if(!manual_forming_document.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек №7: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(manual_forming_document.documentElement().tagName().toUpper() == "SETTING_7")
        {
            QDomElement root = manual_forming_document.documentElement();
            QDomNode search_dir_node = root.firstChild();

            PackSettings *current_pack_settings = new PackSettings();

            if(search_dir_node.toElement().tagName().toUpper() == "SEARCH_DIR")
            {
                QString search_dir_path = search_dir_node.toElement().attribute("path");

                Settings::settings()->replaceVariablesIn(search_dir_path);

                current_pack_settings->search_dir_path = search_dir_path;

                QDomNode pack_creation_dir_node = search_dir_node.nextSibling();

                if(pack_creation_dir_node.toElement().tagName().toUpper() == "PACK_CREATION_DIR")
                {
                    QString pack_creation_dir_path = pack_creation_dir_node.toElement().attribute("path");

                    Settings::settings()->replaceVariablesIn(pack_creation_dir_path);

                    current_pack_settings->pack_creation_dir_path = pack_creation_dir_path;

                    QDomNode search_algorithm_node = pack_creation_dir_node.nextSibling();
                    if(search_algorithm_node.toElement().tagName().toUpper() == "SEARCH_ALGORITHM")
                    {

                        QString search_algoritm_type = search_algorithm_node.toElement().attribute("type", "0");

                        Settings::settings()->replaceVariablesIn(search_algoritm_type);

                        current_pack_settings->search_algoritm_type = search_algoritm_type;

                        QDomNode files_node = search_algorithm_node.nextSibling();
                        if(files_node.toElement().tagName().toUpper() == "FILES")
                        {

                            QDomNode file_node = files_node.firstChild();

                            while(!file_node.isNull())
                            {


                                if(file_node.toElement().tagName().toUpper() == "FILE")
                                {
                                    SettingsForPackFile *settings_for_pack_file = new SettingsForPackFile();

                                    QString file_name = file_node.toElement().attribute("name");
                                    Settings::settings()->replaceVariablesIn(file_name);

                                    QRegExp file_name_reg_exp(file_name);

                                    settings_for_pack_file->file_name_reg_exp = file_name_reg_exp;

                                    QString file_skip = file_node.toElement().attribute("skip", "0");
                                    Settings::settings()->replaceVariablesIn(file_skip);

                                    settings_for_pack_file->file_skip = file_skip;

                                    QString file_query_number = file_node.toElement().attribute("query_number", "0");
                                    Settings::settings()->replaceVariablesIn(file_query_number);

                                    settings_for_pack_file->file_query_number = file_query_number;

                                    QString file_vars = file_node.toElement().attribute("vars");
                                    Settings::settings()->replaceVariablesIn(file_vars);

                                    settings_for_pack_file->file_vars = file_vars;

                                    //добавление в список на позицию, соответствующую приоритету
                                    if(current_pack_settings->settings_for_pack_files.isEmpty())
                                    {
                                        current_pack_settings->settings_for_pack_files.append(settings_for_pack_file);
                                    }
                                    else
                                    {
                                        int i = 0;
                                        for(SettingsForPackFile* settings : current_pack_settings->settings_for_pack_files)
                                        {
                                            if(settings_for_pack_file->file_query_number <= settings->file_query_number)
                                            {
                                                current_pack_settings->settings_for_pack_files.insert(i, settings_for_pack_file);
                                                break;
                                            }
                                            else if((settings_for_pack_file->file_query_number == 0) ||
                                                    (i + 1 == current_pack_settings->settings_for_pack_files.size()))
                                            {
                                                current_pack_settings->settings_for_pack_files.append(settings_for_pack_file);
                                                break;
                                            }
                                            else
                                                i++;
                                        }
                                    }



                                }
                                else
                                {
                                    QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега упаковываемого файла. \n Строка "
                                                         + QString::number(file_node.lineNumber()) + ", символ " + QString::number(file_node.columnNumber()) + ".";
                                    qDebug() << error_text;
                                }
                                file_node = file_node.nextSibling();
                            }





                            QDomNode pack_file_names_node = files_node.nextSibling();
                            if(pack_file_names_node.toElement().tagName().toUpper() == "PACK_FILE_NAMES")
                            {
                                QDomNode pack_data_file_node = pack_file_names_node.firstChild();


                                if(pack_data_file_node.toElement().tagName().toUpper() == "PACK_DATA_FILE")
                                {
                                    QString pack_data_file_name = pack_data_file_node.toElement().attribute("name");
                                    Settings::settings()->replaceVariablesIn(pack_data_file_name);

                                    current_pack_settings->pack_data_file_name = pack_data_file_name;

                                    QDomNode manual_file_node = pack_data_file_node.nextSibling();
                                    if(manual_file_node.toElement().tagName().toUpper() == "MANUAL_FILE")
                                    {
                                        QString manual_file_name = manual_file_node.toElement().attribute("name");
                                        Settings::settings()->replaceVariablesIn(manual_file_name);

                                        current_pack_settings->manual_file_name = manual_file_name;

                                        QDomNode pack_data_manual_file_node = manual_file_node.nextSibling();
                                        if(pack_data_manual_file_node.toElement().tagName().toUpper() == "PACK_DATA_MANUAL_FILE")
                                        {
                                            QString pack_data_manual_file_name = pack_data_manual_file_node.toElement().attribute("name");
                                            Settings::settings()->replaceVariablesIn(pack_data_manual_file_name);

                                            current_pack_settings->pack_data_manual_file_name = pack_data_manual_file_name;

                                            QDomNode filter_node = pack_file_names_node.nextSibling();
                                            if(filter_node.toElement().tagName().toUpper() == "FILTER")
                                            {
                                                QString filter_path = filter_node.toElement().attribute("path");
                                                Settings::settings()->replaceVariablesIn(filter_path);

                                                current_pack_settings->filter_path = filter_path;

                                                return current_pack_settings;
                                            }
                                            else
                                            {
                                                QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега фильтра. \n Строка "
                                                                     + QString::number(filter_node.lineNumber()) + ", символ " + QString::number(filter_node.columnNumber()) + ".";
                                                qDebug() << error_text;
                                            }

                                        }
                                        else
                                        {
                                            QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега фильтра. \n Строка "
                                                                 + QString::number(pack_data_manual_file_node.lineNumber()) + ", символ " + QString::number(pack_data_manual_file_node.columnNumber()) + ".";
                                            qDebug() << error_text;
                                        }


                                    }
                                    else
                                    {
                                        QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега фильтра. \n Строка "
                                                             + QString::number(manual_file_node.lineNumber()) + ", символ " + QString::number(manual_file_node.columnNumber()) + ".";
                                        qDebug() << error_text;
                                    }


                                }
                                else
                                {
                                    QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега фильтра. \n Строка "
                                                         + QString::number(pack_data_file_node.lineNumber()) + ", символ " + QString::number(pack_data_file_node.columnNumber()) + ".";
                                    qDebug() << error_text;
                                }


                            }
                            else
                            {
                                QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега наименований файлов упаковки. \n Строка "
                                                     + QString::number(pack_file_names_node.lineNumber()) + ", символ " + QString::number(pack_file_names_node.columnNumber()) + ".";
                                qDebug() << error_text;
                            }
                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега упаковываемых файлов. \n Строка "
                                                 + QString::number(files_node.lineNumber()) + ", символ " + QString::number(files_node.columnNumber()) + ".";
                            qDebug() << error_text;
                        }
                    }
                    else
                    {
                        QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега алгоритма поиска. \n Строка "
                                             + QString::number(search_algorithm_node.lineNumber()) + ", символ " + QString::number(search_algorithm_node.columnNumber()) + ".";
                        qDebug() << error_text;
                    }
                }
                else
                {
                    QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега директории формирования упаковки. \n Строка "
                                         + QString::number(pack_creation_dir_node.lineNumber()) + ", символ " + QString::number(pack_creation_dir_node.columnNumber()) + ".";
                    qDebug() << error_text;
                }
            }
            else
            {
                QString error_text = "Ошибка при чтении файла настроек №7: неверное имя тега директории поиска файлов для упаковки. \n Строка "
                                     + QString::number(search_dir_node.lineNumber()) + ", символ " + QString::number(search_dir_node.columnNumber()) + ".";
                qDebug() << error_text;
            }
        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №7: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №7: файл настроек отсутствует.";
        qDebug() << error_text;
    }


    return nullptr;
}

//8. Отображение плана (настройка № 8)   -------- TO DO
void SettingsParsers::parseSettings8(QString settings_8_path, Settings *settings)
{
    Q_UNUSED(settings_8_path)
    Q_UNUSED(settings)
}

//9. Действия по окончанию процесса обработки (настройка 9)
QList<EventExecutionParameters*> SettingsParsers::parseSettings9(QString settings_9_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile parse_file_obj(settings_9_path);
    if(parse_file_obj.exists())
    {
        QDomDocument settings_9_document;
        if(!settings_9_document.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек № 9: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(settings_9_document.documentElement().tagName().toUpper() == "SETTING_9")
        {     

            QDomElement root = settings_9_document.documentElement();
            QDomNode action_node = root.firstChild();

            QList<EventExecutionParameters*> actions_list;

            while(!action_node.isNull())
            {

                if(action_node.toElement().tagName().toUpper() == "ACTION")
                {

                    EventExecutionParameters* action_parameters = new EventExecutionParameters();


                    int action_type = action_node.toElement().attribute("type").toInt();
                    action_parameters->task_type = action_type;

                    action_parameters->message_for_user = action_node.toElement().attribute("message");

                    action_parameters->skip = boolFromString(action_node.toElement().attribute("skip", "0"));

                    QDomNode parameters_node = action_node.firstChild();




                    if(parameters_node.toElement().tagName().toUpper() == "PARAMETERS")
                    {
                        QString parameters_string = formParametersString(action_type, &parameters_node);
                        if(parameters_string == "")
                        {
                            QString error_text = "Ошибка при чтении файла настроек №9: ошибка при чтении параметров команды для списка.";
                            qDebug() << error_text;
                        }
                        else
                        {
                            action_parameters->task_exec_parameters = parameters_string;
                            actions_list.append(action_parameters);
                        }
                    }
                    else
                    {
                        QString error_text = "Ошибка при чтении файла настроек №9: неверное имя тега параметров действия.";
                        qDebug() << error_text;
                    }

                }
                else
                {
                    QString error_text = "Ошибка при чтении файла настроек №9: неверное имя тега выполняемого действия.";
                    qDebug() << error_text;
                }

                action_node = action_node.nextSibling();
            }
            return actions_list;
        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №9: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №9: файл настроек отсутствует.";
        qDebug() << error_text  << settings_9_path;
    }
    return QList<EventExecutionParameters*>();
}

//10. Порты серверов (настройка 10)
void SettingsParsers::parseSettings10(QString settings_10_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;



    QFile parse_file_obj(settings_10_path);
    if(parse_file_obj.exists())                                                         //если файл настроек существует
    {

        QDomDocument doc;

        if (!doc.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))//если файл НЕ читается в дом-структуру
        {
            QString error_text = "Ошибка при чтении файла настроек серверов: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        else                                                                            //если файл читается в дом-структуру
        {
            QDomElement root = doc.documentElement();                                   //<SERVERS>
            QDomNode serverNode = root.firstChild();                                    //<SERVER>
            while (!serverNode.isNull())                                                //пока не конец дом-структуры
            {
                if(serverNode.toElement().tagName().toUpper() == "SERVER")
                {
                    if(serverNode.toElement().attribute("name") == "Сервер для клиентов")
                    {
                        QString buf = serverNode.toElement().attribute("port");
                        Settings::settings()->setPortForClients(buf.toInt());
                    }
                    else if(serverNode.toElement().attribute("name") == "Сервер для сервера состояния")
                    {
                        QString buf = serverNode.toElement().attribute("port");
                        Settings::settings()->setPortForStateManager(buf.toInt());
                    }
                    else
                    {
                        QString error_text = "Ошибка при чтении файла настроек серверов: \n Строка "
                                             + QString::number(serverNode.lineNumber()) + ", символ " + QString::number(serverNode.columnNumber()) + "\n" +
                                             "Ошибка в имени сервера.";
                        break;
                        qDebug() << error_text;
                    }
                }
                else
                {
                    QString error_text = "Ошибка при чтении файла настроек серверов: \n Строка "
                                        + QString::number(serverNode.lineNumber()) + ", символ " + QString::number(serverNode.columnNumber()) + "\n" +
                                        "Ошибка в названии тега настроек сервера.";
                    qDebug() << error_text;
                    break;
                }
                serverNode = serverNode.nextSibling();
            }
        }
    }
    else                                                                                //если файл настроек НЕ существует
    {
        QString error_text = "Ошибка при чтении файла настроек №10: файл настроек отстуствует.";
        qDebug() << error_text;
    }
}


bool SettingsParsers::createDirIfDoesNotExist(QString dir_path)
{
    QDir dir(dir_path);
    if(!dir.exists())
    {
        return dir.mkpath(dir_path);
    }
    return true;
}
