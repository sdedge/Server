#include "clientwidget.h"
#include <QDebug>
#include <QPainter>

ClientWidget::ClientWidget(QString objectName, QString role, quint16 number, QWidget *parent) : QWidget(parent)
{
    this->setMinimumSize(128,128);
    this->setMaximumSize(128,128);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setGeometry(0,0,128,128);

    this->setObjectName(objectName);


    this->setStyleSheet("QWidget#" + this->objectName() + " {"
                        "border: solid;"
                        "border-style: outset;"
                        "border-width: 2px;"
                        "border-radius: 10px;"
                        "border-color: grey;"
                        "}");

    this->setMouseTracking(true);

    nameLabel = new QLabel(this);
    nameLabel->setGeometry(45,22,80,17);
    nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    roleLabel = new QLabel(this);
    roleLabel->setGeometry(45,4,80,17);
    roleLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    roleLabel->setText(role);

    img = new QLabel(this);
    img->setGeometry(0,12,128,128);
    img->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);


    QLabel *numberLabel = new QLabel(this);
    numberLabel->setGeometry(2,2,40,40);
    numberLabel->setText(QString::number(number));
    numberLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);


    progressBar = new QProgressBar(this);
    //progressBar->setOrientation(Qt::Vertical);
    progressBar->setGeometry(4, 108, 120, 14);
    progressBar->setVisible(false);
    progressBar->setTextVisible(false);
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
}




void ClientWidget::addLine(QString line)
{

    log.appendPlainText(line);
    emit sendLog(line);
}

QString ClientWidget::getRole() const
{
    return roleLabel->text();
}

void ClientWidget::setRole(QString value)
{
    roleLabel->setText(value);
}

QString ClientWidget::getName() const
{
    QRegExp exp("[()]");
    return nameLabel->text().remove(exp);
}

void ClientWidget::setName(QString value)
{
    nameLabel->setText("(" + value + ")");
}

QString ClientWidget::getLog() const
{
    return log.toPlainText();
}

void ClientWidget::setLog(const QString &value)
{
    log.setPlainText(value);
}

QColor ClientWidget::getColor() const
{
    return color;
}

void ClientWidget::setColor(WidgetColor w_color)
{
    QPixmap pixmap;
    switch (w_color) {
    case Red:
        img->clear();
        pixmap.load(":/img/cancel.png");
        color = Qt::red;
        img->setPixmap(pixmap);

        this->setStyleSheet("QWidget#" + this->objectName() +
            "{"
                "border: solid;"
                "border-style: outset;"
                "border-width: 2px;"
                "border-radius: 10px;"
                "border-color: red;"
                "background-color: qlineargradient("
                                                    "spread:reflect, x1:0, y1:0, x2:0.982682, y2:1, "
                                                    "stop:0.244318 rgba(189, 189, 189, 34), "
                                                    "stop:1 rgba(255, 0, 0, 126)"
                                                 ");"
            "}");
        break;
    case Yellow:
        img->clear();
        color.setRgb(255,164,32);
        pixmap.load(":/img/exclamation.png");
        img->setPixmap(pixmap);

        this->setStyleSheet("QWidget#" + this->objectName() +
            "{"
                "border: solid;"
                "border-style: outset;"
                "border-width: 2px;"
                "border-radius: 10px;"
                "border-color: orange;"
                "background-color: qlineargradient("
                                                    "spread:reflect, x1:0, y1:0, x2:0.982682, y2:1,"
                                                    "stop:0.244318 rgba(252, 252, 252, 34),"
                                                    "stop:1 rgba(255, 140, 0, 126)"
                                                 ");"
            "}");
        break;
    case Green:
        img->clear();
        color = Qt::green;
        pixmap.load(":/img/ok.png");
        img->setPixmap(pixmap);

        this->setStyleSheet("QWidget#" + this->objectName() +
            "{"
                "border: solid;"
                "border-style: outset;"
                "border-width: 2px;"
                "border-radius: 10px;"
                "border-color: green;"
                "background-color: qlineargradient("
                                                    "spread:reflect, x1:0, y1:0, x2:0.982682, y2:1, "
                                                    "stop:0.244318 rgba(252, 252, 252, 34), "
                                                    "stop:1 rgba(0, 255, 7, 126)"
                                                 ");"
            "}");
        break;
    default:
        break;
    }
}

bool ClientWidget::getStatus() const
{
    return status;
}

void ClientWidget::setStatus(bool value)
{
    status = value;
}

bool ClientWidget::getStatusDisp() const
{
    return statusDisp;
}

void ClientWidget::setStatusDisp(bool value)
{
    statusDisp = value;
}

bool ClientWidget::getStatusClient() const
{
    return statusClient;
}

void ClientWidget::setStatusClient(bool value)
{
    statusClient = value;
}


void ClientWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QLine hline;
    hline.setLine(2,40,125,40);
    QLine vline;
    vline.setLine(40,2,40,40);

    p.setPen(color);

    p.drawLine(hline);
    p.drawLine(vline);

    QWidget::paintEvent(event);
}
