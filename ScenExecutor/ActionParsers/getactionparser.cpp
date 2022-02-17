#include "getactionparser.h"

#include "settings.h"

GetActionParser::GetActionParser()
{
    //qDebug() << "constr GetActionParser";
}

GetActionParser::~GetActionParser()
{
    //qDebug() << "destr GetActionParser";
}

bool GetActionParser::parseActionParameters(QDomElement parameters_element, DataPack **data_pack)
{
    QString source = parameters_element.attribute("source");
    QString target = parameters_element.attribute("target");

    Settings::replaceVariablesIn(source, m_variables);
    Settings::replaceVariablesIn(target, m_variables);

    *data_pack = formDataPack(source, target);
    return true;
}

DataPack* GetActionParser::formDataPack(QString source, QString target)
{
    DataPack *command_data_pack = new DataPack();
    command_data_pack->type = 11;
    command_data_pack->source = source;
    command_data_pack->target = target;
    return command_data_pack;
}
