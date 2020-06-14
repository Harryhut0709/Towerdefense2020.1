#ifndef SOUNDCONTROL_H
#define SOUNDCONTROL_H

#include <QObject>
#include <QMediaPlayer>

enum SoundType//ö�����͵�����
{
    PlantTower,               //����
    SellTower,                //����
    UpgradeTower,             //������
    SelectTower,              //ѡ��ĳһ����
    LifeLoss,                 //���ﵽ���أ���Ѫ
    Bulletshot,               //�ӵ�����
    EnemyKilled               //�������
};

class SoundControl: public QObject
{
public:
    explicit SoundControl(QObject *parent = 0);

    void startBGM();
    void playSound(SoundType soundType);

private:
    QMediaPlayer *m_backgroundMusic; // ֻ�������ű�������
};

#endif // SOUNDCONTROL_H
