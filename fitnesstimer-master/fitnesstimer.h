#ifndef FITNESSTIMER_H
#define FITNESSTIMER_H

#include <QMainWindow>
#include <QTime>

namespace Ui {
class FitnessTimer;
}

class QSoundEffect;
class QTimer;

class FitnessTimer : public QMainWindow
{
    Q_OBJECT

public:
    explicit FitnessTimer(QWidget *parent = 0);
    ~FitnessTimer();

protected:
    virtual void keyReleaseEvent(QKeyEvent *event);

private slots:
    void start();
    void stop();
    void timekeeping();
    void ticktack();

private:
    enum Stage {
        Break,
        Exercise,
    };

private:
    Ui::FitnessTimer *ui;
    Stage stage;
    QSoundEffect *effect;
    QTimer *timer;
    QTime runTime;
    QTime keepTime;
    int keepCount;
};

#endif // FITNESSTIMER_H
