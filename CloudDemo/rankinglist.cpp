#include "rankinglist.h"
#include "ui_rankinglist.h"

RankingList::RankingList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RankingList)
{
    ui->setupUi(this);
}

RankingList::~RankingList()
{
    delete ui;
}
