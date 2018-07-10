#include "moviemod.h"
#include <QLayout>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include "makeshell.h"
Moviemod::Moviemod(QWidget *parent) : QWidget(parent)
{
    websitelabel=new QLabel(tr("网址"));
    websitetext=new QLineEdit("rtmp://js.live-send.acg.tv/live-js/");
    streamlabel=new QLabel(tr("推流码"));
    streamtext=new QLineEdit("?streamname=live_956714_5294207&key=a47f28b34b8809026e8e93e75adca602");
    okbtn=new QPushButton;
    okbtn->setText(tr("开始直播"));
    closebtn=new QPushButton;
    closebtn->setText(tr("停止直播"));
    choosemoive=new QPushButton;
    choosemoive->setText(tr("选择电影"));
    moviename=new QLineEdit;
    choosesrt=new QPushButton;
    choosesrt->setText(tr("选择字幕"));
    srt_name=new QLineEdit;


    QGridLayout* mainlayout=new QGridLayout(this);
    QHBoxLayout* btnlayout=new QHBoxLayout;
    btnlayout->addStretch(1);
    btnlayout->addWidget(okbtn);
    btnlayout->addWidget(closebtn);
    mainlayout->setMargin(10);
    mainlayout->setSpacing(6);
    mainlayout->addWidget(websitelabel,0,0);
    mainlayout->addWidget(websitetext,0,1);
    mainlayout->addWidget(streamlabel,1,0);
    mainlayout->addWidget(streamtext,1,1);
    mainlayout->addWidget(choosemoive,2,0);
    mainlayout->addWidget(moviename,2,1);
    mainlayout->addWidget(choosesrt,3,0);
    mainlayout->addWidget(srt_name,3,1);
    mainlayout->addLayout(btnlayout,4,0,1,2);
    process_0 = new QProcess(this);
    connect(okbtn,SIGNAL(clicked()),this,SLOT(mkandexcute_shell()));
    connect(choosemoive,SIGNAL(clicked()),this,SLOT(showfile()));
    connect(closebtn,SIGNAL(clicked()),process_0,SLOT(kill()));
    connect(choosesrt,SIGNAL(clicked(bool)),this,SLOT(showsrt()));
    connect(fswatcher,SIGNAL(directoryChanged(QString)),this,SLOT(setsrt(QString)));

}

void Moviemod::showfile()
{
    QString path_of_video =QFileDialog::getOpenFileName(this,tr("选择电影文件"),"/");
    moviename->setText(path_of_video);
}


void Moviemod::showsrt()
{
    QString path_of_video =QFileDialog::getOpenFileName(this,tr("选择srt字幕文件"),moviename->text());
    srt_name->setText(path_of_video);
}

void Moviemod::mkandexcute_shell()
{
    QString rtmp = websitetext->text()+streamtext->text();
    creat_srt_shell(moviename->text(),rtmp,srt_name->text());
#if defined(Q_OS_WIN32)
    QString pathofshell = QDir::currentPath() + "/raspliveshell.bat" ;
    qDebug() << pathofshell;
    process_0->start("cmd.exe",QStringList() << "/c" << pathofshell);
  /*  process_0->start("cmd.exe",QStringList() << "/c" << "ping 123.206.209.50");
    qint64 maxSize = 512;
    char buffer[maxSize];
    qint64 len;
    while(true)
    {
        len = process_0->readLine(buffer, maxSize);
        qDebug()<<"buffer len"<<len;

        if(len <= 0) {
            break;
        }
        QString str = QString::fromLocal8Bit(buffer);
        qDebug()<<"qstring len"<<str.length();
        qDebug()<<str;
        qDebug()<<"";
    }  */
#elif defined(Q_OS_LINUX)
    QString pathofshell = "sh "+QDir::currentPath() + "/raspliveshell.sh" ;
    qDebug() << pathofshell;
    process_0->start(pathofshell);
#endif


}

void Moviemod::downsrt()
{
    if(choosemoive->text().isEmpty())
        return;

    QString path_movie=choosemoive->text();
    QFileInfo mvinfo(path_movie);
    fswatcher->addPath(mvinfo.absolutePath());
    process_0->start("python3 getsrt.py"+path_movie);

}

void Moviemod::setsrt(QString path)
{

    QDir mvdir(path);
    QStringList str_list;
    str_list<<"*";
    QFileInfoList filelist=mvdir.entryInfoList(str_list,QDir::Files,QDir::Name);
    foreach(QFileInfo finfo, filelist)
    {
        if(finfo.suffix()=="srt")
        {
            QString shell="ffmpeg -i "+finfo.fileName()+" newname.ass";
            process_0->start(shell);
        }
        if(finfo.suffix()=="ass")
        {
            choosesrt->setText(finfo.absoluteFilePath());
            break;
        }

    }

}


Moviemod::~Moviemod()
{

}
