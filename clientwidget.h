#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QEvent>
#include <QColor>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QProgressBar>

enum WidgetColor
{
    Red,
    Yellow,
    Green
};


class ClientWidget : public QWidget
{
    Q_OBJECT
public:
    explicit    ClientWidget(QString objectName, QString role, quint16 number, QWidget *parent = 0);
    void        addLine     (QString line);

    QString     getRole             () const;
    void        setRole             (QString value);

    QString     getName             () const;
    void        setName             (QString value);

    QString     getLog              () const;
    void        setLog              (const QString &value);

    QColor      getColor            () const;
    void        setColor            (WidgetColor w_color);

    bool        getStatus           () const;
    void        setStatus           (bool value);

    bool        getStatusDisp       () const;
    void        setStatusDisp       (bool value);

    bool        getStatusClient     () const;
    void        setStatusClient     (bool value);

signals:
    void        sendLog             (QString);
    void        showDialog          ();
public slots:

private:
    QLabel             *img;
    QLabel             *nameLabel;
    QLabel             *roleLabel;
    QPlainTextEdit     log;
    QProgressBar       *progressBar;
    QPushButton        *execAnyScen;
    QPushButton        *execAvailableScens;

    bool               leftButtonPressed = false;

    QStringList        availableScens;
    QColor             color;
    bool               status            = false;
    bool               statusDisp        = false;
    bool               statusClient      = false;



protected:
    void               paintEvent  (QPaintEvent *event);
};

#endif // CLIENTWIDGET_H
