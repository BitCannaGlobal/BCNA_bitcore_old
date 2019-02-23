#include "addeditbitcannanode.h"
#include "ui_addeditbitcannanode.h"

#include "walletdb.h"
#include "wallet.h"
#include "ui_interface.h"
#include "util.h"
#include "key.h"
#include "script/script.h"
#include "init.h"
#include "base58.h"
#include "masternodeconfig.h"
#include "netbase.h"
#include <QMessageBox>

AddEditBitCannaNode::AddEditBitCannaNode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddEditBitCannaNode)
{
    ui->setupUi(this);

}

AddEditBitCannaNode::~AddEditBitCannaNode()
{
    delete ui;
}


void AddEditBitCannaNode::on_okButton_clicked()
{
    QMessageBox msg;
    if(ui->addressLineEdit->text() == "") {
        msg.setText("Please enter an address.");
        msg.exec();
        return;
    } else {
        CService ipPort((ui->addressLineEdit->text()).toStdString(), 13700);
        if(!ipPort.IsIPv4()) {
            msg.setText("Please enter a valid address.");
            msg.exec();
            return;
        }
        CBitCannaNodeConfig c;
        int index = masternodeConfig.getCount()+2;
        c.sAlias = "false"; // for compatibility
        c.sIndex = "Masternode_"+std::to_string(index);
        c.sAddress = ipPort.ToStringIPPort();
        CKey secret;
        secret.MakeNewKey(false);
        c.sMasternodePrivKey = CBitcoinSecret(secret).ToString();
	
        CWalletDB walletdb(pwalletMain->strWalletFile);
        CAccount account;
        walletdb.ReadAccount(c.sIndex, account);
        bool bKeyUsed = false;
        bool bForceNew = false;

        // Check if the current key has been used
        if (account.vchPubKey.IsValid())
        {
            CScript scriptPubKey = GetScriptForDestination(account.vchPubKey.GetID());
            for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin();
                 it != pwalletMain->mapWallet.end() && account.vchPubKey.IsValid();
                 ++it)
            {
                const CWalletTx& wtx = (*it).second;
                BOOST_FOREACH(const CTxOut& txout, wtx.vout)
                    if (txout.scriptPubKey == scriptPubKey)
                        bKeyUsed = true;
            }
        }

        // Generate a new key
        if (!account.vchPubKey.IsValid() || bForceNew || bKeyUsed)
        {
            if (!pwalletMain->GetKeyFromPool(account.vchPubKey))
            {
                QMessageBox msg;
                msg.setText("Keypool ran out, please call keypoolrefill first.");
                msg.exec();
                return;
            }
            pwalletMain->SetAddressBook(account.vchPubKey.GetID(), c.sIndex, "");
            walletdb.WriteAccount(c.sIndex, account);
        }

        c.sCollateralAddress = CBitcoinAddress(account.vchPubKey.GetID()).ToString();

        pwalletMain->mapMyBitCannaNodes.insert(make_pair(c.sIndex, c));
        walletdb.WriteBitCannaNodeConfig(c.sIndex, c);
        uiInterface.NotifyBitCannaNodeChanged(c);

        masternodeConfig.add(c.sIndex, c.sAlias, c.sAddress, c.sMasternodePrivKey, "false", "false");

        accept();
    }
}

void AddEditBitCannaNode::on_cancelButton_clicked()
{
    reject();
}

