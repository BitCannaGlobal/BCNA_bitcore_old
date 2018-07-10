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
#include <boost/lexical_cast.hpp>
#include <fstream>

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

MasternodeManager::MasternodeManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MasternodeManager),
    clientModel(0),
    walletModel(0)
{
    ui->setupUi(this);

    ui->editButton->setEnabled(false);
    ui->getConfigButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->copyAddressButton->setEnabled(false);

    subscribeToCoreSignals();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateNodeList()));
    if(!GetBoolArg("-reindexaddr", false))
        timer->start(1000);
        fFilterUpdated = true;
	nTimeFilterUpdated = GetTime();

    updateNodeList();
}

MasternodeManager::~MasternodeManager()
{
    delete ui;
}

static void NotifyBitCannaNodeUpdated(MasternodeManager *page, CBitCannaNodeConfig nodeConfig)
{
    // alias, address, privkey, collateral address
    QString alias = QString::fromStdString(nodeConfig.sAlias);
    QString addr = QString::fromStdString(nodeConfig.sAddress);
    QString privkey = QString::fromStdString(nodeConfig.sMasternodePrivKey);
    QString collateral = QString::fromStdString(nodeConfig.sCollateralAddress);
    
    QMetaObject::invokeMethod(page, "updateBitCannaNode", Qt::QueuedConnection,
                              Q_ARG(QString, alias),
                              Q_ARG(QString, addr),
                              Q_ARG(QString, privkey),
                              Q_ARG(QString, collateral)
                              );
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
        ui->editButton->setEnabled(true);
        ui->getConfigButton->setEnabled(true);
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
	ui->copyAddressButton->setEnabled(true);
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

void MasternodeManager::updateBitCannaNode(QString alias, QString addr, QString privkey, QString collateral)
{
    LOCK(cs_adrenaline);
    bool bFound = false;
    int nodeRow = 0;
    for(int i=0; i < ui->tableWidget_2->rowCount(); i++)
    {
        if(ui->tableWidget_2->item(i, 0)->text() == alias)
        {
            bFound = true;
            nodeRow = i;
            break;
        }
    }

    if(nodeRow == 0 && !bFound)
        ui->tableWidget_2->insertRow(0);

    QTableWidgetItem *statusItem = new QTableWidgetItem("Inactive");

    BOOST_FOREACH(CMasterNode mn, vecMasternodes)
    {
        if(QString::fromStdString(mn.addr.ToString()) == addr && mn.IsEnabled()) {
            statusItem->setText("Active");
        }
    }

    QTableWidgetItem *aliasItem = new QTableWidgetItem(alias);
    QTableWidgetItem *addrItem = new QTableWidgetItem(addr);
    QTableWidgetItem *collateralItem = new QTableWidgetItem(collateral);

    CBitCannaNodeConfig c;
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
	QTableWidgetItem *activeItem = new QTableWidgetItem(QString::number(mn.IsEnabled()));
	QTableWidgetItem *addressItem = new QTableWidgetItem(QString::fromStdString(mn.addr.ToString()));
	QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(GetMasternodeRank(mn.vin, chainActive.Tip()->nHeight)));
	QTableWidgetItem *activeSecondsItem = new QTableWidgetItem(seconds_to_DHMS((qint64)(mn.lastTimeSeen - mn.now)));
	QTableWidgetItem *lastSeenItem = new QTableWidgetItem(QString::fromStdString(DateTimeStrFormat("%Y-%m-%d %H:%M:%S", mn.lastTimeSeen)));
	
	CScript pubkey;
        pubkey =GetScriptForDestination(mn.pubkey.GetID());
        CTxDestination address1;
        ExtractDestination(pubkey, address1);
        CBitcoinAddress address2(address1);
	QTableWidgetItem *pubkeyItem = new QTableWidgetItem(QString::fromStdString(address2.ToString()));
	
	ui->tableWidget->setItem(mnRow, 0, addressItem);
	ui->tableWidget->setItem(mnRow, 1, rankItem);
	ui->tableWidget->setItem(mnRow, 2, activeItem);
	ui->tableWidget->setItem(mnRow, 3, activeSecondsItem);
	ui->tableWidget->setItem(mnRow, 4, lastSeenItem);
	ui->tableWidget->setItem(mnRow, 5, pubkeyItem);
    }

    ui->countLabel->setText(QString::number(ui->tableWidget->rowCount()));

    if(pwalletMain)
    {
        LOCK(cs_adrenaline);
        BOOST_FOREACH(PAIRTYPE(std::string, CBitCannaNodeConfig) adrenaline, pwalletMain->mapMyBitCannaNodes)
        {
            updateBitCannaNode(QString::fromStdString(adrenaline.second.sAlias), QString::fromStdString(adrenaline.second.sAddress), QString::fromStdString(adrenaline.second.sMasternodePrivKey), QString::fromStdString(adrenaline.second.sCollateralAddress));
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
    CBitCannaNodeConfig c = pwalletMain->mapMyBitCannaNodes[sAddress];
    std::string sPrivKey = c.sMasternodePrivKey;
    std::string sTxHash = "";

    BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
        if(mne.getIp() == sAddress) {
            sTxHash = mne.getTxHash();
            break;
        }
    }

    EditMasterNodeDialog* editMNDialog = new EditMasterNodeDialog(this, sAlias, QString::fromStdString(sAddress), QString::fromStdString(sPrivKey), QString::fromStdString(sTxHash));
    editMNDialog->exec();

    // get existing config entry

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
    confirm = QMessageBox::question(this, "Delete Adrenaline Node?", "Are you sure you want to delete this adrenaline node configuration?", QMessageBox::Yes|QMessageBox::No);

    if(confirm == QMessageBox::Yes)
    {
        QModelIndex index = selected.at(0);
        int r = index.row();
        std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
        CBitCannaNodeConfig c = pwalletMain->mapMyBitCannaNodes[sAddress];
        CWalletDB walletdb(pwalletMain->strWalletFile);
        pwalletMain->mapMyBitCannaNodes.erase(sAddress);
        masternodeConfig.remove(sAddress);
        walletdb.EraseBitCannaNodeConfig(c.sAddress);
        ui->tableWidget_2->clearContents();
        ui->tableWidget_2->setRowCount(0);
        BOOST_FOREACH(PAIRTYPE(std::string, CBitCannaNodeConfig) adrenaline, pwalletMain->mapMyBitCannaNodes)
        {
            updateBitCannaNode(QString::fromStdString(adrenaline.second.sAlias), QString::fromStdString(adrenaline.second.sAddress), QString::fromStdString(adrenaline.second.sMasternodePrivKey), QString::fromStdString(adrenaline.second.sCollateralAddress));
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
    std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
    CBitCannaNodeConfig c = pwalletMain->mapMyBitCannaNodes[sAddress];

    std::string errorMessage;
    std::string txHash = masternodeConfig.findEntryByIp(sAddress).getTxHash();

    QMessageBox msg;
    bool result = false;

    if(txHash == "" || txHash == "false" || txHash.size() != 64) { // todo sanitize txHash
        errorMessage = "Please enter valid TX hash in the edit dialog\n";
    } else {
        const CWalletTx& wtx = pwalletMain->mapWallet[uint256(txHash)];

        int confirms = wtx.GetDepthInMainChain(false);
        int confirmsTotal = GetIXConfirmations(wtx.GetHash()) + confirms;

        if(confirmsTotal < MASTERNODE_MIN_CONFIRMATIONS) {
            errorMessage = "Please try later. Masternode payment transaction must have at least " + std::to_string(MASTERNODE_MIN_CONFIRMATIONS) + " confirmations\n";
        } else {
            if(setMasterNodeForIX(c, errorMessage)) {
                result = activeMasternode.RegisterByPubKey(c.sAddress, c.sMasternodePrivKey, c.sCollateralAddress, errorMessage);
            }
        }
    }

    if(result) {
        msg.setText("Adrenaline Node at " + QString::fromStdString(c.sAddress) + " started.");
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
    std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
    CBitCannaNodeConfig c = pwalletMain->mapMyBitCannaNodes[sAddress];
    std::string sTxHash = masternodeConfig.findEntryByIp(sAddress).getTxHash();

    std::string errorMessage;
    bool result = activeMasternode.StopMasterNode(c.sAddress, c.sMasternodePrivKey, errorMessage, sTxHash);
    QMessageBox msg;
    if(result) {
        msg.setText("Adrenaline Node at " + QString::fromStdString(c.sAddress) + " stopped.");
        fMasterNode = false;
    } else {
        msg.setText("Error: " + QString::fromStdString(errorMessage));
    }
    msg.exec();
}

void MasternodeManager::on_startAllButton_clicked()
{
    std::string results;
    BOOST_FOREACH(PAIRTYPE(std::string, CBitCannaNodeConfig) adrenaline, pwalletMain->mapMyBitCannaNodes)
    {
        CBitCannaNodeConfig c = adrenaline.second;
        std::string errorMessage;
        bool result = false;
        if(setMasterNodeForIX(c, errorMessage)) {
            result = activeMasternode.RegisterByPubKey(c.sAddress, c.sMasternodePrivKey, c.sCollateralAddress, errorMessage);
        }
        if(result) {
            results += c.sAddress + ": STARTED\n";
        } else {
            results += c.sAddress + ": ERROR: " + errorMessage + "\n";
        }
    }

    QMessageBox msg;
    msg.setText(QString::fromStdString(results));
    msg.exec();
}

void MasternodeManager::on_stopAllButton_clicked()
{
    std::string results;
    BOOST_FOREACH(PAIRTYPE(std::string, CBitCannaNodeConfig) adrenaline, pwalletMain->mapMyBitCannaNodes)
    {
        CBitCannaNodeConfig c = adrenaline.second;
        std::string errorMessage;
        bool result = activeMasternode.StopMasterNode(c.sAddress, c.sMasternodePrivKey, errorMessage);
        if(result) {
            results += c.sAddress + ": STOPPED\n";
            fMasterNode = false;
        } else {
            results += c.sAddress + ": ERROR: " + errorMessage + "\n";
        }
    }

    QMessageBox msg;
    msg.setText(QString::fromStdString(results));
    msg.exec();
}


