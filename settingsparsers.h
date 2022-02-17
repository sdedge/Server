#ifndef SETTINGSPARSERS_H
#define SETTINGSPARSERS_H

#include <QObject>
#include "settings.h"
#include <QDomDocument>

class Settings;


//Настройки для формирования упаковки и справки
struct SettingsForPackFile
{
    QRegExp file_name_reg_exp;
    QString file_skip;
    QString file_query_number;
    QString file_vars;
};

struct PackSettings
{
    QString search_dir_path;
    QString pack_creation_dir_path;
    QString search_algoritm_type;
    QString pack_data_file_name;
    QString manual_file_name;
    QString pack_data_manual_file_name;
    QString filter_path;
    QList<SettingsForPackFile*> settings_for_pack_files;
};

struct EventExecutionParameters;

class SettingsParsers : public QObject
{
    Q_OBJECT
public:
    SettingsParsers();
    ~SettingsParsers();

    //1. Путь к отслеживаемым каталогам и соответствующим настройкам №2 (настройка № 1)
    static void parseSettings1(QString settings_1_path);

    //2. Действия при поступлении файла в один из отслеживаемых каталогов на серевере (настройка № 2)
    static void parseSettings2(QString settings_2_path, QString filter_dir_path);
    static QString formParametersString(int action_type, QDomNode *parameters_node);

    //3. Структура для разбора плана (настройка № 3)
    static void parseSettings3(QString settings_3_path);

    //4. Действия при поступлении файла в один из отслеживаемых каталогов на клиенте(настройка № 4)
    static void parseSettings4(QString settings_4_path);
    //5. Роли и группы с указанием выполняемых сценариев(настройка № 5)
    static void parseSettings5(QString settings_5_path);


    /*
    * Эта настройка парсится первой и использется в ходе парсинга остальных ностроек
    *
    * 6. Переменные пользователя, глобальные пути программы (настройка 6)*/
    static void parseSettings6(QString settings_6_path);

    //7. Содержимое справки и упаковки (настройка № 7)
    static PackSettings* parseSettings7(QString settings_7_path);

    //8. Отображение плана (настройка № 8)------
    static void parseSettings8(QString settings_8_path, Settings *settings);

    //9. Действия по окончанию процесса обработки (настройка 9)
    static QList<EventExecutionParameters *> parseSettings9(QString settings_9_path);

    //10. Порты серверов (настройка 10)
    static void parseSettings10(QString settings_10_path);


    static bool createDirIfDoesNotExist(QString dir_path);

private:


};

#endif // SETTINGSPARSERS_H
