#include "timer.h"

#include <QDebug>
#include <QTimer>

Timer::Timer(QObject *parent) : QObject(parent)
{
    next_work_program = nullptr;
    next_drop = nullptr;
    program_active = false;
    drop_status = NotPrepered;
}


Timer::~Timer()
{
    delete next_work_program;
    delete next_drop;
}






//==============================================================================================================================================================
//процедура основной работы потка
//==============================================================================================================================================================
void Timer::run()
{
    while(1)
    {

        if(next_work_program != nullptr)
        {
            if(program_active)
            {
                waitForWorkProgramEnds();
                switch(drop_status)
                {
                    case InProccess:
                    {
                        waitForDropEnds();
                    }
                    case Prepered:
                    {
                        waitForDropBegins();
                    }
                    case NotPrepered:
                    {
                        waitForDrop();
                    }
                }
            }
            else
                waitForWorkProgramBegins();
        }




        QEventLoop loop;                                                                //
        QTimer timer;                                                                   //
        timer.setInterval(100); // 0.1 sec                                              //   небольшая задержка, чтобы не грузить ЦП
        connect (&timer, &QTimer::timeout, &loop, &QEventLoop::quit);                   //
        timer.start();                                                                  //
        loop.exec();                                                                    //
    }
}


//==============================================================================================================================================================
//процедура добавления рабочей программы
//==============================================================================================================================================================
void Timer::insertNewWorkProgram(WorkProgram *new_program)
{

    if(new_program->begin_time < QDateTime::currentDateTime())
    {
        qDebug() << "Рабочая программа на время " + new_program->begin_time.toString() + " устарела.";
        return;
    }


    if(work_plan.size() != 0)
    {
        bool inserted = false;

        for(int i = 0; i < work_plan.size(); ++i)
        {
            if(work_plan[i]->begin_time > new_program->begin_time)
            {
                work_plan.insert(i, new_program);
                inserted = true;
            }
            //если добавляемая программа пересекается с уже запланированной, то эта программа и все, что позже, перезаписываются
            else if((work_plan[i]->begin_time < new_program->begin_time) && (work_plan[i]->end_time > new_program->begin_time))
            {
                while(work_plan.size() != i)
                    work_plan.removeLast();
                work_plan.insert(i, new_program);
                inserted = true;
            }
        }

        if(!inserted)
            work_plan.append(new_program);
    }
    else
        work_plan.append(new_program);


    //план был пуст или текущая рабочая программа была перезаписана
    if(next_work_program == nullptr)
    {
        next_work_program = work_plan.first();
        next_drop = next_work_program->drops.first();
    }
}


WorkProgram *Timer::workProgram() const
{
    return next_work_program;
}


Drop *Timer::drop() const
{
    return next_drop;
}


void Timer::deleteCurrentAndSelectNextWorkProgram()
{
    work_plan.removeFirst();
    next_work_program = work_plan.first();
}


void Timer::deleteCurrentAndSelectNextDrop()
{
    next_work_program->drops.removeFirst();
    next_drop = next_work_program->drops.first();
}


void Timer::waitForWorkProgramBegins()
{
    if(next_work_program->begin_time > QDateTime::currentDateTime())
    {
        program_active = true;
        emit timeToUpdateVariables();
    }
}

void Timer::waitForWorkProgramEnds()
{
    if(next_work_program->end_time < QDateTime::currentDateTime())
    {
        program_active = false;
        deleteCurrentAndSelectNextWorkProgram();
        emit timeToCleanVariables();
    }
}

void Timer::waitForDropBegins()
{
    if(next_drop->begin_time > QDateTime::currentDateTime())
    {
        drop_status = InProccess;
        emit begin_drop_execution();
    }
}

void Timer::waitForDropEnds()
{
    if(next_drop->end_time.secsTo(QDateTime::currentDateTime()) > 1800)     //более 30 минут прошло с окончания сеанса
    {
        drop_status = NotPrepered;
        deleteCurrentAndSelectNextDrop();
        emit timeToCleanDropVariables();
    }
}

void Timer::waitForDrop()
{
    if(QDateTime::currentDateTime().secsTo(next_drop->begin_time) < 1800)   //менее 30 минут до начала сеанса
    {
        drop_status = Prepered;
        emit timeToUpdateDropVariables();
    }
}
