#ifndef SOUNDCONTROL_H
#define SOUNDCONTROL_H

#include <QObject>
#include <QMediaPlayer>

enum SoundType//枚举类型的数组
{
    PlantTower,               //建塔
    SellTower,                //拆塔
    UpgradeTower,             //升级塔
    SelectTower,              //选中某一种塔
    LifeLoss,                 //怪物到基地，掉血
    Bulletshot,               //子弹发射
    EnemyKilled               //怪物击毙
};

class SoundControl: public QObject
{
public:
    explicit SoundControl(QObject *parent = 0);

    void startBGM();
    void playSound(SoundType soundType);

private:
    QMediaPlayer *m_backgroundMusic; // 只用来播放背景音乐
};

#endif // SOUNDCONTROL_H
