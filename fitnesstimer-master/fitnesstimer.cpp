#include <QSoundEffect>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>
#include "fitnesstimer.h"
#include "ui_fitnesstimer.h"

FitnessTimer::FitnessTimer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FitnessTimer),
    stage(Break),
    keepCount(0)
{
    ui->setupUi(this);

    ui->stopButton->setVisible(false);

    connect(ui->startButton, SIGNAL(released()), this, SLOT(start()));
    connect(ui->stopButton, SIGNAL(released()), this, SLOT(stop()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(ticktack()));

    effect = new QSoundEffect(this);
    effect->setVolume(0.5f);
}

FitnessTimer::~FitnessTimer()
{
    delete ui;
}

void FitnessTimer::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        if (timer->isActive())
            timekeeping();
        event->accept();
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
        break;
    }
}

void FitnessTimer::start()
{
    ui->startButton->setVisible(false);
    ui->stopButton->setVisible(true);
    runTime.start();
    keepTime.start();
    timer->start(1000);
    stage = Exercise;
    effect->setSource(QUrl::fromLocalFile(":/exercise.wav"));
    effect->setLoopCount(1);
    effect->play();
}

void FitnessTimer::stop()
{
    ui->startButton->setVisible(true);
    ui->stopButton->setVisible(false);
    timer->stop();
}

void FitnessTimer::timekeeping()
{
    keepTime.restart();
    if (stage == Break) {
        stage = Exercise;
        effect->setSource(QUrl::fromLocalFile(":/exercise.wav"));
        effect->setLoopCount(1);
        effect->play();
    } else {
        stage = Break;
        keepCount = 0;
        effect->setSource(QUrl::fromLocalFile(":/break.wav"));
        effect->setLoopCount(keepCount);
        effect->play();
    }
}

void FitnessTimer::ticktack()
{
    QTime time(0, 0, 0);
    const QTime &time1 = time.addMSecs(runTime.elapsed());
    const QTime &time2 = time.addMSecs(keepTime.elapsed());

    ui->hourLabel->setText(QString("%1").arg(time1.hour(), 2, 10, QLatin1Char('0')));
    ui->minuteLabel->setText(QString("%1").arg(time1.minute(), 2, 10, QLatin1Char('0')));
    ui->secondLabel->setText(QString("%1").arg(time1.second(), 2, 10, QLatin1Char('0')));

    ui->minuteKeepLabel->setText(QString("%1").arg(time2.minute(), 2, 10, QLatin1Char('0')));
    ui->secondKeepLabel->setText(QString("%1").arg(time2.second(), 2, 10, QLatin1Char('0')));

    if (stage == Break) {
        int elapsedCount = keepTime.elapsed() / (1000 * 30);
        if (elapsedCount > keepCount) {
            keepCount = elapsedCount;
            effect->setLoopCount(keepCount);
            effect->play();
        }
    }
}
