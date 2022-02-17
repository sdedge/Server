#include "copyactionparser.h"

#include "settings.h"

CopyActionParser::CopyActionParser()
{
    //qDebug() << "constr CopyActionParser";
}

CopyActionParser::~CopyActionParser()
{
    //qDebug() << "destr CopyActionParser";
}

bool CopyActionParser::parseActionParameters(QDomElement parameters_element, DataPack **data_pack)
{
    QString source = parameters_element.attribute("source");
    QString target = parameters_element.attribute("target");
    Settings::replaceVariablesIn(source, m_variables);
    Settings::replaceVariablesIn(target, m_variables);

    *data_pack = formDataPack(source, target);
    return true;
}

DataPack* CopyActionParser::formDataPack(QString source, QString target)
{
    DataPack *command_data_pack = new DataPack();
    command_data_pack->type = 14;
    command_data_pack->source = source;
    command_data_pack->target = target;
    return command_data_pack;
}
