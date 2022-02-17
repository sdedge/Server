#include "settings.h"

#include <QDebug>
#include <QThread>


Settings* Settings::obj_ptr = nullptr;


Settings::Settings()
{
    directory_manager = new DirectoryManager();
    clients_file_executor = new EventExecutor();
    wait_list_clear_events_executor = new EventExecutor();
    plan_timer = new Timer();
    m_port_for_clients = -1;
    m_port_for_state_manager = -1;
}

void Settings::createNewWaitList(QString scen_name, QString list_id)
{
    WaitList *new_wait_list = new WaitList(list_id);
    connect(new_wait_list, &WaitList::listIsEmpty, this, &Settings::executeEmptyWaitListEvent);
    new_wait_list->addScen(scen_name);
    wait_lists.append(new_wait_list);
}


Settings* Settings::settings()
{
    if(obj_ptr == nullptr)
    {
        obj_ptr = new Settings();
    }
    return obj_ptr;
}

Settings::~Settings()
{
    if(directory_manager != nullptr)
    {
        delete directory_manager;
        directory_manager = nullptr;
    }

    if(wait_list_clear_events_executor != nullptr)
    {
        delete wait_list_clear_events_executor;
        wait_list_clear_events_executor = nullptr;
    }

    if(clients_file_executor != nullptr)
    {
        delete clients_file_executor;
        clients_file_executor = nullptr;
    }

    if(plan_timer != nullptr)
    {
        delete plan_timer;
        plan_timer = nullptr;
    }

    if(m_plan_parse_rules != nullptr)
    {
        delete m_plan_parse_rules;
        m_plan_parse_rules = nullptr;
    }

}

bool Settings::replaceVariablesIn(QString &attribute)
{
    bool done_without_errors = true;                                                        //флаг отсутствия ошибок в процессе замены переменных
    int first_variable_occurrence;
    QRegExp variable_reg_exp("@*@");
    if(attribute.contains(variable_reg_exp))
    {
        first_variable_occurrence = attribute.indexOf("@");
        QString variable_name = attribute.mid(first_variable_occurrence + 1, attribute.indexOf("@", first_variable_occurrence + 1) - first_variable_occurrence - 1);
        Variable *var = Settings::settings()->searchVar(variable_name);
        if(var != nullptr)                                                                  //проверка наличия переменной с таким именем
        {
            attribute.replace(first_variable_occurrence, var->name.size() + 2, var->value); //замена имени переменной на её значение
            done_without_errors = replaceVariablesIn(attribute);                            //рекурсивный вызов, до тех пор, пока не будут заменены все переменные в атрибуте
        }
        else
        {
           qDebug() << "Не найдена переменная " << variable_name;
           done_without_errors = false;
        }
    }
    return done_without_errors;
}

bool Settings::replaceVariablesIn(QString &attribute, QList<Variable *> variables)
{
    bool done_without_errors = true;                                                        //флаг отсутствия ошибок в процессе замены переменных
    int first_variable_occurrence;
    QRegExp variable_reg_exp("@*@");
    if(attribute.contains(variable_reg_exp))
    {
        first_variable_occurrence = attribute.indexOf("@");
        QString variable_name = attribute.mid(first_variable_occurrence + 1, attribute.indexOf("@", first_variable_occurrence + 1) - first_variable_occurrence - 1);
        Variable *var = Settings::searchVar(variable_name,variables);
        if(var != nullptr)                                                                  //проверка наличия переменной с таким именем
        {
            attribute.replace(first_variable_occurrence, var->name.size() + 2, var->value); //замена имени переменной на её значение
            done_without_errors = replaceVariablesIn(attribute, variables);                 //рекурсивный вызов, до тех пор, пока не будут заменены все переменные в атрибуте
        }
        else
        {
           qDebug() << "Не найдена переменная " << variable_name;
           done_without_errors = false;
        }
    }
    return done_without_errors;
}


void Settings::setPortForClients(int port_for_clients)
{
    m_port_for_clients = port_for_clients;
}

int Settings::portForClients() const
{
    return m_port_for_clients;
}

void Settings::setPortForStateManager(int port_for_state_manager)
{
    m_port_for_state_manager = port_for_state_manager;
}

int Settings::portForStateManager() const
{
    return m_port_for_state_manager;
}

DirectoryManager* Settings::directoryManager() const
{
    return directory_manager;
}

EventExecutor *Settings::clientsFileExecutor() const
{
    return clients_file_executor;
}

void Settings::executeFileFromClient(QString file_name, QTcpSocket *socket)
{

}

Variable *Settings::searchVar(QString var_name)
{
    for (Variable* var: vars)
    {
        if(var->name == var_name)
            return var;
    }
    qDebug() << "Переменная " + var_name + " не существует.";
    return nullptr;
}

Variable *Settings::searchVar(QString var_name, QList<Variable *> variables)
{
    for (Variable* var: variables)
    {
        if(var->name == var_name)
            return var;
    }

    return nullptr;
}

bool Settings::setValueToVar(QString var_name, QString value)
{
    for (Variable* var: vars)
    {
        if(var->name == var_name)
        {
            var->value = value;
            return true;
        }
    }
    qDebug() << "Переменная " + var_name + " не существует.";
    return  false;
}

bool Settings::setValueToVar(QString var_name, QString value, QList<Variable *> variables)
{
    for (Variable* var: variables)
    {
        if(var->name == var_name)
        {
            var->value = value;
            return true;
        }
    }
    qDebug() << "Переменная " + var_name + " не существует.";
    return  false;
}



bool Settings::createVar(QString var_name, VariableType type, QString value)
{
    for (Variable* var: vars)
    {
        if(var->name == var_name)
        {
            return false;
        }
    }

    Variable *new_var = new Variable();
    new_var->name = var_name;
    new_var->type = type;
    new_var->value = value;

    vars.append(new_var);

    return true;
}

QList<Variable *> Settings::getCurrentVariables() const
{
    return vars;
}


void Settings::setVarNameForWorkProgramCurrentIndex(QString var_name)
{
    var_name_work_program_current_index = var_name;
}

QString Settings::getVarNameForWorkProgramCurrentIndex() const
{
    return var_name_work_program_current_index;
}

void Settings::setVarNameForSeanceCurrentIndex(QString var_name)
{
    var_name_seance_current_index = var_name;
}

QString Settings::getVarNameForSeanceCurrentIndex() const
{
    return var_name_seance_current_index;
}

void Settings::setVarNameForRotateCurrentIndex(QString var_name)
{
    var_name_rotate_index = var_name;
}

QString Settings::getVarNameForRotateCurrentIndex() const
{
    return var_name_rotate_index;
}

void Settings::setVarNameForMessageFilePath(QString var_name)
{
    var_name_message_file_path = var_name;
}

QString Settings::getVarNameForMessageFilePath() const
{
    return var_name_message_file_path;
}

void Settings::setVarNameForMessageFileName(QString var_name)
{
    var_name_message_file_name = var_name;
}

QString Settings::getVarNameForMessageFileName() const
{
    return var_name_message_file_name;
}

void Settings::setVarNameForReturnCode(QString var_name)
{
    var_name_return_code = var_name;
}

QString Settings::getVarNameForReturnCode() const
{
    return var_name_return_code;
}

void Settings::setVarNameForPackerPrekeys(QString var_name)
{
    var_name_packer_prekeys = var_name;
}
QString Settings::getVarNameForPackerPrekeys() const
{
    return var_name_packer_prekeys;
}

void Settings::setVarNameForPackerAfterkeys(QString var_name)
{
    var_name_packer_afterkeys = var_name;
}
QString Settings::getVarNameForPackerAfterkeys() const
{
    return var_name_packer_afterkeys;
}


PlanParseRules *Settings::planParseRules() const
{
    return m_plan_parse_rules;
}

void Settings::setPlanParseRules(PlanParseRules *plan_parse_rules)
{
    m_plan_parse_rules = plan_parse_rules;
}



void Settings::setPackCommand(QString command)
{
    m_pack_command = command;
}

QString Settings::packCommand() const
{
    return m_pack_command;
}

void Settings::setUnpackCommand(QString command)
{
    m_unpack_command = command;
}

QString Settings::unpackCommand() const
{
    return m_unpack_command;
}

Timer *Settings::timer() const
{
    return plan_timer;
}

void Settings::insertScenInWaitList(QString scen_name, QString list_id)
{
    if(wait_lists.isEmpty())
    {
        createNewWaitList(scen_name, list_id);
    }
    else
    {
        bool list_finded = false;
        for(WaitList *wait_list : wait_lists)
        {
            if(wait_list->listId() == list_id)
            {
                wait_list->addScen(scen_name);
                list_finded = true;
            }
        }
        if(list_finded == false)
        {
            createNewWaitList(scen_name, list_id);
        }
    }
}

void Settings::removeScenFromWaitList(QString scen_name, QString list_id)
{
    for(WaitList *wait_list : wait_lists)
    {
        if(wait_list->listId() == list_id)
        {
            wait_list->removeScen(scen_name);
        }
    }
}

void Settings::setWaitListClearEventActions(WaitListClearEventActions *actions)
{
    wait_lists_clear_event_actions.append(actions);
}

QMap<QString, QString> Settings::defaultRolesMap() const
{
    return default_roles;
}

void Settings::setDefaultRoleForGroup(QString group, QString default_role)
{
    default_roles.insert(group, default_role);
}

QMap<QString, QMap<QString, QStringList> > Settings::groupsRolesMap() const
{
    return group_roles;
}

void Settings::setRolesForGroup(QString group, QMap<QString, QStringList> roles)
{
    group_roles.insert(group, roles);
}

QString Settings::defaultExecutorRole() const
{
    return default_executor;
}

void Settings::setDafaultExecutorRole(QString role)
{
    default_executor = role;
}



void Settings::executeEmptyWaitListEvent(QString id)
{
    for(WaitListClearEventActions *list_actions: wait_lists_clear_event_actions)
    {
        if(list_actions->wait_list_id == id)
        {
            qDebug() << list_actions->message;
            wait_list_clear_events_executor->executeActions(list_actions->events);
            break;
        }
    }
}

