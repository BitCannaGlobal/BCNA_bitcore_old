#include "editmasternodedialog.h"
#include "ui_editmasternodedialog.h"

#include <boost/foreach.hpp>
#include "masternodeconfig.h"
#include <QModelIndex>

EditMasterNodeDialog::EditMasterNodeDialog(QWidget *parent, QString alias, QString address, QString privateKey, QString txHash) :
    QDialog(parent),
    ui(new Ui::EditMasterNodeDialog)
{
    ui->setupUi(this);
    ui->editMNDialogAliasEdit->setText(alias);
    ui->editMNDialogAddressEdit->setText(address);
    ui->editMNDialogPrivateKeyEdit->setText(privateKey);
    ui->editMNDialogTxEdit->setText(txHash);

    sAddress = address.toStdString();
}

EditMasterNodeDialog::~EditMasterNodeDialog()
{
    delete ui;
}

void EditMasterNodeDialog::on_buttonBox_accepted()
{
    BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry& mne, masternodeConfig.getEntries()) {
        if(mne.getIp() == sAddress) {
            mne.setAlias(ui->editMNDialogAliasEdit->text().toStdString());
            mne.setIp(ui->editMNDialogAddressEdit->text().toStdString());
            mne.setPrivKey(ui->editMNDialogPrivateKeyEdit->text().toStdString());
            mne.setTxHash(ui->editMNDialogTxEdit->text().toStdString());
            break;
        }
    }
}
