#include "editmasternodedialog.h"
#include "ui_editmasternodedialog.h"

#include <boost/foreach.hpp>
#include "masternodeconfig.h"
#include "netbase.h"
#include <QMessageBox>
#include <QModelIndex>

EditMasterNodeDialog::EditMasterNodeDialog(QWidget *parent, QString alias, QString address, QString privateKey, QString txHash) :
    QDialog(parent),
    ui(new Ui::EditMasterNodeDialog)
{
    ui->setupUi(this);
    this->setModal( true );
    ui->editMNDialogAliasEdit->setText(alias);
    ui->editMNDialogAddressEdit->setText(address);
    ui->editMNDialogPrivateKeyEdit->setText(privateKey);
    ui->editMNDialogTxEdit->setText(txHash);

    connect( ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
             this,          SLOT(buttonClicked(QAbstractButton*)));

    sAddress = address.toStdString();
}

EditMasterNodeDialog::~EditMasterNodeDialog()
{
    delete ui;
}

void EditMasterNodeDialog::done(int r)
{
    if(QDialog::Accepted == r)  // ok was pressed
        {
            BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry& mne, masternodeConfig.getEntries()) {
                if(mne.getIp() == sAddress) {
                    QMessageBox msg;
                    if(ui->editMNDialogAliasEdit->text() == "") {
                        msg.setText("Please enter an alias.");
                        msg.exec();
                        return;
                    } else if(ui->editMNDialogAddressEdit->text() == "") {
                        msg.setText("Please enter an address.");
                        msg.exec();
                        return;
                    } else {
                        CService ipPort((ui->editMNDialogAddressEdit->text()).toStdString(), 13700);
                        if(!ipPort.IsIPv4()) {
                            msg.setText("Please enter a valid address.");
                            msg.exec();
                            return;
                        }
                        mne.setIp(ui->editMNDialogAddressEdit->text().toStdString());
                        this->newIp = ui->editMNDialogAddressEdit->text().toStdString();
                        this->index = mne.getIndex();
                        QDialog::done(r);
                        return;
                    }
                }
        }
    }
    else    // cancel, close or exc was pressed
    {
        QDialog::done(r);
        return;
    }
}
