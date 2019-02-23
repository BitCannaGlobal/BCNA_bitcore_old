#include <boost/algorithm/string/replace.hpp>

#include "masternodemanager.h"
#include "ui_masternodemanager.h"
#include "addeditbitcannanode.h"
#include "bitcannanodeconfigdialog.h"

#include "editmasternodedialog.h"

#include "sync.h"
#include "clientmodel.h"
#include "walletmodel.h"
#include "activemasternode.h"
#include "masternodeconfig.h"
#include "masternode.h"
#include "walletdb.h"
#include "wallet.h"
#include "init.h"
#include "stake.h"
#include "rpcserver.h"
#include "bitcoinunits.h"
#include <boost/lexical_cast.hpp>
#include <fstream>

#include "rpcwallet.cpp"

using namespace json_spirit;
using namespace std;

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QScrollArea>
#include <QScroller>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QScrollBar>
#include <QGraphicsDropShadowEffect>

#define LABEL_WIDTH 500
#define LEFT_MARGIN 10
#define TOP_MARGIN  30

#include "moc_basetabledelegate.cpp"

MasternodeManager::MasternodeManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MasternodeManager),
    clientModel(0),
    walletModel(0),
    mnNetLabel(new QLabel(tr("Masternode Network"), this)),
    mnMyMasternodesLabel(new QLabel(tr("My Masternode"), this)),
    networkMasterNodesTableDelegate(new BaseTableDelegate(3)),
    myMasterNodesTableDelegate(new BaseTableDelegate(4, 665))
{
    ui->setupUi(this);

    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->copyAddressButton->setEnabled(false);

    subscribeToCoreSignals();

    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tableWidget_2->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    mnNetLabel->setObjectName("mnNetLabel");
    mnNetLabel->resize(LABEL_WIDTH, mnNetLabel->height());
    mnNetLabel->move(LEFT_MARGIN, TOP_MARGIN);

    mnMyMasternodesLabel->setObjectName("mnMyMasternodesLabel");
    mnMyMasternodesLabel->resize(LABEL_WIDTH, mnMyMasternodesLabel->height());
    mnMyMasternodesLabel->move(LEFT_MARGIN, TOP_MARGIN);
    mnMyMasternodesLabel->hide();

    ui->tableWidget->setColumnWidth(0, 100);
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setColumnWidth(2, 235);

    ui->tableWidget_2->setColumnWidth(0, 100);
    ui->tableWidget_2->setColumnWidth(1, 200);
    ui->tableWidget_2->setColumnWidth(2, 100);
    ui->tableWidget_2->setColumnWidth(3, 380);

    ui->tableWidget->verticalHeader()->setDefaultSectionSize(69);
    ui->tableWidget->setItemDelegate(networkMasterNodesTableDelegate);

    ui->tableWidget_2->verticalHeader()->setDefaultSectionSize(69);
    ui->tableWidget_2->setItemDelegate(myMasterNodesTableDelegate);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateNodeList()));
    if(!GetBoolArg("-reindexaddr", false))
        timer->start(1000);
        fFilterUpdated = true;
	nTimeFilterUpdated = GetTime();

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged()));

    updateNodeList();
}

MasternodeManager::~MasternodeManager()
{
    delete ui;
}

static void NotifyBitCannaNodeUpdated(MasternodeManager *page, CBitCannaNodeConfig nodeConfig)
{
    // alias, address, privkey, collateral address
    // String index = QString::fromStdString(nodeConfig.sIndex);
    QString alias = QString::fromStdString(nodeConfig.sAlias);
    QString addr = QString::fromStdString(nodeConfig.sAddress);
    QString privkey = QString::fromStdString(nodeConfig.sMasternodePrivKey);
    QString collateral = QString::fromStdString(nodeConfig.sCollateralAddress);
    
    QMetaObject::invokeMethod(page, "updateBitCannaNode", Qt::QueuedConnection,
                              // Q_ARG(QString, index),
                              Q_ARG(QString, alias),
                              Q_ARG(QString, addr),
                              Q_ARG(QString, privkey),
                              Q_ARG(QString, collateral)
                              );
}

void MasternodeManager::tabChanged() {
    if(ui->tabWidget->currentIndex() == 1) {
        mnMyMasternodesLabel->hide();
        mnNetLabel->show();
    } else {
        mnNetLabel->hide();
        mnMyMasternodesLabel->show();
    }
}

void MasternodeManager::subscribeToCoreSignals()
{
    // Connect signals to core
    uiInterface.NotifyBitCannaNodeChanged.connect(boost::bind(&NotifyBitCannaNodeUpdated, this, _1));
}

void MasternodeManager::unsubscribeFromCoreSignals()
{
    // Disconnect signals from core
    uiInterface.NotifyBitCannaNodeChanged.disconnect(boost::bind(&NotifyBitCannaNodeUpdated, this, _1));
}

void MasternodeManager::on_tableWidget_2_itemSelectionChanged()
{
    if(ui->tableWidget_2->selectedItems().count() > 0)
    {
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        ui->copyAddressButton->setEnabled(true);
        ui->actionsBox->setEnabled(true);
    }
}

bool setMasterNodeForIX(CBitCannaNodeConfig c, std::string& errorMessage) {
    if (!fMasterNode) {
        fMasterNode = true;
        LogPrintf("IS DARKSEND MASTER NODE\n");

        strMasterNodeAddr = c.sAddress;
        if (!strMasterNodeAddr.empty()) {
            CService addrTest = CService(strMasterNodeAddr);
            if (!addrTest.IsValid()) {
                errorMessage = "Invalid -masternodeaddr address: " + strMasterNodeAddr;
                return false;
            }
        }

        strMasterNodePrivKey = c.sMasternodePrivKey;
        if (!strMasterNodePrivKey.empty()) {
            std::string errorMessage;

            CKey key;
            CPubKey pubkey;

            if (!darkSendSigner.SetKey(strMasterNodePrivKey, errorMessage, key, pubkey)) {
                errorMessage = "Invalid masternodeprivkey. Please see documenation.";
                return false;
            }

            activeMasternode.pubKeyMasternode = pubkey;
        } else {
            errorMessage = "You must specify a masternodeprivkey in the configuration. Please see documentation for help.";
            return false;
        }
    }
    return true;
}

void MasternodeManager::updateBitCannaNode(QString alias, QString addr, QString privkey, QString collateral, QString balance, QString index)
{
    LOCK(cs_adrenaline);
    bool bFound = false;
    int nodeRow = 0;

    QList<QTableWidgetItem *> items = ui->tableWidget_2->findItems(collateral, Qt::MatchExactly);

    if(items.count() > 0){
        bFound = true;
        nodeRow = items.at(0)->row();
    }

    std::string indexToDisplay = index.toStdString();

    if(nodeRow == 0 && !bFound) {
        ui->tableWidget_2->insertRow(0);
        indexToDisplay = "Masternode_"+std::to_string(ui->tableWidget_2->rowCount());
    }

    boost::replace_all(indexToDisplay, "_", " ");

    QTableWidgetItem *statusItem = new QTableWidgetItem("Inactive");

    BOOST_FOREACH(CMasterNode mn, vecMasternodes)
    {
        if(QString::fromStdString(mn.addr.ToString()) == addr && mn.IsEnabled()) {
            statusItem->setText("Active");
        }
    }

    QTableWidgetItem *aliasItem = new QTableWidgetItem(QString::fromStdString(indexToDisplay));
    QTableWidgetItem *addrItem = new QTableWidgetItem(addr);
    QTableWidgetItem *collateralItem = new QTableWidgetItem(collateral);
    QTableWidgetItem *balanceItem = new QTableWidgetItem(balance);

    CBitCannaNodeConfig c;
    c.sIndex = index.toStdString();
    c.sAddress = addr.toStdString();
    c.sAlias = alias.toStdString();
    c.sCollateralAddress = collateral.toStdString();
    c.sMasternodePrivKey = privkey.toStdString();

    std::string errorMessage;
    setMasterNodeForIX(c, errorMessage);

    ui->tableWidget_2->setItem(nodeRow, 0, aliasItem);
    ui->tableWidget_2->setItem(nodeRow, 1, addrItem);
    ui->tableWidget_2->setItem(nodeRow, 2, statusItem);
    ui->tableWidget_2->setItem(nodeRow, 3, collateralItem);
    ui->tableWidget_2->setItem(nodeRow, 4, balanceItem);
}

static QString seconds_to_DHMS(quint32 duration)
{
  QString res;
  int seconds = (int) (duration % 60);
  duration /= 60;
  int minutes = (int) (duration % 60);
  duration /= 60;
  int hours = (int) (duration % 24);
  int days = (int) (duration / 24);
  if((hours == 0)&&(days == 0))
      return res.sprintf("%02dm:%02ds", minutes, seconds);
  if (days == 0)
      return res.sprintf("%02dh:%02dm:%02ds", hours, minutes, seconds);
  return res.sprintf("%dd %02dh:%02dm:%02ds", days, hours, minutes, seconds);
}

void MasternodeManager::updateNodeList()
{
    TRY_LOCK(cs_masternodes, lockMasternodes);
    if(!lockMasternodes)
        return;

    ui->countLabel->setText("Updating...");
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    BOOST_FOREACH(CMasterNode mn, vecMasternodes)
    {
        int mnRow = 0;
        ui->tableWidget->insertRow(0);
//        mn.UpdateLastSeen();

 	// populate list
	// Address, Rank, Active, Active Seconds, Last Seen, Pub Key
    QTableWidgetItem *activeItem = new QTableWidgetItem(QString(mn.IsEnabled() ? "Yes" : "No"));
    QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(GetMasternodeRank(mn.vin, chainActive.Height())));
	QTableWidgetItem *activeSecondsItem = new QTableWidgetItem(seconds_to_DHMS((qint64)(mn.lastTimeSeen - mn.now)));
    QTableWidgetItem *lastSeenItem = new QTableWidgetItem(QString::fromStdString(DateTimeStrFormat("%Y-%m-%d %H:%M:%S", mn.lastTimeSeen)));
	
	CScript pubkey;
        pubkey =GetScriptForDestination(mn.pubkey.GetID());
        CTxDestination address1;
        ExtractDestination(pubkey, address1);
        CBitcoinAddress address2(address1);
	QTableWidgetItem *pubkeyItem = new QTableWidgetItem(QString::fromStdString(address2.ToString()));
	
    ui->tableWidget->setItem(mnRow, 0, rankItem);
    ui->tableWidget->setItem(mnRow, 1, activeItem);
    ui->tableWidget->setItem(mnRow, 2, activeSecondsItem);
    ui->tableWidget->setItem(mnRow, 3, pubkeyItem);

    }

    ui->countLabel->setText(QString::number(ui->tableWidget->rowCount()));

    if(pwalletMain)
    {
        LOCK(cs_adrenaline);
        CAmount balance;
        QString strBalance;
        BOOST_REVERSE_FOREACH(PAIRTYPE(std::string, CBitCannaNodeConfig) adrenaline, pwalletMain->mapMyBitCannaNodes)
        {
            balance = GetAccountBalance(adrenaline.second.sIndex, 1, ISMINE_SPENDABLE);
            strBalance = BitcoinUnits::simpleFormat(0, balance, false, BitcoinUnits::separatorAlways, 2);
            updateBitCannaNode(QString::fromStdString(adrenaline.second.sAlias), QString::fromStdString(adrenaline.second.sAddress), QString::fromStdString(adrenaline.second.sMasternodePrivKey), QString::fromStdString(adrenaline.second.sCollateralAddress), strBalance, QString::fromStdString(adrenaline.second.sIndex));
        }
    }
}

void MasternodeManager::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
    }
}

void MasternodeManager::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
    }

}

void MasternodeManager::on_createButton_clicked()
{
    AddEditBitCannaNode* aenode = new AddEditBitCannaNode();
    aenode->exec();
}

void MasternodeManager::on_copyAddressButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    std::string sCollateralAddress = ui->tableWidget_2->item(r, 3)->text().toStdString();

    QApplication::clipboard()->setText(QString::fromStdString(sCollateralAddress));
}

void MasternodeManager::on_editButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    QString sAlias = ui->tableWidget_2->item(r, 0)->text();
    std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
    CBitCannaNodeConfig& c = pwalletMain->mapMyBitCannaNodes["Masternode_"+std::to_string(r+1)];
    std::string sPrivKey = c.sMasternodePrivKey;
    std::string sTxHash = "";

    BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
        if(mne.getIp() == sAddress) {
            sTxHash = mne.getTxHash();
            break;
        }
    }


    //TODO move to constructor
    EditMasterNodeDialog* editMNDialog = new EditMasterNodeDialog(this, sAlias, QString::fromStdString(sAddress), QString::fromStdString(sPrivKey), QString::fromStdString(sTxHash));
    if(editMNDialog->exec()) {
        CAmount balance = GetAccountBalance(editMNDialog->index, 1, ISMINE_SPENDABLE);;
        QString strBalance = BitcoinUnits::simpleFormat(0, balance, false, BitcoinUnits::separatorAlways, 2);;
        c.sAddress = editMNDialog->newIp;
        ui->tableWidget_2->item(r, 1)->setText(QString::fromStdString(editMNDialog->newIp));
        ui->tableWidget_2->item(r, 4)->setText(strBalance);
        CAccount account;
        CWalletDB walletdb(pwalletMain->strWalletFile);
        walletdb.ReadAccount(c.sIndex, account);
        pwalletMain->SetAddressBook(account.vchPubKey.GetID(), c.sIndex, "");
    }

}

void MasternodeManager::on_getConfigButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
    CBitCannaNodeConfig c = pwalletMain->mapMyBitCannaNodes[sAddress];
    std::string sPrivKey = c.sMasternodePrivKey;
    BitCannaNodeConfigDialog* d = new BitCannaNodeConfigDialog(this, QString::fromStdString(sAddress), QString::fromStdString(sPrivKey));
    d->exec();
}

void MasternodeManager::on_removeButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Delete Masternode?", "Are you sure you want to delete this Masternode configuration?", QMessageBox::Yes|QMessageBox::No);

    if(confirm == QMessageBox::Yes)
    {
        QModelIndex index = selected.at(0);
        int r = index.row();
        std::string mnIndex = "Masternode_"+std::to_string(r+1);
        CBitCannaNodeConfig c = pwalletMain->mapMyBitCannaNodes[mnIndex];

        std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();

        std::string errorMessage;
        bool result = activeMasternode.StopMasterNode(c.sAddress, c.sMasternodePrivKey, errorMessage, masternodeConfig.findEntryByIp(sAddress).getVin());
        QMessageBox msg;
        if(result) {
            msg.setText("Masternode at " + QString::fromStdString(c.sAddress) + " stopped.");
            fMasterNode = false;
        } else {
            msg.setText("Error: " + QString::fromStdString(errorMessage));
        }
        msg.exec();

        CWalletDB walletdb(pwalletMain->strWalletFile);
        pwalletMain->mapMyBitCannaNodes.erase(mnIndex);
        masternodeConfig.remove(mnIndex);
        walletdb.EraseBitCannaNodeConfig(c.sIndex);
        ui->tableWidget_2->clearContents();
        ui->tableWidget_2->setRowCount(0);
        BOOST_FOREACH(PAIRTYPE(std::string, CBitCannaNodeConfig) adrenaline, pwalletMain->mapMyBitCannaNodes)
        {
            updateBitCannaNode(QString::fromStdString(adrenaline.second.sAlias), QString::fromStdString(adrenaline.second.sAddress), QString::fromStdString(adrenaline.second.sMasternodePrivKey), QString::fromStdString(adrenaline.second.sCollateralAddress), "", QString::fromStdString(adrenaline.second.sIndex));
        }
    }
}

void MasternodeManager::on_startButton_clicked()
{
    // start the node
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    std::string mnIndex = "Masternode_"+std::to_string(r+1);
    CBitCannaNodeConfig c = pwalletMain->mapMyBitCannaNodes[mnIndex];

    std::string errorMessage;

    QMessageBox msg;
    bool result = false;

    if(setMasterNodeForIX(c, errorMessage)) {
        result = activeMasternode.RegisterByPubKey(c.sAddress, c.sMasternodePrivKey, c.sCollateralAddress, errorMessage);
    }

    if(result) {
        msg.setText("Masternode at " + QString::fromStdString(c.sAddress) + " started.");
    }
    else
        msg.setText("Error: " + QString::fromStdString(errorMessage));

    msg.exec();
}

void MasternodeManager::on_stopButton_clicked()
{
    // start the node
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    std::string mnIndex = "Masternode_"+std::to_string(r+1);
    std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
    CBitCannaNodeConfig c = pwalletMain->mapMyBitCannaNodes[mnIndex];

    std::string errorMessage;
    bool result = activeMasternode.StopMasterNode(c.sAddress, c.sMasternodePrivKey, errorMessage, masternodeConfig.findEntryByIp(sAddress).getVin());
    QMessageBox msg;
    if(result) {
        msg.setText("Masternode at " + QString::fromStdString(c.sAddress) + " stopped.");
        fMasterNode = false;
    } else {
        msg.setText("Error: " + QString::fromStdString(errorMessage));
    }
    msg.exec();
}

void MasternodeManager::on_actionsBox_activated(int index)
{
    switch (index) {
    case 0:
        break;
    case 1:
        on_editButton_clicked();
//        on_getConfigButton_clicked();
        break;
    case 2:
        on_removeButton_clicked();
        break;
    default:
        break;
    }
}
