#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QString>
#include <QApplication>
#include <QDir>
#include <QTime>
#include <QDate>


//структуры хранения информации из плана
//структура данных съёма
struct Drop
{
    QDateTime begin_time;
    QDateTime end_time;
    int drop_index = 0;
    int circle_index = 0;
};


//структура данных рабочей программы
struct WorkProgram
{
    QDateTime begin_time;
    QDateTime end_time;
    int index = 0;
    QList<Drop*> drops;
};


enum DropStatus
{
    InProccess,
    Prepered,
    NotPrepered
};


class Timer : public QObject
{
    Q_OBJECT
public:
    explicit Timer(QObject *parent = nullptr);
    ~Timer();


public slots:
    void            run                     ();

    void            insertNewWorkProgram    (WorkProgram *new_program);

    WorkProgram*    workProgram      () const;
    Drop*           drop             () const;


private:

    QList<WorkProgram*> work_plan;

    WorkProgram*        next_work_program;
    Drop*               next_drop;

    bool                program_active;
    DropStatus          drop_status;

    void                deleteCurrentAndSelectNextWorkProgram();
    void                deleteCurrentAndSelectNextDrop();

    void                waitForWorkProgramBegins();
    void                waitForWorkProgramEnds();

    void                waitForDropBegins();
    void                waitForDropEnds();
    void                waitForDrop();



signals:
    void            finished                        ();
    void            timeToUpdateVariables           ();
    void            timeToUpdateDropVariables       ();

    void            timeToCleanVariables            ();
    void            timeToCleanDropVariables        ();

    void            begin_drop_execution            ();




};

#endif // TIMER_H
