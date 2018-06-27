#include "bitcannanodeconfigdialog.h"
#include "ui_bitcannanodeconfigdialog.h"

#include <QModelIndex>

BitCannaNodeConfigDialog::BitCannaNodeConfigDialog(QWidget *parent, QString nodeAddress, QString privkey) :
    QDialog(parent),
    ui(new Ui::BitCannaNodeConfigDialog)
{
    ui->setupUi(this);
    QString desc = "rpcallowip=127.0.0.1<br>rpcuser=REPLACEME<br>rpcpassword=REPLACEME<br>staking=0<br>server=1<br>listen=1<br>port=REPLACEMEWITHYOURPORT<br>masternode=1<br>masternodeaddr=" + nodeAddress + "<br>masternodeprivkey=" + privkey + "<br>";
    ui->detailText->setHtml(desc);
}

BitCannaNodeConfigDialog::~BitCannaNodeConfigDialog()
{
    delete ui;
}
