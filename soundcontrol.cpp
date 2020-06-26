//+
#include "soundcontrol.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDir>

SoundControl::SoundControl(QObject *parent)
    : QObject(parent)
    , m_backgroundMusic(NULL)
{
    // 创建一直播放的背景音乐
    QUrl BGMUrl = QUrl("qrc:/sound/BGM.mp3");

    m_backgroundMusic = new QMediaPlayer(this);
    QMediaPlaylist *BGMList = new QMediaPlaylist();

    QMediaContent media(BGMUrl);
    BGMList->addMedia(media);
    BGMList->setCurrentIndex(0);

    // 设置背景音乐循环播放
    BGMList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    m_backgroundMusic->setPlaylist(BGMList);
}

void SoundControl::startBGM()
{
    if (m_backgroundMusic) m_backgroundMusic->play();
}

void SoundControl::playSound(SoundType soundType)
{
    static const QUrl mediasUrls[] =
    {
        QUrl("qrc:/sound/PlantTower.mp3"),
        QUrl("qrc:/sound/SellTower.mp3"),
        QUrl("qrc:/sound/UpgradeTower.mp3"),
        QUrl("qrc:/sound/SelectTower.mp3"),
        QUrl("qrc:/sound/LifeLoss.mp3"),
        QUrl("qrc:/sound/BulletShot.mp3"),
        QUrl("qrc:/sound/EnemyKilled.mp3"),
        QUrl("qrc:/sound/Finalwave.mp3"),
        QUrl("qrc:/sound/SelectButton.mp3")
    };

    static QMediaPlayer player;

    player.setMedia(mediasUrls[soundType]);
    player.play();
}

