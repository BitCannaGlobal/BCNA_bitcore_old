// -*- c++ -*-
#ifndef MASTERNODEMANAGER_H
#define MASTERNODEMANAGER_H

#include "util.h"
#include "sync.h"

#include <QWidget>
#include <QTimer>
#include <QFuture>
#include <QLabel>

#include "basetabledelegate.h"

#define MASTERNODELIST_UPDATE_SECONDS 15
#define MASTERNODELIST_FILTER_COOLDOWN_SECONDS 3

namespace Ui {
    class MasternodeManager;
}
class ClientModel;
class WalletModel;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Masternode Manager page widget */
class MasternodeManager : public QWidget
{
    Q_OBJECT

public:
    explicit MasternodeManager(QWidget *parent = 0);
    ~MasternodeManager();

    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);
    void updateListConc();

public slots:
    void updateNodeList();
    void updateBitCannaNode(QString alias, QString addr, QString privkey, QString collateral, QString balance = "", QString index = "");
    void tabChanged();
    
signals:

private:
    QTimer *timer;
    Ui::MasternodeManager *ui;
    ClientModel *clientModel;
    WalletModel *walletModel;
    BaseTableDelegate* networkMasterNodesTableDelegate;
    BaseTableDelegate* myMasterNodesTableDelegate;
    CCriticalSection cs_adrenaline;
    int64_t nTimeFilterUpdated;
    QLabel* mnNetLabel;
    QLabel* mnMyMasternodesLabel;
	bool fFilterUpdated;
	QFuture<void> f1;
    void subscribeToCoreSignals();
    void unsubscribeFromCoreSignals();
   
private slots:
    void on_copyAddressButton_clicked();
    void on_createButton_clicked();
    void on_editButton_clicked();
    void on_getConfigButton_clicked();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_removeButton_clicked();
    void on_tableWidget_2_itemSelectionChanged();
    void on_actionsBox_activated(int index);
};

#endif // MASTERNODEMANAGER_H
