#include "sendactionparser.h"

#include "settings.h"

SendActionParser::SendActionParser()
{
    //qDebug() << "constr SendActionParser";
}

SendActionParser::~SendActionParser()
{
    //qDebug() << "destr SendActionParser";
}

bool SendActionParser::parseActionParameters(QDomElement parameters_element, DataPack **data_pack)
{
    QString source = parameters_element.attribute("source");
    QString target = parameters_element.attribute("target");
    QString notify = parameters_element.attribute("notify");

    Settings::replaceVariablesIn(source, m_variables);
    Settings::replaceVariablesIn(target, m_variables);
    Settings::replaceVariablesIn(notify, m_variables);

    *data_pack = formDataPack(source, target, notify);

    return true;
}

DataPack* SendActionParser::formDataPack(QString source, QString target, QString notify)
{
    DataPack *command_data_pack = new DataPack();
    command_data_pack->type = 13;
    command_data_pack->source = source;
    command_data_pack->target = target;
    command_data_pack->notify = notify;
    return command_data_pack;
}
