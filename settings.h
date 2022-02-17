#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QMap>
#include <QHash>
#include <QDateTime>

#include "settingsparsers.h"
#include "directorymanager.h"
#include "timer.h"
#include "waitlist.h"
#include "ScenExecutor/scenexecutor.h"

class SettingsParsers;



//Настройки обработки поступлений всех файлов в фильтр
struct FilterSettings
{
    //      путь к "мусорке" фильтра
    QString junk_path;

       /*\\имя файла\\соответствующие имени настройки обработки\\*/
    QMap<QString,   QList<EventExecutionParameters>> files_execution_parameters;
};

//настройки обработки события завершения списка отслеживаемых сценариев
struct WaitListClearEventActions
{
    QString wait_list_id;
    QString message;
    QList<EventExecutionParameters*> events;
};



enum VariableType
{
    string,
    number,
    undefined
};



//структура хранения пользовательских переменных
struct Variable
{
    QString name;
    QString value;
    VariableType type;
};



// структуры правил извлечения данных из плана
struct PlanParseRules
{
    //название главного тега плана
    QString plan_tag;
    //правила извлечения параметров рабочей программы
    QString work_program_tag;
    QString work_program_parameter_index;
    QString work_program_parameter_begin_time;
    QString work_program_parameter_end_time;


    //правила извлечения параметров съёма
    QString drop_tag;
    QString drop_parameter_drop_index;
    QString drop_parameter_circle_index;
    QString drop_parameter_begin_time;
    QString drop_parameter_end_time;
};






class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* settings();
    ~Settings();

    //рекурсивная замена переменных в строках на их значения
    bool replaceVariablesIn(QString &attribute);
    static bool replaceVariablesIn(QString &attribute, QList<Variable*> variables);


    bool setScenExecutor(ScenExecutor *scen_executor); 

    void setPortForClients(int port_for_clients);
    int  portForClients() const;


    void setPortForStateManager(int port_for_state_manager);
    int portForStateManager() const;


    //-----------------------------------------------------------
    //Работа с переменными
    Variable* searchVar(QString var_name);
    static Variable* searchVar(QString var_name, QList<Variable*> variables);
    bool setValueToVar(QString var_name, QString value);
    static bool setValueToVar(QString var_name, QString value, QList<Variable*> variables);
    bool createVar(QString var_name, VariableType type, QString value = "");
    QList<Variable*> getCurrentVariables() const;


    void setVarNameForWorkProgramCurrentIndex(QString var_name);
    QString getVarNameForWorkProgramCurrentIndex() const;

    void setVarNameForSeanceCurrentIndex(QString var_name);
    QString getVarNameForSeanceCurrentIndex() const;

    void setVarNameForRotateCurrentIndex(QString var_name);
    QString getVarNameForRotateCurrentIndex() const;

    void setVarNameForMessageFilePath(QString var_name);
    QString getVarNameForMessageFilePath() const;

    void setVarNameForMessageFileName(QString var_name);
    QString getVarNameForMessageFileName() const;

    void setVarNameForReturnCode(QString var_name);
    QString getVarNameForReturnCode() const;

    void setVarNameForPackerPrekeys(QString var_name);
    QString getVarNameForPackerPrekeys() const;

    void setVarNameForPackerAfterkeys(QString var_name);
    QString getVarNameForPackerAfterkeys() const;

    //-----------------------------------------------------------
    //Правила парсинга файла плана
    PlanParseRules *planParseRules() const;

    void setPlanParseRules(PlanParseRules *plan_parse_rules);


    //-----------------------------------------------------------
    // Методы доступа к настройкам ахиватора

    void setPackCommand(QString command);

    QString packCommand() const;

    void setUnpackCommand(QString command);

    QString unpackCommand() const;

    //-----------------------------------------------------------
    // Метод доступа к таймеру плана
    Timer *timer() const;
    //-----------------------------------------------------------
    // Методы доступа к листам ожидания
    void insertScenInWaitList(QString scen_name, QString list_id);
    void removeScenFromWaitList(QString scen_name, QString list_id);
    void setWaitListClearEventActions(WaitListClearEventActions *actions);
    //-----------------------------------------------------------
    // Методы доступа к рабочим ролям и группам

    QMap<QString, QString> defaultRolesMap() const;
    void setDefaultRoleForGroup(QString group, QString default_role);

    QMap<QString, QMap<QString, QStringList>> groupsRolesMap() const;
    void setRolesForGroup(QString group, QMap<QString, QStringList> roles);

    QString defaultExecutorRole() const;
    void setDafaultExecutorRole(QString role);

    //----------------------------------------------------------


    DirectoryManager *directoryManager() const;


    EventExecutor *clientsFileExecutor() const;

    void executeFileFromClient(QString file_name, QTcpSocket *socket);


public slots:
    //-----------------------------------------------------------
    // Слот обработки завершения листа ожидания
    void executeEmptyWaitListEvent(QString id);
     //-----------------------------------------------------------

private:
    explicit Settings();
    //-----------------------------------------------------------
    // Метод создания нового листа ожидания
    void createNewWaitList(QString scen_name, QString list_id);
    //-----------------------------------------------------------



    static Settings *obj_ptr;

    int                                     m_port_for_clients;

    int                                     m_port_for_state_manager;


    //управляющий отслеживаемыми каталогами
    DirectoryManager *directory_manager;
    //обработчик сообщений от клиентов о поступлении файлов в их отслеживаемые папки
    EventExecutor *clients_file_executor;
    //обработчик событий завершения списков ожидания
    EventExecutor *wait_list_clear_events_executor;


    //настройки рабочих ролей и групп
    QMap<QString, QString>          default_roles;

    QMap<QString,
       QMap<QString, QStringList> > group_roles;

    QString                         default_executor;


    //-----------------------------------------------------------
    // Глобальный объект ослеживания времени плана
    Timer *plan_timer;

    //-----------------------------------------------------------
    // Правила разбора файла плана
    PlanParseRules *m_plan_parse_rules;

    //-----------------------------------------------------------
    // Переменные прорграммы
    QList<Variable*> vars;

    //-----------------------------------------------------------
    // имена переменных, используемых для обновления
    // текущих рабочих параметров
    QString var_name_work_program_current_index;

    QString var_name_seance_current_index;

    QString var_name_rotate_index;

    QString var_name_message_file_path;

    QString var_name_message_file_name;

    QString var_name_return_code;

    QString var_name_packer_afterkeys;

    QString var_name_packer_prekeys;

    //-----------------------------------------------------------
    // Настройки ахиватора
    QString m_pack_command;

    QString m_unpack_command;
    //-----------------------------------------------------------
    // Списки ожидания
    QList<WaitList*>  wait_lists;
    QList<WaitListClearEventActions*> wait_lists_clear_event_actions;

    //-----------------------------------------------------------


signals:
    void newText(QString error_text);
};


#endif // SETTINGS_H
