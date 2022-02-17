#include "deleteactionparser.h"

#include "settings.h"

DeleteActionParser::DeleteActionParser()
{
    //qDebug() << "constr DeleteActionParser";
}

DeleteActionParser::~DeleteActionParser()
{
    //qDebug() << "destr DeleteActionParser";
}

bool DeleteActionParser::parseActionParameters(QDomElement parameters_element, DataPack **data_pack)
{
    QString target = parameters_element.attribute("target");
    Settings::replaceVariablesIn(target, m_variables);

    *data_pack = formDataPack(target);
    return true;
}

DataPack* DeleteActionParser::formDataPack(QString target)
{
    DataPack *command_data_pack = new DataPack();
    command_data_pack->type = 16;
    command_data_pack->target = target;
    return command_data_pack;
}
